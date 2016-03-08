//
// "$Id: My_Text_Display.cxx,v 1.12.2.46 2003/08/24 13:09:06 easysw Exp $"
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

#include <stdio.h>
#include <stdlib.h>
//#include <FL/flstring.H>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include "My_Text_Display.h"
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>

#undef min
#undef max

// Text area margins.  Left & right margins should be at least 3 so that
// there is some room for the overhanging parts of the cursor!
#define TOP_MARGIN 1
#define BOTTOM_MARGIN 1
#define LEFT_MARGIN 3
#define RIGHT_MARGIN 3

#define NO_HINT -1

/* Masks for text drawing methods.  These are or'd together to form an
   integer which describes what drawing calls to use to draw a string */
#define FILL_MASK 0x100
#define SECONDARY_MASK 0x200
#define PRIMARY_MASK 0x400
#define HIGHLIGHT_MASK 0x800
#define STYLE_LOOKUP_MASK 0xff

/* Maximum displayable line length (how many characters will fit across the
   widest window).  This amount of memory is temporarily allocated from the
   stack in the draw_vline() method for drawing strings */
#define MAX_DISP_LINE_LEN 1000

static int max( int i1, int i2 );
static int min( int i1, int i2 );
static int countlines( const char *string );

// CET - FIXME
#define TMPFONTWIDTH 6

//
// End of "$Id: My_Text_Display.cxx,v 1.12.2.46 2003/08/24 13:09:06 easysw Exp $".
//
