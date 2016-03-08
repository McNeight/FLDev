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

#include "My_File_Browser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int My_File_Browser::load(const char* directory) {
	
	sprintf(current_dir,"%s",directory);

	//if(!Fl_File_Browser::load(directory)) return 0;
	Fl_File_Browser::load(directory);
	if(!showhidden)
	{
		int end = size();
		for(int i = 0; i < end; i++)
		{
			if(!text(i)) continue;
			
			if(text(i)[0] == '.' && !(strlen(text(i)) == 3 && text(i)[1] == '.')) {
				remove(i);
				i--;
				end--;
			}
		}
	}
	return 1;
	
}

void My_File_Browser::reload() {
	//clear();
	//printf("%s\n",current_dir);
	load(current_dir);
}
