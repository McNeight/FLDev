//////////////////////////////////////////////////////////////////////////////////////////
/*
		FLDev
		
		Lightweight Integrated Development Environment

		version:	0.5.2
		author:		Philipp Pracht
		email:		pracht@informatik.uni-muenchen.de
		 
		descr.:	FLDev is a IDE designed for older systems and small C/C++ Applications
				and is based on the	Editor described in the FLTK Manual.

		 
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
/////////////////////////////////////////////////////////////////////////////////////////////


#include "extras.h" 
#include <iostream>
#include <stdio.h>

extern string get_load_err_msg();

string trim(string const& source, char const* delims) {
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(delims);
  if(index != std::string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != std::string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}




	
void build_template_main() {
		char buf[255];
		int BUFSIZE=255;
		sprintf(buf,"main.cpp");
		FILE *ptr = fopen(buf,"w");
		if(ptr != NULL) 
		{
			fprintf(ptr,"#include <iostream>\nusing namespace std;\n\n\n\nint main(int argc, char *argv[])\n{\n\tcout << \"Hello World!\\n\";\n\treturn 0;\n}\n");
			fclose(ptr);
		}
}



void File_History::add(string fname, int pos) {
	bool found = false;

	for(int i = 0; i < items.size(); i++)
	{
		if(items[i].name == fname) {
			items[i].position = pos;
			found = true;
			//cout << "Found old, added new pos:" << pos << endl;
			break;
		}
	}

	if(!found) {
		item i;
		i.name = fname;
		i.position = pos;
		items.push_back(i);	
		//cout << "Found nothing, added new item:" << pos << endl;
	}
}

int File_History::getPosition(string fname) {
	for(int i = 0; i < items.size(); i++)
	{
		if(strcmp(items[i].name.c_str(),fname.c_str())==0) return items[i].position;
	}
	return 0;
}



void Project::addToBrowser(Fl_Hold_Browser *b) {
	b->clear();

	char *tmp_src_files = strdup(src_files.c_str());
	while(src_files!="")
	{			
		char *fname = strsep(&tmp_src_files," ");

		char *first = strsep(&fname,".");

		char fn[255];
	
		sprintf(fn,"%s.%s",first,fname);
		if(fn!=NULL && fn!="") b->add(fn);
	
		if(tmp_src_files == NULL || tmp_src_files == "") break;
	}

	char *tmp_hdr_files = strdup(header_files.c_str());
	while(header_files!="")
	{			
		char *fname = strsep(&tmp_hdr_files," ");

		char *first = strsep(&fname,".");

		char fn[255];
	
		sprintf(fn,"%s.%s",first,fname);
		if(fn!=NULL && fn!="") b->add(fn);
	
		if(tmp_hdr_files == NULL || tmp_hdr_files == "") break;
	}
}


void Project::save() {
	if(trim(name) == "") return;

	char buf[255];
	int BUFSIZE=255;
	sprintf(buf,"%s.fldev",name.c_str());
	FILE *ptr = fopen(buf,"w");
	run_in_console = console_check->value();
	if(ptr != NULL) 
	{
		fprintf(ptr,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n%s\n", 
					name.c_str(),
					binfilename.c_str(),
					oDir.c_str(),
					Bin.c_str(),
					libs.c_str(),
					libdirs.c_str(),
					incdirs.c_str(),
					ldflags.c_str(),
					cflags.c_str(),
					run_in_console,
					src_files.c_str(),
					header_files.c_str()
				);
		fclose(ptr);
		modified=false;
	}
}

int Project::load() {
	char buf[255];
	int BUFSIZE=255;
	sprintf(buf,"%s",pr_filename.c_str());
	FILE *ptr = fopen(buf,"r");

	char aname[255], abinf[255], aodir[255], abin[255], alibs[255], alibd[255], ainc[255], ald[255], ac[255], asrc[255], ahdr[255], acons[255];
	if(ptr != NULL) 
	{
		fgets(aname,255,ptr);
		fgets(abinf,255,ptr);
		fgets(aodir,255,ptr);
		fgets(abin,255,ptr);
		fgets(alibs,255,ptr);
		fgets(alibd,255,ptr);
		fgets(ainc,255,ptr);
		fgets(ald,255,ptr);
		fgets(ac,255,ptr);
		fgets(acons,255,ptr);
		fgets(asrc,255,ptr);
		fgets(ahdr,255,ptr);

					
		name = trim(aname);
		binfilename = trim(abinf);
		oDir = trim(aodir);
		Bin = trim(abin);
		libs = trim(alibs);
		libdirs = trim(alibd);
		incdirs = trim(ainc);
		ldflags = trim(ald);
		cflags = trim(ac);
		src_files = trim(asrc);
		header_files = trim(ahdr);
		sscanf(acons,"%d",&run_in_console);
					
		pname->value(name.c_str());
		ptarg->value(binfilename.c_str());
		psrc->value(src_files.c_str());
		phdr->value(header_files.c_str());
		pobdir->value(oDir.c_str());
		pbin->value(Bin.c_str());
		plib->value(libs.c_str());
		plibdir->value(libdirs.c_str());
		pincl->value(incdirs.c_str());
		pldflags->value(ldflags.c_str());
		pcflags->value(cflags.c_str());
		console_check->value(run_in_console);

		fclose(ptr);
		assigned = true;
		modified=false;
		return 1;
	}
	else {
		fl_alert(get_load_err_msg().c_str());
		return 0;
	}
}


static void kill_selection(My_Text_Editor* e) {
  if (e->buffer()->selected()) {
    e->insert_position(e->buffer()->primary_selection()->start());
    e->buffer()->remove_selection();
  }
}



int Fl_Text_Editor_ext::handle(int event) {
  if (!buffer()) return 0;


  if(event==FL_KEYBOARD) {
      return handle_key_ext();
  }
  

  return My_Text_Editor::handle(event);
}




int Fl_Text_Editor_ext::handle_key_ext() {
  
  
  int del;
  if (Fl::compose(del)) {
    if (del) buffer()->select(insert_position()-del, insert_position());
    kill_selection(this);
    if (Fl::event_length()) {
      if (insert_mode()) {
			insert(Fl::event_text());
		}
      else overstrike(Fl::event_text());
    }
    show_insert_position();
    if (when()&FL_WHEN_CHANGED) do_callback();
    else set_changed();
    return 1;
  }
	//cout << line << "end" << endl;


  int key = Fl::event_key(), state = Fl::event_state(), c = Fl::event_text()[0];
  state &= FL_SHIFT|FL_CTRL|FL_ALT|FL_META; // only care about these states

  if(Fl::event_key()==FL_Enter && smart_indent) {
	char *line = "";
	line = strdup(buffer()->line_text(insert_position()));
	for(int i = 0; i < strlen(line); i++) {
		if(line[i]!=' ' && line[i]!='\t') line[i]='\0';
	}
	kf_enter(c,this);
  	insert(line); 
	return 1;
  }

  else if(Fl::event_key()==FL_Home) {
	int pos;
	char *line = "";
	
	line = strdup(buffer()->line_text(insert_position()));
	for(int i = 0; i < strlen(line); i++) {
		if(line[i]!=' ' && line[i]!='\t') { pos = i; break; }
	}
	int old = insert_position();
	insert_position(buffer()->line_start(insert_position()) + pos);
	if(state == FL_SHIFT) buffer()->select(insert_position(),old);
	if(insert_position() != old) return 1;
  }
  else 0;
 
  Key_Func f;
  f = bound_key_function(key, state, global_key_bindings);
  if (!f) f = bound_key_function(key, state, key_bindings);
  if (f) return f(key, this);
  if (default_key_function_ && !state) return default_key_function_(c, this);
  return 0;

}


