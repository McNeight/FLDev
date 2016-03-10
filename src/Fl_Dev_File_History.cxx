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

#include "Fl_Dev_File_History.h"

void Fl_Dev_File_History::add(std::string fname, int pos) {
    bool found = false;

    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].name == fname)
        {
            items[i].position = pos;
            found = true;
            //cout << "Found old, added new pos:" << pos << endl;
            break;
        }
    }

    if (!found)
    {
        item i;
        i.name = fname;
        i.position = pos;
        items.push_back(i);
        //cout << "Found nothing, added new item:" << pos << endl;
    }
}

int Fl_Dev_File_History::getPosition(std::string fname) {
    for (int i = 0; i < items.size(); i++)
    {
        if (strcmp(items[i].name.c_str(), fname.c_str()) == 0) return items[i].position;
    }
    return 0;
}
