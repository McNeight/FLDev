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


#ifndef EXTRAS_H
#define EXTRAS_H

#include <string>
#include <deque>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_File_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Tile.H>

#include "proj_form.h"
//extern Fl_Menu_Bar* menubar;

using namespace std;

void build_template_main();
int create_file(const char *file);



class Nav_entry {
public:
    Nav_entry(string name, int pos) {
        this->name = name;
        this->pos = pos;
        this->depth = 0;
    }
    Nav_entry(string name, int pos, int depth) {
        this->name = name;
        this->pos = pos;
        this->depth = depth;
    }
    Nav_entry(string name, int pos, string type, string args) {
        this->name = name;
        this->pos = pos;
        this->type = type;
        this->args = args;
        this->depth = 0;
    }
    string name;
    string type;
    string args;
    int pos;
    int depth;
    static deque <Nav_entry> children;
};

class Class_stack_item {
public:
    Class_stack_item(string name, int depth) {
        this->name = name;
        this->depth = depth;
    }

    string name;
    int depth;
};


class My_Tile_wo_Nav : public Fl_Tile {
public:
    My_Tile_wo_Nav(int x, int y, int w, int h) : Fl_Tile(x, y, w, h) {}
    int handle(int event);
};

class My_Group_wo_Nav : public Fl_Group {
public:
    My_Group_wo_Nav(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {}
    int handle(int event);
};



class SmartButton : public Fl_Button {
public:
    SmartButton(int x, int y, int w, int h) : Fl_Button(x, y, w, h) {
        box(FL_NO_BOX);
        down_box(FL_DOWN_BOX);

    }

    void draw() {
        if (type() == FL_HIDDEN_BUTTON) return;
        Fl_Color col = value() ? selection_color() : color();
        draw_box(value() ? (down_box() ? down_box() : fl_down(box())) : box(), col);
        draw_label();
    }

};





class Fl_Text_Editor_ext : public Fl_Text_Editor {
public:
    Fl_Text_Editor_ext(int x, int y, int w, int h) :Fl_Text_Editor(x, y, w, h) {}
    int handle_key_ext();
    int handle(int event);
    //Fl_Color mCursor_color;
    bool smart_indent;
    void get_line_nrs(int *first_line, int *last_line);
};


#endif
