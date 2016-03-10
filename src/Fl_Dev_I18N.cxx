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

#include "Fl_Dev_I18N.h"

//the strings that are translated
//Main Mode
std::string file_str = "File",
             mod_str = "modified",
          insert_str = "INSERT",
       overwrite_str = "OVERWRITE",
             row_str = "row",
             col_str = "col",
          cancel_str = "Cancel",
            save_str = "Save",
         discard_str = "Discard";

std::string strmsg[STR_MSG_SIZE];
std::string line_label_str;

void init_strings() {
    strmsg[0] = "Cpp-Reference not found!\nDo you want to download?";
    strmsg[1] = "The current file has not been saved.\nWould you like to save it now?";
    strmsg[2] = "The current Project has not been saved.\nWould you like to save it now?";
    strmsg[3] = "Open file";
    strmsg[4] = "Insert file";
    strmsg[5] = "Open Project File";
    strmsg[6] = "Rebuild Makefile?";
    strmsg[7] = "Save File As?";
    strmsg[8] = "Search String";
    strmsg[9] = "Replaced %d occurrences.";
    strmsg[10] = "You have to restart FLDev!";
    strmsg[11] = "Error reading from file";
    strmsg[12] = "Error writing to file";
    strmsg[13] = "Program already running!";
    strmsg[14] = "Name of executable file not defined.\nCheck project options!";
    strmsg[15] = "No occurrences of \'%s\' found!";
    strmsg[16] = "Error reading config file!";
    strmsg[17] = "Project File not found!";
}

std::string get_load_err_msg() {
    return strmsg[17];
}
