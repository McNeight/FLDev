#include "NavBrowser.h" 

//
// "$Id: NavBrowser.cxx,v 1.9.2.12.2.8 2003/01/30 21:41:24 easysw Exp $"
//
// Browser widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include <FL/Fl.H>
#include "NavBrowser.h"
#include <FL/fl_draw.H>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// I modified this from the original Forms data to use a linked list
// so that the number of items in the browser and size of those items
// is unlimited.  The only problem is that the old browser used an
// index number to identify a line, and it is slow to convert from/to
// a pointer.  I use a cache of the last match to try to speed this
// up.

// Also added the ability to "hide" a line.  This set's it's height to
// zero, so the NavBrowser_ cannot pick it.

#define SELECTED 1
#define NOTDISPLAYED 2

struct FL_BLINE {	// data is in a linked list of these
  FL_BLINE* prev;
  FL_BLINE* next;
  void* data;
  short length;		// sizeof(txt)-1, may be longer than string
  char flags;		// selected, displayed
  char txt[1];		// start of allocated array
};

void* NavBrowser::item_first() const {return first;}

void* NavBrowser::item_next(void* l) const {return ((FL_BLINE*)l)->next;}

void* NavBrowser::item_prev(void* l) const {return ((FL_BLINE*)l)->prev;}

int NavBrowser::item_selected(void* l) const {
  return ((FL_BLINE*)l)->flags&SELECTED;}

void NavBrowser::item_select(void* l, int v) {
  if (v) ((FL_BLINE*)l)->flags |= SELECTED;
  else ((FL_BLINE*)l)->flags &= ~SELECTED;
}

FL_BLINE* NavBrowser::find_line(int line) const {
  int n; FL_BLINE* l;
  if (line == cacheline) return cache;
  if (cacheline && line > (cacheline/2) && line < ((cacheline+lines)/2)) {
    n = cacheline; l = cache;
  } else if (line <= (lines/2)) {
    n = 1; l = first;
  } else {
    n = lines; l = last;
  }
  for (; n < line && l; n++) l = l->next;
  for (; n > line && l; n--) l = l->prev;
  ((NavBrowser*)this)->cacheline = line;
  ((NavBrowser*)this)->cache = l;
  return l;
}

int NavBrowser::lineno(void* v) const {
  FL_BLINE* l = (FL_BLINE*)v;
  if (!l) return 0;
  if (l == cache) return cacheline;
  if (l == first) return 1;
  if (l == last) return lines;
  if (!cache) {
    ((NavBrowser*)this)->cache = first;
    ((NavBrowser*)this)->cacheline = 1;
  }
  // assumme it is near cache, search both directions:
  FL_BLINE* b = cache->prev;
  int bnum = cacheline-1;
  FL_BLINE* f = cache->next;
  int fnum = cacheline+1;
  int n = 0;
  for (;;) {
    if (b == l) {n = bnum; break;}
    if (f == l) {n = fnum; break;}
    if (b) {b = b->prev; bnum--;}
    if (f) {f = f->next; fnum++;}
  }
  ((NavBrowser*)this)->cache = l;
  ((NavBrowser*)this)->cacheline = n;
  return n;
}

FL_BLINE* NavBrowser::_remove(int line) {
  FL_BLINE* ttt = find_line(line);
  deleting(ttt);

  cacheline = line-1;
  cache = ttt->prev;
  if (ttt->prev) ttt->prev->next = ttt->next;
  else first = ttt->next;
  if (ttt->next) ttt->next->prev = ttt->prev;
  else last = ttt->prev;

  lines--;
  full_height_ -= item_height(ttt);
  return(ttt);
}

void NavBrowser::remove(int line) {
  if (line < 1 || line > lines) return;
  free(_remove(line));
}

void NavBrowser::insert(int line, FL_BLINE* t) {
  if (!first) {
    t->prev = t->next = 0;
    first = last = t;
  } else if (line <= 1) {
    inserting(first, t);
    t->prev = 0;
    t->next = first;
    t->next->prev = t;
    first = t;
  } else if (line > lines) {
    t->prev = last;
    t->prev->next = t;
    t->next = 0;
    last = t;
  } else {
    FL_BLINE* n = find_line(line);
    inserting(n, t);
    t->next = n;
    t->prev = n->prev;
    t->prev->next = t;
    n->prev = t;
  }
  cacheline = line;
  cache = t;
  lines++;
  full_height_ += item_height(t);
  redraw_line(t);
}

void NavBrowser::insert(int line, const char* newtext, void* d) {
  int l = strlen(newtext);
  FL_BLINE* t = (FL_BLINE*)malloc(sizeof(FL_BLINE)+l);
  t->length = (short)l;
  t->flags = 0;
  strcpy(t->txt, newtext);
  t->data = d;
  insert(line, t);
}

void NavBrowser::move(int to, int from) {
  if (from < 1 || from > lines) return;
  insert(to, _remove(from));
}

void NavBrowser::text(int line, const char* newtext) {
  if (line < 1 || line > lines) return;
  FL_BLINE* t = find_line(line);
  int l = strlen(newtext);
  if (l > t->length) {
    FL_BLINE* n = (FL_BLINE*)malloc(sizeof(FL_BLINE)+l);
    replacing(t, n);
    cache = n;
    n->data = t->data;
    n->length = (short)l;
    n->flags = t->flags;
    n->prev = t->prev;
    if (n->prev) n->prev->next = n; else first = n;
    n->next = t->next;
    if (n->next) n->next->prev = n; else last = n;
    free(t);
    t = n;
  }
  strcpy(t->txt, newtext);
  redraw_line(t);
}

void NavBrowser::data(int line, void* d) {
  if (line < 1 || line > lines) return;
  find_line(line)->data = d;
}

int NavBrowser::item_height(void* lv) const {
  FL_BLINE* l = (FL_BLINE*)lv;
  if (l->flags & NOTDISPLAYED) return 0;

  int hmax = 2; // use 2 to insure we don't return a zero!

  if (!l->txt[0]) {
    // For blank lines set the height to exactly 1 line!
    fl_font(textfont(), textsize());
    int hh = fl_height();
    if (hh > hmax) hmax = hh;
  }
  else {
    // do each column separately as they may all set different fonts:
    for (char* str = l->txt; *str; str++) {
      Fl_Font font = textfont(); // default font
      int tsize = textsize(); // default size
      while (*str==format_char()) {
	str++;
	switch (*str++) {
	case 'l': case 'L': tsize = 24; break;
	case 'm': case 'M': tsize = 18; break;
	case 's': tsize = 11; break;
	case 'b': font = (Fl_Font)(font|FL_BOLD); break;
	case 'i': font = (Fl_Font)(font|FL_ITALIC); break;
	case 'f': case 't': font = FL_COURIER; break;
	case 'B':
	case 'C': strtol(str, &str, 10); break;// skip a color number
	case 'F': font = (Fl_Font)strtol(str,&str,10); break;
	case 'S': tsize = strtol(str,&str,10); break;
	case 0: case '@': str--;
	case '.': goto END_FORMAT;
	}
      }
      END_FORMAT:
      char* ptr = str;
      for(;*str && (*str!=column_char()); str++) ;
      if (ptr < str) {
	fl_font(font, tsize); int hh = fl_height();
	if (hh > hmax) hmax = hh;
      }
      if (!*str) str --;
    }
  }

  return hmax; // previous version returned hmax+2!
}

int NavBrowser::item_width(void* v) const {
  char* str = ((FL_BLINE*)v)->txt;
  const int* i = column_widths();
  int ww = 0;

  while (*i) { // add up all tab-seperated fields
    char* e;
    for (e = str; *e && *e != column_char(); e++);
    if (!*e) break; // last one occupied by text
    str = e+1;
    ww += *i++;
  }

  // OK, we gotta parse the string and find the string width...
  int tsize = textsize();
  Fl_Font font = textfont();
  int done = 0;

  while (*str == format_char_ && str[1] && str[1] != format_char_) {
    str ++;
    switch (*str++) {
    case 'l': case 'L': tsize = 24; break;
    case 'm': case 'M': tsize = 18; break;
    case 's': tsize = 11; break;
    case 'b': font = (Fl_Font)(font|FL_BOLD); break;
    case 'i': font = (Fl_Font)(font|FL_ITALIC); break;
    case 'f': case 't': font = FL_COURIER; break;
    case 'B':
    case 'C': strtol(str, &str, 10); break;// skip a color number
    case 'F': font = (Fl_Font)strtol(str, &str, 10); break;
    case 'S': tsize = strtol(str, &str, 10); break;
    case '.':
      done = 1;
    case '@':
      str--;
      done = 1;
    }

    if (done)
      break;
  }

  if (*str == format_char_ && str[1])
    str ++;

  fl_font(font, tsize);
  return ww + int(fl_width(str)) + 6;
}

int NavBrowser::full_height() const {
  return full_height_;
}

int NavBrowser::incr_height() const {
  return textsize()+2;
}



int lastsub;
int subs[255];


void NavBrowser::clearsubs() {
	memset (subs, '\0', 255*sizeof(int));
  //	bzero(subs,255*sizeof(int));
}

void NavBrowser::item_draw(void* v, int X, int Y, int W, int H) const {
  char* str = ((FL_BLINE*)v)->txt;
  const int* i = column_widths();
  int maxsub = 0;

  while (W > 6) {	// do each tab-seperated field
    int w1 = W;	// width for this field
    char* e = 0; // pointer to end of field or null if none
    //if (*i) 
    { // find end of field and temporarily replace with 0
      for (e = str; *e && *e != column_char(); e++);
      if (*e) {*e = 0; } else e = 0;
    }
    int tsize = textsize();
    Fl_Font font = textfont();
    Fl_Color lcol = textcolor();
    Fl_Align talign = FL_ALIGN_LEFT;
    // check for all the @-lines recognized by XForms:
    while (*str == format_char() && *++str && *str != format_char()) {
      switch (*str++) {
      case 'l': case 'L': tsize = 24; break;
      case 'm': case 'M': tsize = 18; break;
      case 's': tsize = 11; break;
      case 'b': font = (Fl_Font)(font|FL_BOLD); break;
      case 'i': font = (Fl_Font)(font|FL_ITALIC); break;
      case 'f': case 't': font = FL_COURIER; break;
      case 'c': talign = FL_ALIGN_CENTER; break;
      case 'r': talign = FL_ALIGN_RIGHT; break;
      case 'B': 
	if (!(((FL_BLINE*)v)->flags & SELECTED)) {
	  fl_color((Fl_Color)strtol(str, &str, 10));
	  fl_rectf(X, Y, w1, H);
	} else strtol(str, &str, 10);
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
	fl_line(X+3, Y+H/2, X+w1-3, Y+H/2);
	fl_color(FL_LIGHT3);
	fl_line(X+3, Y+H/2+1, X+w1-3, Y+H/2+1);
	break;
      case 'u':
      case '_':
	fl_color(lcol);
	fl_line(X+3, Y+H-1, X+w1-3, Y+H-1);
	break;
      case '.':
	goto BREAK;
      case '@':
	str--; goto BREAK;
      }
    }
  BREAK:
    fl_font(font, tsize);
    if (((FL_BLINE*)v)->flags & SELECTED)
      lcol = fl_contrast(lcol, selection_color());
    if (!active_r()) lcol = fl_inactive(lcol);
    
    //replace  +- with line
    
    //w1 = 0;
    
    int sub = 0;
    while(1)
    {
    	if(strlen(str) > 1 && str[0]=='+' && str[1]=='-')
    	{
    		str += 2;
    		X += 20;
    		sub++;
    		//w1 += 5;
    	}
    	else break;
    }
    
    
    if(sub > 0)
    {
    	fl_line_style(0,2);
    	fl_color(200,200,200);
    	//fl_color(00,00,00);
    	fl_line(X-8,Y+7,X-1,Y+7);
    	
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
    	else */{ 
    		fl_line(X-7,Y+7,X-7,Y);
    		
    	}
    	fl_line_style(0);
    }
    
    if(sub > maxsub) maxsub = sub;
    
    fl_color(lcol);
    fl_draw(str, X+3, Y, w1-6, H, e ? Fl_Align(talign|FL_ALIGN_CLIP) : talign, 0, 0);
    w1 = (int)fl_width(str);
    if (!e) break; // no more fields...
    *e = column_char(); // put the seperator back
    X += w1;
    W -= w1;
    str = e+1;
  }
  lastsub = maxsub;
}

static const int no_columns[1] = {0};

NavBrowser::NavBrowser(int X, int Y, int W, int H, const char*l)
  : Fl_Browser_(X, Y, W, H, l) {
  column_widths_ = no_columns;
  lines = 0;
  full_height_ = 0;
  cacheline = 0;
  format_char_ = '@';
  column_char_ = '\t';
  first = last = cache = 0;
  type(FL_HOLD_BROWSER);
}

void NavBrowser::lineposition(int line, Fl_Line_Position pos) {
  if (line<1) line = 1;
  if (line>lines) line = lines;
  int p = 0;

  FL_BLINE* l;
  for (l=first; l && line>1; l = l->next) {
    line--; p += item_height(l);
  }
  if (l && (pos == BOTTOM)) p += item_height (l);

  int final = p, X, Y, W, H;
  bbox(X, Y, W, H);

  switch(pos) {
    case TOP: break;
    case BOTTOM: final -= H; break;
    case MIDDLE: final -= H/2; break;
  }
  
  if (final > (full_height() - H)) final = full_height() -H;
  position(final);
}

int NavBrowser::topline() const {
  return lineno(top());
}

void NavBrowser::clear() {
  for (FL_BLINE* l = first; l;) {
    FL_BLINE* n = l->next;
    void *d = l->data;
    if(d) free(d);
    free(l);
    l = n;
  }
  full_height_ = 0;
  first = 0;
  lines = 0;
  new_list();
}

void NavBrowser::add(const char* newtext, void* d) {
  insert(lines+1, newtext, d);
  //NavBrowser_::display(last);
}

const char* NavBrowser::text(int line) const {
  if (line < 1 || line > lines) return 0;
  return find_line(line)->txt;
}

void* NavBrowser::data(int line) const {
  if (line < 1 || line > lines) return 0;
  return find_line(line)->data;
}

int NavBrowser::select(int line, int v) {
  if (line < 1 || line > lines) return 0;
  return Fl_Browser_::select(find_line(line), v);
}

int NavBrowser::selected(int line) const {
  if (line < 1 || line > lines) return 0;
  return find_line(line)->flags & SELECTED;
}

void NavBrowser::show(int line) {
  FL_BLINE* t = find_line(line);
  if (t->flags & NOTDISPLAYED) {
    t->flags &= ~NOTDISPLAYED;
    full_height_ += item_height(t);
    if (Fl_Browser_::displayed(t)) redraw_lines();
  }
}

void NavBrowser::hide(int line) {
  FL_BLINE* t = find_line(line);
  if (!(t->flags & NOTDISPLAYED)) {
    full_height_ -= item_height(t);
    t->flags |= NOTDISPLAYED;
    if (Fl_Browser_::displayed(t)) redraw_lines();
  }
}

void NavBrowser::display(int line, int v) {
  if (line < 1 || line > lines) return;
  if (v) show(line); else hide(line);
}

int NavBrowser::visible(int line) const {
  if (line < 1 || line > lines) return 0;
  return !(find_line(line)->flags&NOTDISPLAYED);
}

int NavBrowser::value() const {
  return lineno(selection());
}

//
// End of "$Id: NavBrowser.cxx,v 1.9.2.12.2.8 2003/01/30 21:41:24 easysw Exp $".
//
