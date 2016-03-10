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

#ifndef Fl_Dev_Code_Browser_H
#define Fl_Dev_Code_Browser_H

#pragma once

#include <FL/Fl_Hold_Browser.H>

class Fl_Dev_Code_Browser : public Fl_Hold_Browser {

protected:
    void item_draw(void* item, int X, int Y, int W, int H) const;

public:
    void clearsubs();
    void draw() { clearsubs(); Fl_Hold_Browser::draw(); }

    Fl_Dev_Code_Browser(int X, int Y, int W, int H, const char *L = 0) : Fl_Hold_Browser(X, Y, W, H, L) {};
};

#endif // Fl_Dev_Code_Browser_H