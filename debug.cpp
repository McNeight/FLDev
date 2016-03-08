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

#include <stdlib.h>
#include <pty.h>
#include <FL/Fl.H>
#include <iostream>
#include <signal.h>
#include "extras.h"
#include "debug.h"
#include "globals.h"

int pty;
pid_t childpid = 0;
/*
void debug(string file)
{
  	
	int pid = fork();
	
	
	if(pid == 0)
	{
		return;
	}
	else
	{ 
		execlp("gdb", "gdb", file.c_str(),(char *)0);
	}
}*/

int highlight_in_editor(char *file, int line)
{
	if(strcmp(file,filename) != 0) {
		if (!check_save()) return 0;

		if(file=="") return 0;
		if (file != NULL) 
		{
			FILE *fptr = fopen(file,"r");
			if(!fptr) return 2;
			fclose(fptr);
			load_file(file, -1);
			add_recent_file_to_menu(file);
		}
	}
	
	
	int txtpos;
		int size = textbuf->length();
		while(txtpos < size) {
			if(textbuf->count_lines(0,textbuf->line_end(txtpos))>=line) break;
			txtpos = textbuf->line_end(txtpos);
			txtpos ++;
		}

	Fl::focus(te);
	te->buffer()->unselect();
	te->insert_position(te->line_start(txtpos-1));
	te->buffer()->select(te->buffer()->line_start(txtpos-1), te->buffer()->line_end(txtpos-1)+1);
	te->show_insert_position();
	return 1;			
}


int write_gdb(string str)
{	
	if (pty < 0) {
		cout << "Debugger not active" << endl;
		return 0;
	}
	write(pty, str.c_str(), str.size());
	cout << "-->write " << str << endl;
	return 1;
}


string read_gdb() {
   fd_set ifs;
   struct timeval tvzero;
   char buf[512];
   int bytesread;
   if (pty < 0) return "";  /* nothing to pump */
   string retstr;

	//fflush(pty);
   while (1) {
      /* check if pty has something to say */
      FD_ZERO(&ifs); FD_SET(pty, &ifs);
      tvzero.tv_sec = 0; tvzero.tv_usec = 0;
   
      if (select(pty + 1, &ifs, NULL, NULL, &tvzero) <= 0)
         break; /* nothing to read, or select() failed */

      /* read what we can. This is guaranteed not to block, since
       * select() told us there was something to read. */
      bytesread = read(pty, buf, 512);
      if (bytesread <= 0) break;   

      retstr += buf;
      //cout << buf << flush;
   }
   //cout << retstr << flush;
   return retstr;
}


string read_gdb_until_prompt() {
   string ret;
   
   while(1)
   {
   		ret += read_gdb();
   		int pos = ret.find("(gdb)",0);
   		if(pos != ret.npos) 
   		{   			
   			ret = ret.substr(0,pos);
   			break;
   		}
   	}

   return ret;
}



int gdb_parse_for_line(string str)
{
	char *file;
	int line;
	char *raw = strdup(str.c_str());
	char *data = strstr(raw,"file=");
	if(!data) return 0;
	
	char *result = strtok(data,"\"");
	
	if(result != NULL) 
	{
		//cout << result << endl;
		result = strtok(NULL,"\"");
	}
	if(result != NULL) 
	{
		file = result;
		result = strtok(NULL,"\"");
	}
	if(result != NULL) 
	{
		result = strtok(NULL,"\"");
	}
	if(result != NULL) 
	{
		line = atoi(result);
	}
	
	free(raw);
	cout << file << " " << line << flush << endl;
	
	return highlight_in_editor(file, line);
}



pid_t debug(string file) {
   
   struct winsize ws;
   
   //ws.ws_row = 60;
   //ws.ws_col = 20;
   //ws.ws_xpixel = ws.ws_ypixel = 0;
   
   struct termios tios;
   //raw mode
	 tios.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
	 tios.c_oflag &= ~(OPOST);
	 tios.c_cflag &= ~(CSIZE|PARENB);
	 tios.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
	 tios.c_cflag |= CS8;
	 tios.c_cc[VMIN] = 1;
	 tios.c_cc[VTIME]= 0;

   childpid = forkpty(&pty, NULL, &tios, &ws);

   if (childpid < 0) return -1;

   if (childpid == 0) {
      /* we are the child, running under the slave side of the pty. */

      /* Cajole application into using linux-console-compatible escape
       * sequences (which is what we are prepared to interpret) */
      //setenv("TERM", "linux", 1);

      /* Now we will exec /bin/sh -c command. */
      //execl("/bin/sh", "/bin/sh", "-c", command, NULL);
      execlp("gdb", "gdb", "--interpreter=mi", "-q", file.c_str(),(char *)0);

      fprintf(stderr, "\nexecl() failed.\nCommand: '%s'\n", file.c_str());
      exit(127);  /* error exec'ing */
   }
	
	read_gdb_until_prompt();
	
	
	//insert breakpoints
	gdb_break_cb(3268, "editor.cpp");
	
   /* if we got here we are the parent process */
   return childpid;
}




int gdb_run_cb() {

	if(childpid==0) return 0;
	write_gdb("run\n");
	string ret = read_gdb_until_prompt();
	
	//cout << ret;
	return gdb_parse_for_line(trim(ret));
}

int gdb_break_cb(int line, string filename) {

	if(childpid==0) return 0;	
	char buf[255];
	sprintf(buf,"-break-insert %s:%d\n",filename.c_str(),line);
	write_gdb(buf);
	
	string ret = read_gdb_until_prompt();
	return 0;//gdb_parse_for_line(trim(ret));
}


int gdb_step_cb() {

	if(childpid==0) return 0;
	write_gdb("step\n");
	string ret = read_gdb_until_prompt();
	if(gdb_parse_for_line(trim(ret)) == 2) return gdb_step_cb();
	return 0;
}

int gdb_next_cb() {
	
	if(childpid==0) return 0;
	write_gdb("next\n");
	string ret = read_gdb_until_prompt();
	return gdb_parse_for_line(trim(ret));
}

int gdb_kill_cb() {
	if(childpid==0) return 0;
	kill(childpid,9);
	childpid=0;
	return 1;
	/*
	cout << "\n-->sending kill" << endl;
	write_gdb("kill\n");
	string ret = read_gdb_until_prompt();
	
	write_gdb("quit\n");
	cout << ret;*/
}

