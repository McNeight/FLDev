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


#include "extras.h"
#include <iostream>
#include <stdio.h>

extern string get_load_err_msg();
extern bool auto_brace_mode;

//redefine Group and tile to remove keyboard navigation
int My_Group_wo_Nav::handle(int event) {
    if (event == FL_KEYBOARD) return 0;
    else return Fl_Group::handle(event);
}


int My_Tile_wo_Nav::handle(int event) {
    if (event == FL_KEYBOARD) return 0;
    else return Fl_Tile::handle(event);
}




void build_template_main() {
    char buf[255];
    int BUFSIZE = 255;
    sprintf(buf, "main.cpp");
    FILE *ptr = fopen(buf, "w");
    if (ptr != NULL)
    {
        fprintf(ptr, "#include <iostream>\n\nusing namespace std;\n\n\n\nint main(int argc, char *argv[])\n{\n\tcout << \"Hello World!\\n\";\n\treturn 0;\n}\n");
        fclose(ptr);
    }
}


int create_file(const char *file) {
    FILE *ptr = fopen(file, "w");
    if (ptr != NULL)
    {

        fclose(ptr);
        return 1;
    }
    return 0;
}






static void kill_selection(Fl_Text_Editor* e) {
    if (e->buffer()->selected())
    {
        e->insert_position(e->buffer()->primary_selection()->start());
        e->buffer()->remove_selection();
    }
}



int Fl_Text_Editor_ext::handle(int event) {
    if (!buffer()) return 0;


    if (event == FL_KEYBOARD)
    {
//int key = Fl::event_key(), state = Fl::event_state(), c = Fl::event_text()[0];
//if(key == 'f' && state == FL_CTRL) return parent()->parent()->parent()->handle(event);
        return handle_key_ext();
    }


    return Fl_Text_Editor::handle(event);
}


bool autobrace;

int Fl_Text_Editor_ext::handle_key_ext() {


    int del;
    if (Fl::compose(del))
    {
        if (del) buffer()->select(insert_position() - del, insert_position());
    //    kill_selection(this);
        if (Fl::event_length())
        {
            if (insert_mode())
            {
                insert(Fl::event_text());
                if (strcmp(Fl::event_text(), "{") == 0 && auto_brace_mode)
                {
                    autobrace = true;
                }
                else autobrace = false;
            }
            else overstrike(Fl::event_text());
        }
        show_insert_position();
        if (when()&FL_WHEN_CHANGED) do_callback();
        else set_changed();
        return 1;
    }
      //cout << line << "end" << endl;


    int key = Fl::event_key(), state = Fl::event_state(), c = Fl::event_text()[0];
    state &= FL_SHIFT | FL_CTRL | FL_ALT | FL_META; // only care about these states




    if (Fl::event_key() == FL_Enter && smart_indent)
    {
        char *line = '\0';
        line = strdup(buffer()->line_text(insert_position()));
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ' && line[i] != '\t') line[i] = '\0';
        }
        kf_enter(c, this);
        insert(line);

        if (autobrace)
        {
            int inspos = insert_position();
            kf_enter(c, this);
            insert(line);
            insert("}");
            insert_position(inspos);
            insert("\t");
        }
        autobrace = false;
        return 1;
    }


    else if (Fl::event_key() == FL_Home)
    {
        int pos;
        char *line = '\0';

        line = strdup(buffer()->line_text(insert_position()));
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ' && line[i] != '\t') { pos = i; break; }
        }
        int old = insert_position();
        insert_position(buffer()->line_start(insert_position()) + pos);
        if (state == FL_SHIFT) buffer()->select(insert_position(), old);
        if (insert_position() != old) return 1;
    }
    else 0;

    Key_Func f;
    f = bound_key_function(key, state, global_key_bindings);
    if (!f) f = bound_key_function(key, state, key_bindings);
    if (f) return f(key, this);
    if (default_key_function_ && !state) return default_key_function_(c, this);
    return 0;

}


void Fl_Text_Editor_ext::get_line_nrs(int *first_line, int *last_line) {
    *first_line = mTopLineNum; //get_absolute_top_line_number();
    *last_line = *first_line + mNVisibleLines;//textD->nVisibleLines;	
}


