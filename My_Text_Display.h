//
// "$Id: My_Text_Display.H,v 1.4.2.13 2003/01/30 21:40:18 easysw Exp $"
//
// Header file for My_Text_Display class.
//
// Copyright 2001-2003 by Bill Spitzak and others.
// Original code Copyright Mark Edel.  Permission to distribute under
// the LGPL for the FLTK library granted by Mark Edel.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#ifndef MY_TEXT_DISPLAY_H
#define MY_TEXT_DISPLAY_H

#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>

class FL_EXPORT My_Text_Display: public Fl_Text_Display {
public:
      My_Text_Display();
      friend class My_Text_Editor;
};

#endif

//
// End of "$Id: My_Text_Display.H,v 1.4.2.13 2003/01/30 21:40:18 easysw Exp $".
//
