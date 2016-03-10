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





