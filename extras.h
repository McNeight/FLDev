//////////////////////////////////////////////////////////////////////////////////////////
/*
		FLDev
		
		Lightweight Integrated Development Environment

		version:	0.5.2
		author:		Philipp Pracht
		email:		pracht@informatik.uni-muenchen.de
		 
		descr.:	FLDev is a IDE designed for older systems and small C/C++ Applications
				and is based on the	Editor described in the FLTK Manual.

		 
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
		Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef EXTRAS_H
#define EXTRAS_H

#include <string>
#include <deque>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_File_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>

#include "proj_form.h"
//extern Fl_Menu_Bar* menubar;

using namespace std;

string trim(string const& source, char const* delims = " \t\r\n");
void build_template_main();

class File_History {
  public:
	class item {
	  public:
		string name;
		int position;
	};

	deque<item> items;

	void add(string fname, int pos);

	int getPosition(string fname);
};


class My_File_Browser : public Fl_File_Browser {
  public:
	My_File_Browser(int x, int y, int w, int h):Fl_File_Browser(x,y,w,h) { type(FL_HOLD_BROWSER); }
};



class Project {

  public:
	bool assigned;
	string pr_filename;
	string name;
	string pr_dir;
	string binfilename;
	string src_files;
	string header_files;


	string oDir;
	string Bin;
	string libs;
	string libdirs;
	string incdirs;
	string ldflags;
	string srcs;
	deque<string> exobjs;
	string cflags;

	bool modified;

	int run_in_console;

	Project() {
		oDir = ".";
		Bin = ".";
		string cflags = "-O";
		assigned = false;
		modified = false;
	}

	void addToBrowser(Fl_Hold_Browser *b);
	void save();
	int load();
};


class Fl_Text_Editor_ext : public Fl_Text_Editor {
  public:
	Fl_Text_Editor_ext(int x, int y, int w, int h):Fl_Text_Editor(x,y,w,h) {}
	int handle_key_ext();
	int handle(int event);

	bool smart_indent;
};


#endif
