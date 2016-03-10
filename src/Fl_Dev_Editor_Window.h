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

#ifndef Fl_Dev_Editor_Window_H
#define Fl_Dev_Editor_Window_H

#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Text_Editor.H>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tabs.H>

#include "Fl_Dev_Code_Editor.h"
#include "Fl_Dev_Util.h"
#include "globals.h"


class Fl_Dev_Editor_Window : public Fl_Double_Window {
public:
    Fl_Dev_Editor_Window(int w, int h, const char* t);
    ~Fl_Dev_Editor_Window();
    void draw();
    void hide_output();
    void show_output();
    void hide_browser();
    void show_browser();
    void hide_linenrs();
    void show_linenrs();

    int handle(int event) {
        if (event == FL_KEYBOARD) return 0;
        else return	Fl_Double_Window::handle(event);
    }


    Fl_Window			*pr_options_dlg;
    Fl_Input           	*commandline;
    Fl_Check_Button		*console_check;
    Fl_Button          	*pr_ok_bt;

    Fl_Window          	*replace_dlg;
    Fl_Input           	*replace_find;
    Fl_Input           	*replace_with;
    Fl_Button          	*replace_all;
    Fl_Return_Button   	*replace_next;
    Fl_Button          	*replace_cancel;

    Fl_Button          	*outputwindowbutton;
    Fl_Tabs				*tabs;
    Fl_Box	       		*statusbar;
    Fl_Output			*fb_label;
    Fl_Menu_Button		*fb_home_btn;
    Fl_Dev_Code_Editor 	*editor;
    Fl_Dev_Code_Editor	*output;
    Fl_Box				*line_nr_box;

    Fl_File_Browser	 	*file_browser;
    Fl_File_Browser 	*pr_browser;
    char               	search[256];

};



#endif // Fl_Dev_Editor_Window_H
