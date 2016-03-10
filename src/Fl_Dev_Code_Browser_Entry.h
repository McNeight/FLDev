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

#ifndef Fl_Dev_Code_Browser_Entry_H
#define Fl_Dev_Code_Browser_Entry_H

#pragma once

#include <string>
#include <deque>

class Fl_Dev_Code_Browser_Entry {
public:
    Fl_Dev_Code_Browser_Entry(std::string name, int pos) {
        this->name = name;
        this->pos = pos;
        this->depth = 0;
    }
    Fl_Dev_Code_Browser_Entry(std::string name, int pos, int depth) {
        this->name = name;
        this->pos = pos;
        this->depth = depth;
    }
    Fl_Dev_Code_Browser_Entry(std::string name, int pos, std::string type, std::string args) {
        this->name = name;
        this->pos = pos;
        this->type = type;
        this->args = args;
        this->depth = 0;
    }

    std::string name;
    int pos;
    int depth;
    std::string type;
    std::string args;
    static std::deque <Fl_Dev_Code_Browser_Entry> children;
};

#endif // Fl_Dev_Code_Browser_Entry_H