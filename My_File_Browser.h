/*
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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef MY_FILE_BROWSER_H
#define MY_FILE_BROWSER_H

#include <FL/Fl_File_Browser.H>
#include <string>


class My_File_Browser : public Fl_File_Browser 
{
  public:
  	My_File_Browser(int x, int y, int w, int h) : Fl_File_Browser(x,y,w,h) {
  		showhidden = false;
  		type(FL_HOLD_BROWSER);
  	}
  	void show_hidden(bool yn) { showhidden = yn;}
  	void toggle_show_hidden() { showhidden = !showhidden;}
  	int load(const char* directory);
  	void reload();
  	
  private:
  	bool showhidden;
  	char current_dir[1024];
	
};


#endif
