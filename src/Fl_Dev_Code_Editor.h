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

#ifndef Fl_Dev_Code_Editor_H
#define Fl_Dev_Code_Editor_H

#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Text_Editor.H>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tabs.H>

#include "Fl_Dev_Util.h"
#include "globals.h"

//output for make, could be replaced by Fl_Browser in the future
class Fl_Dev_Code_Editor : public Fl_Text_Editor {
public:
    Fl_Dev_Code_Editor(int x, int y, int w, int h) : Fl_Text_Editor(x, y, w, h) {};

    int linepos(int line);
    int handle_key_ext();
    int handle(int event);
    //Fl_Color mCursor_color;
    char smart_indent; // really a bool, but FLTK uses a char for Fl_Button::value()
    void get_line_nrs(int *first_line, int *last_line);
    bool autobrace;

};

#endif // Fl_Dev_Code_Editor_H
