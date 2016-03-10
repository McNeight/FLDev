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

#ifndef Fl_Dev_I18N_H
#define Fl_Dev_I18N_H

#pragma once

#include <string>

#define STR_MSG_SIZE 20

//the strings that are translated
//Main Mode
extern std::string  file_str,
                    mod_str,
                    insert_str,
                    overwrite_str,
                    row_str,
                    col_str,
                    cancel_str,
                    save_str,
                    discard_str;

extern std::string strmsg[STR_MSG_SIZE];
extern std::string line_label_str;

void init_strings();
std::string get_load_err_msg();

#endif // Fl_Dev_I18N_H
