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


#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once

#include <FL/Fl.H>
#include <FL/x.H>

#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H> 
#include <FL/Fl_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Button.H>

#include "extras.h" 
#include "NavBrowser.h" 
#include "pref_form.h"
#include "proj_form.h"
#include "proj_wiz_form.h"


void set_title(Fl_Window* w);
void add_recent_file_to_menu(const char *filename);
void add_recent_project_to_menu(const char *filename);
void save_config_file();
void set_text_size(int t);
int check_save();
int load_file(const char *, int);
void replace_text_in_menu(int index, char *newtext);
void removebackupfiles();
void add_nav_timeout_handler();

Fl_Window* make_form();

class Fl_Dev_Editor_Window;
class Fl_Dev_Project;
class Fl_Text_Editor_ext;

extern Fl_Text_Buffer *outputtb;
extern char filename[256];
extern Fl_Dev_Project project;
extern Fl_Dev_Editor_Window* window;
/*
Project project;
Fl_Dev_File_History *file_hist;
Fl_Menu_Bar* menubar;
Fl_Help_Dialog *help_dialog;
Fl_Group *smartbar;
Fl_Group *output_grp;
Fl_Tile *tile;
*/

extern Fl_Text_Buffer     *textbuf;
extern Fl_Text_Buffer     *stylebuf;
extern Fl_Text_Buffer		*op_stylebuf;
extern Fl_Text_Editor_ext 		*te;



//Editor colors
extern Fl_Color hl_plain;
extern Fl_Color hl_linecomment;
extern Fl_Color hl_blockcomment;
extern Fl_Color hl_string;
extern Fl_Color hl_directive;
extern Fl_Color hl_type;
extern Fl_Color hl_keyword;
extern Fl_Color hl_character;
extern Fl_Color background_color;


extern bool cppfile;
extern NavBrowser *navigator_browser;
extern Fl_Group *browser_file_grp, *browser_nav_grp;


#define TEXTSIZE 10

extern Fl_Text_Display::Style_Table_Entry op_styletable[];
extern Fl_Text_Display::Style_Table_Entry styletable[];

extern const char         *code_keywords[];
extern const char         *code_types[];

#define FL_MENU_RECENT_FILES 4
#define FL_MENU_RECENT_PROJECTS 48

#endif
