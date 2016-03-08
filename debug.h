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

#ifndef DEBUG_H
#define DEBUG_H

#include <string>

using namespace std;



int write_gdb(string str);
string read_gdb();
pid_t debug(string file);

int gdb_break_cb(int line, string filename);
int gdb_run_cb();
int gdb_step_cb();
int gdb_next_cb();
int gdb_kill_cb();

#endif
