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

#ifndef Fl_Dev_Project_H
#define Fl_Dev_Project_H

#pragma once

#include <string>
#include <deque>

#include <FL/Fl_File_Browser.H>

class Fl_Dev_Project {

public:
    bool assigned;
    std::string pr_filename;
    std::string name;
    std::string pr_dir;
    std::string own_makefile;

    int run_in_console;
    int use_own_makefile;

    std::string binfilename;
    std::string src_files;
    std::string header_files;
    std::string gui_files;


    std::string oDir;
    std::string Bin;
    std::string libs;
    std::string libdirs;
    std::string incdirs;
    std::string ldflags;
    std::string srcs;
    std::deque<std::string> exobjs;
    std::string cflags;
    std::string gdbflags;

    bool modified;

    Fl_Dev_Project() {
        oDir = ".";
        Bin = ".";
        std::string cflags = "-O2";
        std::string gdbflags = "-s";
        assigned = false;
        modified = false;
    }

    void addToBrowser(Fl_File_Browser *b);
    void save();
    int load();
};

#endif // Fl_Dev_Project_H