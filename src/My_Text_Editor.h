//
// "$Id: My_Text_Editor.H,v 1.1.2.7 2003/01/30 21:40:20 easysw Exp $"
//
// Header file for My_Text_Editor class.
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


#ifndef My_Text_Editor_H
#define My_Text_Editor_H

#include "My_Text_Display.h"
#include <FL/Fl_Text_Editor.H>

// key will match in any state
#define My_Text_Editor_ANY_STATE  (-1L)

class FL_EXPORT My_Text_Editor : public Fl_Text_Editor {
public:
      My_Text_Editor();      
};

#endif

//
// End of "$Id: My_Text_Editor.H,v 1.1.2.7 2003/01/30 21:40:20 easysw Exp $".
//

