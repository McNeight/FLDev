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

#include "Fl_Dev_Code_Editor.h"

int Fl_Dev_Code_Editor::handle(int event) {
    if (!buffer()) return 0;

    if (event == FL_KEYBOARD)
    {
        //int key = Fl::event_key(), state = Fl::event_state(), c = Fl::event_text()[0];
        //if(key == 'f' && state == FL_CTRL) return parent()->parent()->parent()->handle(event);
        return handle_key_ext();
    }

    if (!Fl::event_inside(text_area.x, text_area.y, text_area.w, text_area.h))
    {
        return Fl_Group::handle(event);
    }

    if (event == FL_PUSH)
    {
        if (Fl::focus() != this)
        {
            Fl::focus(this);
            handle(FL_FOCUS);
        }

        int pos = xy_to_position(Fl::event_x(), Fl::event_y(), CURSOR_POS);
        buffer()->select(buffer()->line_start(pos), buffer()->line_end(pos) + 1);
        if (Fl::event_clicks() > 0)
        {
            Fl::event_clicks(-1);
            char *buf = outputtb->line_text(pos);
            if (buf == NULL) return 0;
            char *t_filename = strsep(&buf, ":");
            if (buf == NULL) return 0;
            char *linenr = strsep(&buf, ":");
            if (buf == NULL) return 0;
            int line;

            if (strcmp(t_filename, filename) != 0)
            {
                if (!check_save()) return 0;

                if (t_filename == "") return 0;
                if (t_filename != NULL)
                {
                    load_file(t_filename, -1);
                    add_recent_file_to_menu(filename);
                }
            }

            if (sscanf(linenr, "%d", &line))
            {
                Fl::focus(te);
                te->buffer()->unselect();
                int txtpos = linepos(line);
                te->insert_position(te->line_start(txtpos - 1));
                te->buffer()->select(te->buffer()->line_start(txtpos - 1), te->buffer()->line_end(txtpos - 1) + 1);
                te->show_insert_position();
            }
        }
    }
    else if (event == FL_MOUSEWHEEL) return mVScrollBar->handle(event);
    //else return Fl_Group::handle(event);
    // Is this correct?
    else return Fl_Text_Editor::handle(event);

};

int Fl_Dev_Code_Editor::linepos(int line) {
    int ret = 0;
    int size = textbuf->length();
    while (ret < size)
    {
        if (textbuf->count_lines(0, textbuf->line_end(ret)) >= line) break;
        ret = textbuf->line_end(ret);
        ret++;
    }
    return ret;
}


int Fl_Dev_Code_Editor::handle_key_ext() {


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


void Fl_Dev_Code_Editor::get_line_nrs(int *first_line, int *last_line) {
    *first_line = mTopLineNum; //get_absolute_top_line_number();
    *last_line = *first_line + mNVisibleLines;//textD->nVisibleLines;	
}
