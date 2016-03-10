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


#ifndef Fl_Dev_Callbacks_H
#define Fl_Dev_Callbacks_H

#pragma once

#include <FL/Fl.H>

//callback functions
void owbt_callback(Fl_Widget*, void*);
void pr_opt_cb(Fl_Widget*, void*);
void open_pr_cb();
void save_pr_cb();
void new_pr_cb();
void pref_cb(Fl_Widget*, void*);
void show_browser_cb();
void linenumbers_cb();
void show_nav_cb();
void generate_makefile_cb();
void b_cb(Fl_Widget* o, void*);
void manual_cb(Fl_Widget *, void *);
void ref_cb(Fl_Widget *, void *);
void recent_file_cb(Fl_Widget* w, void*);
void save_cb();
void saveas_cb();
void find2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);
void generate_makefile_cb();
void run_cb(Fl_Widget*, void*);
void make_(Fl_Widget* w, void* v, int mode = 0);

#endif // Fl_Dev_Callbacks_H