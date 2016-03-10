/*
    FLDev - FLTK Integrated Development Environment
    Copyright (C) 2005-2016  Philipp Pracht, Georg Potthast, Neil McNeight

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <string>
#include <cstdlib>
#include <cmath>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "Fl_Dev_Code_Browser.h"

#define SELECTED 1
#define NOTDISPLAYED 2

// WARNING:
//       Fl_File_Chooser.cxx also has a definition of this structure (FL_BLINE).
//       Changes to FL_BLINE *must* be reflected in Fl_File_Chooser.cxx as well.
//       This hack in Fl_File_Chooser should be solved.
//
struct FL_BLINE {	// data is in a linked list of these
    FL_BLINE* prev;
    FL_BLINE* next;
    void* data;
    Fl_Image* icon;
    short length;		// sizeof(txt)-1, may be longer than string
    char flags;		// selected, displayed
    char txt[1];		// start of allocated array
};

int lastsub;
int subs[255];

/**
DAFUQ?
*/
void Fl_Dev_Code_Browser::clearsubs() {
    memset(subs, '\0', 255 * sizeof(int));
    //	bzero(subs,255*sizeof(int));
}

void Fl_Dev_Code_Browser::item_draw(void* item, int X, int Y, int W, int H) const {
    FL_BLINE* l = (FL_BLINE*)item;
    char* str = l->txt;
    const int* i = column_widths();
    int maxsub = 0;

    bool first = true;	// for icon
    while (W > 6)
    {	// do each tab-separated field
        int w1 = W;	// width for this field
        char* e = 0; // pointer to end of field or null if none
                     // HERE THERE BE COLOR!
                     //if (*i) 
        { // find end of field and temporarily replace with 0
            for (e = str; *e && *e != column_char(); e++);
            if (*e) { *e = 0; }
            else e = 0;
        }
        // Icon drawing code
        if (first)
        {
            first = false;
            if (l->icon)
            {
                l->icon->draw(X + 2, Y + 1);	// leave 2px left, 1px above
                int iconw = l->icon->w() + 2;
                X += iconw; W -= iconw; w1 -= iconw;
            }
        }
        int tsize = textsize();
        Fl_Font font = textfont();
        Fl_Color lcol = textcolor();
        Fl_Align talign = FL_ALIGN_LEFT;
        // check for all the @-lines recognized by XForms:
        //#if defined(__GNUC__)
        //#warning FIXME This maybe needs to be more UTF8 aware now...?
        //#endif /*__GNUC__*/
        while (*str == format_char() && *++str && *str != format_char())
        {
            switch (*str++)
            {
                case 'l': case 'L': tsize = 24; break;
                case 'm': case 'M': tsize = 18; break;
                case 's': tsize = 11; break;
                case 'b': font = (Fl_Font)(font | FL_BOLD); break;
                case 'i': font = (Fl_Font)(font | FL_ITALIC); break;
                case 'f': case 't': font = FL_COURIER; break;
                case 'c': talign = FL_ALIGN_CENTER; break;
                case 'r': talign = FL_ALIGN_RIGHT; break;
                case 'B':
                    if (!(l->flags & SELECTED))
                    {
                        fl_color((Fl_Color)strtol(str, &str, 10));
                        fl_rectf(X, Y, w1, H);
                    }
                    else while (isdigit(*str & 255)) str++; // skip digits
                    break;
                case 'C':
                    lcol = (Fl_Color)strtol(str, &str, 10);
                    break;
                case 'F':
                    font = (Fl_Font)strtol(str, &str, 10);
                    break;
                case 'N':
                    lcol = FL_INACTIVE_COLOR;
                    break;
                case 'S':
                    tsize = strtol(str, &str, 10);
                    break;
                case '-':
                    fl_color(FL_DARK3);
                    fl_line(X + 3, Y + H / 2, X + w1 - 3, Y + H / 2);
                    fl_color(FL_LIGHT3);
                    fl_line(X + 3, Y + H / 2 + 1, X + w1 - 3, Y + H / 2 + 1);
                    break;
                case 'u':
                case '_':
                    fl_color(lcol);
                    fl_line(X + 3, Y + H - 1, X + w1 - 3, Y + H - 1);
                    break;
                case '.':
                    goto BREAK;
                case '@':
                    str--; goto BREAK;
            }
        }
    BREAK:
        fl_font(font, tsize);
        if (l->flags & SELECTED)
            lcol = fl_contrast(lcol, selection_color());
        if (!active_r()) lcol = fl_inactive(lcol);

        // DAFUQ?

        //replace  +- with line

        //w1 = 0;

        int sub = 0;
        while (1)
        {
            if (strlen(str) > 1 && str[0] == '+' && str[1] == '-')
            {
                str += 2;
                X += 20;
                sub++;
                //w1 += 5;
            }
            else break;
        }


        if (sub > 0)
        {
            fl_line_style(0, 2);
            fl_color(200, 200, 200);
            //fl_color(00,00,00);
            fl_line(X - 8, Y + 7, X - 1, Y + 7);

            /*if(lastsub < sub) {
            fl_line(X-7,Y+7,X-7,Y);
            subs[sub] = Y;
            }
            else if(lastsub > sub)
            {
            int nr = subs[lastsub] + 5;
            subs[sub] = Y;
            subs[lastsub] = 0;

            fl_line(X-7,Y+7,(X-7),nr);
            }
            else */
            {
                fl_line(X - 7, Y + 7, X - 7, Y);

            }
            fl_line_style(0);
        }

        if (sub > maxsub) maxsub = sub;

        fl_color(lcol);
        fl_draw(str, X + 3, Y, w1 - 6, H, e ? Fl_Align(talign | FL_ALIGN_CLIP) : talign, 0, 0);
        w1 = (int)fl_width(str);
        if (!e) break; // no more fields...
        *e = column_char(); // put the separator back
        X += w1;
        W -= w1;
        str = e + 1;
    }
    lastsub = maxsub;
}
