//////////////////////////////////////////////////////////////////////////////////////////
/*
		FLDev
		
		Lightweight Integrated Development Environment

		version:	0.5.5
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




// TODO:
/*
		make better comments
		debugging function
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <deque>
#include <X11/xpm.h>
#include "fldevicon.xpm"
#include "icon_pixmaps.h"

#ifdef __MWERKS__ 
#define FL_DLL
#endif


#include <FL/Fl.H>
#include <FL/x.H>

#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H> 
#include <FL/Fl_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Help_Dialog.H>


#include "pref_form.h"
#include "proj_form.h"
#include "proj_wiz_form.h"
#include "extras.h" 


  
using namespace std;

class EditorWindow;

int				rec_pr_menu_index = 45;
int                bufchanged = 0;
char               filename[256] = "";
char               filename_wo_path[256] = "";
char               title[256];  
bool hidden = true, cppfile, filelistopen=false, make_error = false, exec_running = false;
int linecount;
int num_windows = 0;
int loading = 0;


Fl_Color hl_plain = FL_BLACK;
Fl_Color hl_linecomment = FL_DARK_GREEN;
Fl_Color hl_blockcomment = FL_DARK_GREEN;
Fl_Color hl_string = FL_BLUE;
Fl_Color hl_directive = FL_DARK_MAGENTA;
Fl_Color hl_type = FL_DARK_RED;
Fl_Color hl_keyword = FL_BLUE;
Fl_Color hl_character = FL_DARK_RED;
Fl_Color background_color = FL_WHITE;

Fl_Text_Editor_ext 		*te;
Fl_Text_Buffer     *textbuf = 0;
Fl_Text_Buffer     *outputtb = 0;
Project project;
File_History *file_hist;
Fl_Menu_Bar* menubar;
Fl_Help_Dialog *help_dialog;
EditorWindow* window;

void set_title(Fl_Window* w);
void add_recent_file_to_menu(char *filename);
void add_recent_project_to_menu(char *filename);
void save_config_file();
void set_text_size(int t);
int check_save();
int load_file(char *,int);

Fl_Window* make_form();


//callback functions
void owbt_callback(Fl_Widget*, void*);
void pr_opt_cb(Fl_Widget*, void*);
void open_pr_cb();
void save_pr_cb();
void new_pr_cb();
void pref_cb(Fl_Widget*, void*);
void show_browser_cb();
void generate_makefile_cb();
void b_cb(Fl_Widget* o, void*);
void manual_cb(Fl_Widget *, void *);
void ref_cb(Fl_Widget *, void *);
void recent_file_cb(Fl_Widget* w, void*);
void save_cb();
void saveas_cb();
void find2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);
void generate_makefile_cb();



// Syntax highlighting stuff...
Fl_Text_Buffer     *stylebuf = 0;
Fl_Text_Buffer		*op_stylebuf = 0;


#define TEXTSIZE 10

Fl_Text_Display::Style_Table_Entry
           op_styletable[] = {	// Style table
		     { FL_BLACK,      FL_HELVETICA, TEXTSIZE }, 	// A - Plain
		     { FL_RED, 		  FL_HELVETICA, TEXTSIZE } 	// B - Error
		   };

My_Text_Display::Style_Table_Entry
                   styletable[] = {	// Style table
		     { hl_plain,      FL_COURIER,        TEXTSIZE }, // A - Plain
		     { hl_linecomment, FL_COURIER_ITALIC, TEXTSIZE }, // B - Line comments
		     { hl_blockcomment, FL_COURIER_ITALIC, TEXTSIZE }, // C - Block comments
		     { hl_string,       FL_COURIER,        TEXTSIZE }, // D - Strings
		     { hl_directive,   FL_COURIER,        TEXTSIZE }, // E - Directives
		     { hl_type,   FL_COURIER_BOLD,   TEXTSIZE }, // F - Types
		     { hl_keyword,       FL_COURIER_BOLD,   TEXTSIZE }, // G - Keywords
		     { hl_character,    FL_COURIER,        TEXTSIZE }  // H - Character
		   };





const char         *code_keywords[] = {	// List of known C/C++ keywords...
		     "and",
		     "and_eq",
		     "asm",
		     "bitand",
		     "bitor",
		     "break",
		     "case",
		     "catch",
		     "compl",
		     "continue",
		     "default",
		     "delete",
		     "do",
		     "else",
		     "false",
		     "for",
		     "goto",
		     "if",
		     "new",
		     "not",
		     "not_eq",
		     "operator",
		     "or",
		     "or_eq",
		     "return",
		     "switch",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "using",
		     "while",
		     "xor",
		     "xor_eq"
		   };
const char         *code_types[] = {	// List of known C/C++ types...
		     "auto",
		     "bool",
		     "char",
		     "class",
		     "const",
		     "const_cast",
		     "double",
		     "dynamic_cast",
		     "enum",
		     "explicit",
		     "extern",
		     "float",
		     "friend",
		     "inline",
		     "int",
		     "long",
		     "mutable",
		     "namespace",
		     "private",
		     "protected",
		     "public",
		     "register",
		     "short",
		     "signed",
		     "sizeof",
		     "static",
		     "static_cast",
		     "struct",
		     "template",
		     "typedef",
		     "typename",
		     "union",
		     "unsigned",
		     "virtual",
		     "void",
		     "volatile"
		   };

//
// 'compare_keywords()' - Compare two keywords...
//
int
compare_keywords(const void *a,
                 const void *b) {
  return (strcmp(*((const char **)a), *((const char **)b)));
}


//
// 'style_parse()' - Parse text and produce style data.
//
//
void
style_parse(const char *text,
            char       *style,
	    int        length) {
  char	     current; 
  int	     last;
  char	     buf[255],
             *bufptr;
  const char *temp; 

  if(!cppfile) return;

  for (current = *style, last = 0; length > 0; length --, text ++) {
    
    //if ((current != 'C') && (current != 'D') && (current != 'E')) current = 'A';
    //if ((current == 'B')) current = 'A';
  
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      	if (*text == '#') {
        	// Set style to directive
	        current = 'E';
	} 

	else if (strncmp(text, "//", 2) == 0) {
	        current = 'B';
		for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

	        if (length == 0) break;
      	} 

	else if (strncmp(text, "/*", 2) == 0) {
        	current = 'C';
		*style++ = current;
		*style--;
	} 

	else if (strncmp(text, "\\\"", 2) == 0) {
	        // Quoted quote...
		*style++ = current;
		*style++ = current;
		text ++;
		length --;
		continue;
      	} 

	else if (*text == '\"' /*| *text == '\''*/) {
        	current = 'D';
      	} 

	else if (*text == '\'') {
        	current = 'H';
      	} 

	else if (!last && islower(*text)) {
        	// Might be a keyword...
		for (temp = text, bufptr = buf; (isalnum(*temp) || *temp=='_') && bufptr < (buf + sizeof(buf) - 1); *bufptr++ = *temp++);
		        if (!islower(*temp)) {
				*bufptr = '\0';
				bufptr = buf;

				if (bsearch(&bufptr, code_types, sizeof(code_types) / sizeof(code_types[0]), sizeof(code_types[0]), compare_keywords)) {
					while (text < temp) {
				  		*style++ = 'F';
						text ++;
		      				length --;
					}

	    				text --;
	    				length ++;
	    				last = 1;
	    				continue;
	  			} else if (bsearch(&bufptr, code_keywords, sizeof(code_keywords) / sizeof(code_keywords[0]), sizeof(code_keywords[0]), compare_keywords)) {
	    				while (text < temp) {
	      					*style++ = 'G';
	      					text ++;
	      					length --;
	    				}

	    				text --;
	    				length ++;
	    				last = 1;
	    				continue;
	  			}
			}
      		}
    	} else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      	// Close a C comment... 

	      	*style++ = current;
      		*style++ = current;
	      	text ++;
      		length --;
	      	current = 'A';
      		continue;
    	} else if (current == 'D') {
      	// Continuing in string...
      		if (strncmp(text, "\\\"", 2) == 0 || strncmp(text, "\\\'", 2) == 0) {
        	// Quoted end quote...
			*style++ = current; 
			*style++ = current;
			text ++;
			length --;
			continue;
      		} else if (*text == '\"'/* || *text == '\''*/) {
        	// End quote...
			*style++ = current;
			current = 'A';
			continue;
      		}
    	} else if (current == 'H') {
      	// Continuing in char...
      		if (*text == '\'') {
        	// End quote...
			*style++ = current;
			current = 'A';
			continue;
      		}
    	}


    	// Copy style info...
    	if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
    	else *style++ = current;

    	last = isalnum(*text) || *text == '.';

    	if (*text == '\n') {
      	// Reset column and possibly reset the style
      		if (current != 'D' && current != 'C') current = 'A';
    	}

  } //for
}


//
// 'style_init()' - Initialize the style buffer...
//

void
style_init(void) {
  char *style = new char[textbuf->length() + 1];
  char *text = textbuf->text();
  

  memset(style, 'A', textbuf->length());
  style[textbuf->length()] = '\0';

  if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

  if(cppfile) style_parse(text, style, textbuf->length());

  stylebuf->text(style);
  delete[] style;
  free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
style_unfinished_cb(int, void*) {
}


//
// 'style_update()' - Update the style buffer...
//

void
style_update(	int        pos,		// I - Position of update
             	int        nInserted,	// I - Number of inserted chars
	     		int        nDeleted,	// I - Number of deleted chars
             	int        /*nRestyled*/,	// I - Number of restyled chars
	     		const char * /*deletedText*/,// I - Text that was deleted
             	void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last,				// Last style on line
	stringdeleted=0;
  char *style,				// Style data
	*text;				// Text data


  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = (char*)malloc(nInserted + 1);
    memset(style, 'A', nInserted);
    style[nInserted] = '\0'; 

    stylebuf->replace(pos, pos + nDeleted, style);
    free(style);
  } else {
    // Just delete characters in the style buffer...
    if((stylebuf->character(pos) == 'D') || (stylebuf->character(pos) == 'C')) stringdeleted = 1;
    stylebuf->remove(pos, pos + nDeleted);
  }

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
  end   = textbuf->line_end(pos + nInserted);
  text  = textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  last  = style[end - start - 1];


  style_parse(text, style, end - start);


  stylebuf->replace(start, end, style);
  ((Fl_Text_Editor_ext *)cbArg)->redisplay_range(start, end);

  //if ((last != style[end - start - 1]) || stringdeleted || style[end - start - 1] == 'D') 
  {
    // The last character on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    start = 0;
    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);

    stylebuf->replace(start, end, style);
    ((Fl_Text_Editor_ext *)cbArg)->redisplay_range(start, end);
  }

  free(text);
  free(style);
}






/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Editor window functions and class...



class My_Text_Editor2 : public Fl_Text_Editor {
  public:
	My_Text_Editor2(int x, int y, int w, int h) : Fl_Text_Editor(x,y,w,h) {};
    int handle(int event) {
		if (!buffer()) return 0;
  	
  		if (!Fl::event_inside(text_area.x, text_area.y, text_area.w, text_area.h))
		{
    		return Fl_Group::handle(event);
  		}

		if(event==FL_PUSH) {
			if (Fl::focus() != this) {
	  		Fl::focus(this);
			handle(FL_FOCUS);
			}

			int pos = xy_to_position(Fl::event_x(), Fl::event_y(), CURSOR_POS);
			buffer()->select(buffer()->line_start(pos), buffer()->line_end(pos)+1);
			if(Fl::event_clicks()>0) {		
				Fl::event_clicks(-1);
				char *buf = outputtb->line_text(pos);
				if(buf == NULL) return 0;
				char *t_filename = strsep(&buf,":");
				if(buf == NULL) return 0;
				char *linenr = strsep(&buf,":");
				if(buf == NULL) return 0;
				int line;	

				if(strcmp(t_filename,filename_wo_path) != 0) {
					if (!check_save()) return 0;

					if(t_filename=="") return 0;
					if (t_filename != NULL) 
					{
						load_file(t_filename, -1);
						add_recent_file_to_menu(filename);
					}
				}

				if(sscanf(linenr,"%d",&line))
				{
					Fl::focus(te);
					te->buffer()->unselect();
					int txtpos = linepos(line);
					te->insert_position(te->line_start(txtpos-1));
					te->buffer()->select(te->buffer()->line_start(txtpos-1), te->buffer()->line_end(txtpos-1)+1);
					te->show_insert_position();
				}
			}
		}
		else if(event==FL_MOUSEWHEEL) return mVScrollBar->handle(event);
		else return 0;
	};

	int linepos(int line) {
		int ret=0;
		int size = textbuf->length();
		while(ret < size) {
			if(textbuf->count_lines(0,textbuf->line_end(ret))>=line) break;
			ret = textbuf->line_end(ret);
			ret ++;
		}
		return ret;
	}
};




class EditorWindow : public Fl_Double_Window {
  public:
    EditorWindow(int w, int h, const char* t);
    ~EditorWindow();
    void draw();
    void EditorWindow::hide_output();	
    void EditorWindow::show_output();
    void EditorWindow::hide_browser();	
    void EditorWindow::show_browser();
	
	int handle(int event) {
		if(event == FL_KEYBOARD) return 0;
		else return	Fl_Double_Window::handle(event);
	}
    

    Fl_Window			*pr_options_dlg;
    Fl_Input           	*commandline;
    Fl_Check_Button		*console_check;
    Fl_Button          	*pr_ok_bt;

    Fl_Window          	*replace_dlg;
    Fl_Input           	*replace_find;
    Fl_Input           	*replace_with;
    Fl_Button          	*replace_all;
    Fl_Return_Button   	*replace_next;
    Fl_Button          	*replace_cancel;

    Fl_Button          	*outputwindowbutton;
	Fl_Tabs				*tabs;
    Fl_Box	       		*statusbar; 
    Fl_Output			*fb_label;	
	Fl_Button			*fb_home_btn;
    Fl_Text_Editor_ext 	*editor;
    My_Text_Editor2		*output;

	My_File_Browser	 	*file_browser;
	Fl_Hold_Browser 	*pr_browser;
    char               	search[256];

};



EditorWindow::EditorWindow(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
  replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(80, 10, 210, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 210, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback((Fl_Callback *)replcan_cb, this);
  replace_dlg->end();
  replace_dlg->set_non_modal();
  editor = 0;
  *search = (char)0;
}



EditorWindow::~EditorWindow() {
  delete replace_dlg;
}



void EditorWindow::draw() {
  Fl_Double_Window::draw();
}

 
void EditorWindow::show_output()
{
	Fl_Text_Editor *o = output;

	editor->size(editor->w(),h()-167);	//edited
	tabs->size(tabs->w(),h()-167); 	//edited
	o->resize(0,editor->y() + editor->h()+1 ,w(),100);
	o->show();
	Fl::check();
}

void EditorWindow::hide_output()
{
	Fl_Text_Editor *o = output;

	editor->size(editor->w(),h()-66);	//edited
	tabs->size(tabs->w(),h()-66);	//edited
	o->hide();
}
 

 
void EditorWindow::show_browser()
{
	tabs->size(150,editor->h());
	editor->resize(tabs->w(),editor->y(),editor->w()-tabs->w(),editor->h());
	tabs->show();
	editor->redraw();
	Fl::check();
}

void EditorWindow::hide_browser()
{
	editor->resize(0,editor->y(),w(),editor->h());
	tabs->hide();
}


int check_save(void) {
  if (!bufchanged) return 1;

  int r = fl_choice("The current file has not been saved.\n"
                    "Would you like to save it now?",
                    "Cancel", "Save", "Discard");

  if (r == 1) {
    save_cb(); // Save the file...
    return !bufchanged;
  }

  return (r == 2) ? 1 : 0;
}



int check_project_save(void) {
  if (!project.modified) return 1;

  int r = fl_choice("The current Project has not been saved.\n"
                    "Would you like to save it now?",
                    "Cancel", "Save", "Discard");

  if (r == 1) {
    project.save(); // Save the file...
    return !project.modified;
  }

  return (r == 2) ? 1 : 0;
}





int  load_file(char *newfile, int ipos) {
  loading = 1;
  cppfile = false;

  if(strncmp(newfile + strlen(newfile) - 4,".cpp",4) == 0) cppfile = true;
  if(strncmp(newfile + strlen(newfile) - 2,".c",2) == 0) cppfile = true;
  if(strncmp(newfile + strlen(newfile) - 2,".h",2) == 0) cppfile = true;
  if(strncmp(newfile + strlen(newfile) - 4,".hpp",4) == 0) cppfile = true;
  if(strncmp(newfile + strlen(newfile) - 4,".cxx",4) == 0) cppfile = true;
  if(strncmp(newfile + strlen(newfile) - 4,".hxx",4) == 0) cppfile = true;

  if(strncmp(newfile + strlen(newfile) - 6,".fldev",4) == 0) {
		project.pr_filename = newfile;
		if(!project.load()) return 0;
		project.addToBrowser(window->pr_browser);
		add_recent_project_to_menu(newfile);
		return 2;
	}

  int insert = (ipos != -1);
  bufchanged = insert;
  char *tmp = strdup(filename);
  if (!insert) strcpy(filename, "");
  int r;

  if (!insert) { 
	if (tmp[0] != '\0') file_hist->add(tmp,te->insert_position());
	r = textbuf->loadfile(newfile);
  }
  else r = textbuf->insertfile(newfile, ipos);

  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
  else
    if (!insert) strcpy(filename, newfile);
  loading = 0;
  textbuf->call_modify_callbacks();
  set_title(window);
  te->insert_position(file_hist->getPosition(filename));
  te->show_insert_position();
  Fl::focus(te);
  return 1;
}





void save_file(char *newfile) {
  if (textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    strcpy(filename, newfile);
  bufchanged = 0;
  textbuf->call_modify_callbacks();
}




void copy_cb(Fl_Widget*, void* v) {
  	EditorWindow* e = window;
  	Fl_Text_Editor_ext::kf_copy(0, e->editor);
	Fl::focus(e->editor);
}



void cut_cb(Fl_Widget*, void* v) {
  	EditorWindow* e = window;
  	Fl_Text_Editor_ext::kf_cut(0, e->editor);
	Fl::focus(e->editor);
}



void paste_cb(Fl_Widget*, void* v) {
	EditorWindow* e = window;
	Fl_Text_Editor_ext::kf_paste(0, e->editor);
	Fl::focus(e->editor);
}

void undo_cb(Fl_Widget*, void* v) {
	EditorWindow* e = window;
	Fl_Text_Editor_ext::kf_undo(0, e->editor);
	Fl::focus(e->editor);
}


void delete_cb(Fl_Widget*, void*) {
  textbuf->remove_selection();
}




void find_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = window;
  const char *val;
  val = fl_input("Search String:", e->search);
  if (val != NULL) {
    // User entered a string - go find it!
    strcpy(e->search, val);
    find2_cb(w, v);
  }
}









void run_cb(Fl_Widget* w, void* v) {
  FILE *ptr;
  string buf;
  string command;
  if(exec_running) {
		fl_alert("Program already running!");
		return;
  }
  exec_running = true;
  command = project.oDir + "/" + project.binfilename;
  if(command == "") fl_alert("Name of executable file not defined.\nCheck project options!");

  if(console_check->value()) {	
	ptr = fopen("fldevrun.sh","w");
	fprintf(ptr,"#!/bin/sh\n%s\necho 'Press Enter to continue...'\nread",command.c_str());
	fclose(ptr);
	buf = "xterm -e sh fldevrun.sh &";
	ptr = popen(buf.c_str(),"r");
	pclose(ptr);
  }
  else {
  	buf = command+" &";
	ptr = popen((buf + " >/dev/null 2>&1").c_str(),"r");
	 
	pclose(ptr);
  }
  exec_running = false;
}



void xterm_cb(Fl_Widget* w, void* v) {
  FILE *ptr;
  ptr = popen("xterm &","r");
  pclose(ptr);
}


void fluid_cb(Fl_Widget* w, void* v) {
  FILE *ptr;
  ptr = popen("fluid &","r");
  pclose(ptr);
}




void find2_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  if (e->search[0] == '\0') {
    // Search string is blank; get a new one...
    find_cb(w, v);
    return;
  }

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, e->search, &pos);
  if (found) {
    // Found a match; select and update the position...
    textbuf->select(pos, pos+strlen(e->search));
    e->editor->insert_position(pos+strlen(e->search));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", e->search);
}




void set_title(Fl_Window* w) {
  char temp_title[511];
  if (filename[0] == '\0') strcpy(temp_title, "Untitled");
  else {
    char *slash;
    slash = strrchr(filename, '/');
#ifdef WIN32
    if (slash == NULL) slash = strrchr(filename, '\\');
#endif
    if (slash != NULL) strcpy(temp_title, slash + 1);
    else strcpy(temp_title, filename);
  }

  if (bufchanged) strcat(temp_title, " (modified)");
  if(!project.assigned) sprintf(title,"FLDev     %s",temp_title);
  else sprintf(title,"FLDev  Project: %s  File: %s",project.name.c_str(),temp_title);
  sprintf(filename_wo_path,"%s",temp_title);
  ((EditorWindow *)w)->label(title);
}




void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  EditorWindow *w = (EditorWindow *)v;
  if ((nInserted || nDeleted) && !loading) {
		bufchanged = 1;
  		if(auto_hide) w->hide_output();
		set_title(w);
  }
  if (loading) {
		w->editor->show_insert_position();
		set_title(w);
  }
}

void new_cb(Fl_Widget*, void*) {
  if (!check_save()) return;

  filename[0] = '\0';
  textbuf->select(0, textbuf->length());
  textbuf->remove_selection();
  bufchanged = 0;
  textbuf->call_modify_callbacks();
}





void open_cb(Fl_Widget*, void*) {
  if (!check_save()) return;

  char *newfile = fl_file_chooser("Open File?", "[!.]*", filename);
  char path[511];

  if(newfile=="") return;
  if (newfile != NULL) 
	{
  		strcpy(path,newfile);
		char *slash;
    	slash = strrchr(path, '/');
		*slash = '\0';
		load_file(newfile, -1);
		add_recent_file_to_menu(filename);
	}
}





void insert_cb(Fl_Widget*, void *v) {
  char *newfile = fl_file_chooser("Insert File?", "*", filename);
  EditorWindow *w = (EditorWindow *)v;
  if (newfile != NULL) load_file(newfile, w->editor->insert_position());
}


void close_cb(Fl_Widget*, void* v) {
  if(Fl::event_key() == FL_Escape) return;
  Fl_Window* w = (Fl_Window*)v;
  if (num_windows == 1 && !check_save()) {
    return;
  }
  if(!check_project_save()) return;

  w->hide();
  textbuf->remove_modify_callback(changed_cb, w);
  delete w;
  num_windows--;
  save_config_file();
  FILE *ptr = popen("rm -f fldevrun.sh","r");
  pclose(ptr);

  exit(0);
}

void quit_cb(Fl_Widget*, void*) {
  if(Fl::event_key() == FL_Escape) return;
  if (bufchanged && !check_save())
    return;
  if(!check_project_save()) return;
  save_config_file();
  FILE *ptr = popen("rm -f fldevrun.sh","r");
  pclose(ptr);
  exit(0);
}

void replace_cb(Fl_Widget*, void* v) {
  EditorWindow* e = window;
  e->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v) {
  EditorWindow* e = window;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, find, &pos);

  if (found) {
    // Found a match; update the position and replace text...
    textbuf->select(pos, pos+strlen(find));
    textbuf->remove_selection();
    textbuf->insert(pos, replace);
    textbuf->select(pos, pos+strlen(replace));
    e->editor->insert_position(pos+strlen(replace));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replall_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  find = e->replace_find->value();
  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  e->editor->insert_position(0);
  int times = 0;

  // Loop through the whole string
  for (int found = 1; found;) {
    int pos = e->editor->insert_position();
    found = textbuf->search_forward(pos, find, &pos);

    if (found) {
      // Found a match; update the position and replace text...
      textbuf->select(pos, pos+strlen(find));
      textbuf->remove_selection();
      textbuf->insert(pos, replace);
      e->editor->insert_position(pos+strlen(replace));
      e->editor->show_insert_position();
      times++;
    }
  }

  if (times) fl_message("Replaced %d occurrences.", times);
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replcan_cb(Fl_Widget*, void* v) {
  EditorWindow* e = window;
  e->replace_dlg->hide();
}

void save_cb() {
  if (filename[0] == '\0') {
    // No filename - get one!
    saveas_cb();
    return;
  }
  else save_file(filename);
  set_title(window);
}

void saveas_cb() {
  char *newfile;
  newfile = fl_file_chooser("Save File As?", "*", filename);
  if (newfile != NULL) {
	save_file(newfile);
	set_title(window);
  }
}



void make_(Fl_Widget* w, void* v, int mode=0) {
  EditorWindow* e = window;
  FILE *ptr;
  Fl_Text_Editor *o = e->output;
  char buf[255];
  int BUFSIZE=255;
  make_error=false;
  if(!stylebuf) stylebuf = new Fl_Text_Buffer();	


  string outp, erroutp;
  if(bufchanged) save_cb();
  outputtb->remove(0,outputtb->length());
  e->show_output();
  e->redraw();
  ptr = popen("rm errorlog.make 2> /dev/null","r"); 
  pclose(ptr);
  char *fname = strdup(filename_wo_path);
  if(mode==2) sprintf(buf,"make %s.o 2> errorlog.make",strsep(&fname,"."));
  else if(mode==1) sprintf(buf,"make clean 2> errorlog.make &");
  else if(mode==0) sprintf(buf,"make 2> errorlog.make &");
  else return;
  ptr = popen(buf,"r");
  op_stylebuf->remove(0,op_stylebuf->length());
  
  op_stylebuf->append("AAAAA");
  outputtb->append("make:\n");



  int count=0; 
  while(fgets(buf,BUFSIZE,ptr) != NULL)
  {
	for(int i = 0; i < strlen(buf); i++)
		op_stylebuf->append("A");
	outputtb->append(buf);
	Fl::check();
	count++;
  } 
  pclose(ptr);
  outp="";
  ptr = fopen("errorlog.make","r");
  erroutp = "";
  while(fgets(buf,BUFSIZE,ptr) != NULL)
  {
	erroutp += buf;
	for(int i = 0; i < strlen(buf); i++)
		op_stylebuf->append("B");
	Fl::check();
	count++;
  } 

  if(erroutp == "") {
  	erroutp += "make finished.\n";
	op_stylebuf->append("AAAAAAAAAAAAAAA");
  }						 
  else { 
	erroutp += "make finished with errors.\n";
	op_stylebuf->append("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	make_error=true;
  }						 
  outputtb->append((outp + erroutp).c_str());
  o->insert_position(outputtb->length());
  o->show_insert_position();
  fclose(ptr);
  linecount = count+2;
    
}

void make_cb(Fl_Widget* w, void* v) {
	make_(w,v,0);
}

void compile_cb(Fl_Widget* w, void* v) {
	make_(w,v,2);
}

void make_clean_cb(Fl_Widget* w, void* v) {
	make_(w,v,1);
}

void make_run_cb(Fl_Widget* w, void* v) {
  	make_(w,v,0);
 	if(!make_error) run_cb(w,v);
}



void theme_plastic_cb()
{
	Fl::scheme("plastic");
}


void theme_none_cb()
{
	Fl::scheme("none");
}


void mindent_cb() {
	int firstlinepos,actuallinepos;
	te->buffer()->selection_position(&firstlinepos,&actuallinepos);
	actuallinepos = te->buffer()->line_start(actuallinepos);
	firstlinepos = te->buffer()->line_start(firstlinepos);
	while(actuallinepos >= firstlinepos) {
		te->buffer()->insert(actuallinepos,"\t");
		actuallinepos = te->buffer()->line_start(actuallinepos-1);
	}
}

void munindent_cb() {
	int firstlinepos,actuallinepos;
	te->buffer()->selection_position(&firstlinepos,&actuallinepos);
	actuallinepos = te->buffer()->line_start(actuallinepos);
	firstlinepos = te->buffer()->line_start(firstlinepos);
	while(actuallinepos >= firstlinepos) {
		if(strcmp(te->buffer()->text_range(actuallinepos,actuallinepos+1)," ")==0 ||
			strcmp(te->buffer()->text_range(actuallinepos,actuallinepos+1),"\t")==0) 
			te->buffer()->remove(actuallinepos,actuallinepos+1);
		actuallinepos = te->buffer()->line_start(actuallinepos-1);
	}
}


void about_cb() {
	fl_message("FLDev IDE\nVersion 0.5.5\n\nCopyright (C) 2005 by Philipp Pracht\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n");
}


Fl_Menu_Item menuitems[] = {
  { "&File",              0, 0, 0, FL_SUBMENU },	//0
    { "&New File",        0, (Fl_Callback *)new_cb },
    { "&Open File...",    FL_CTRL + 'o', (Fl_Callback *)open_cb },
    { "Open &Recent File",    0, 0, 0, FL_SUBMENU },
		{ "" },
		{ "" },
		{ "" },
		{ "" },
	{ 0 },
    { "&Insert File...",  FL_CTRL + 'e', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
    { "&Save File",       FL_CTRL + 's', (Fl_Callback *)save_cb }, 	//10
    { "Save File &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
    { "E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)quit_cb, 0 },
    { 0 },

  { "&Edit", 0, 0, 0, FL_SUBMENU },
    { "&Undo",     FL_CTRL + 'z', (Fl_Callback *)undo_cb, 0, FL_MENU_DIVIDER },
    { "Cu&t",        FL_CTRL + 'x', (Fl_Callback *)cut_cb },
    { "&Copy",       FL_CTRL + 'c', (Fl_Callback *)copy_cb },
    { "&Paste",      FL_CTRL + 'v', (Fl_Callback *)paste_cb },
    { "&Delete",     0, (Fl_Callback *)delete_cb, 0, FL_MENU_DIVIDER },
    { "&Indent Selection", FL_CTRL + 'i', (Fl_Callback *)mindent_cb },
    { "U&nindent Selection", FL_CTRL + FL_SHIFT + 'i', (Fl_Callback *)munindent_cb , 0, FL_MENU_DIVIDER},	//20
    { "Pre&ferences...",        0, (Fl_Callback *)pref_cb },
    { 0 },	

  { "&View", 0, 0, 0, FL_SUBMENU },
    { "&Show Filebrowser",        0, (Fl_Callback *)show_browser_cb, 0, FL_MENU_TOGGLE },
    { "&Theme",        0,  0, 0, FL_SUBMENU },
      { "&None",        0, (Fl_Callback *)theme_none_cb },
      { "&Plastic",        0, (Fl_Callback *)theme_plastic_cb },
      { 0 },
    { 0 }, 

  { "&Search", 0, 0, 0, FL_SUBMENU },	//30
    { "&Find...",       FL_CTRL + 'f', find_cb },
    { "F&ind Again",    FL_F + 3, find2_cb },
    { "&Replace...",    FL_CTRL + 'r', replace_cb },
    { "Re&place Again", 0, replace2_cb },
    { 0 },

 { "&Project", 0, 0, 0, FL_SUBMENU },
    { "&New Project", FL_CTRL + FL_SHIFT + 'n', (Fl_Callback *)new_pr_cb},
    { "Open &Project", FL_CTRL + FL_SHIFT + 'p', (Fl_Callback *)open_pr_cb },
    { "Previous Pro&ject", 0, 0, 0, FL_SUBMENU },	//39
		{ "" },
		{ "" },
		{ "" },
		{ "" },
	{ 0 },
    { "&Save Project", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)save_pr_cb, 0, FL_MENU_DIVIDER }, 
    { "&Make",       FL_F + 7, (Fl_Callback *)make_cb },
    { "&Run",    FL_F + 8, (Fl_Callback *)run_cb },
    { "&Compile",    FL_F + 6, (Fl_Callback *)compile_cb },
    { "Make &and Run", FL_F + 9, (Fl_Callback *)make_run_cb },
    { "Make C&lean", 0, (Fl_Callback *)make_clean_cb , 0, FL_MENU_DIVIDER},
    { "&Generate Makefile", 0, (Fl_Callback *)generate_makefile_cb , 0, FL_MENU_DIVIDER},
    { "&Options...",       FL_CTRL + FL_ALT + 'p', (Fl_Callback *)pr_opt_cb },
    { 0 },

  { "&Tools", 0, 0, 0, FL_SUBMENU },
    { "&Fluid",       FL_F + 12, (Fl_Callback *)fluid_cb },
    { "&XTerm",       FL_CTRL + 't', (Fl_Callback *)xterm_cb },
    { 0 }, 

  { "&Help", 0, 0, 0, FL_SUBMENU },
    { "Fltk &Manual",       FL_F + 1, (Fl_Callback *)manual_cb },
    { "&C / C++ Reference",       FL_F + 2, (Fl_Callback *)ref_cb },
    { "&About...",       0, (Fl_Callback *)about_cb },
    { 0 },

  { 0 }
};


void owbt_callback(Fl_Widget*, void*) {
	if(!window->output->visible()) window->show_output();
	else window->hide_output();
	window->redraw();
}


void pr_opt_cb(Fl_Widget*, void*) {
	project_options_window->show();
	pr_changed = 0;
	while(project_options_window->visible()) {Fl::wait(5);}
	if(pr_changed > 0)
	{
		project.name = pname->value();
		project.binfilename = ptarg->value();
		project.oDir = pobdir->value();
		project.Bin = pbin->value();
		project.libs = plib->value();
		project.libdirs = plibdir->value();
		project.incdirs = pincl->value();
		project.ldflags = pldflags->value();
		project.cflags = pcflags->value();

		project.src_files = psrc->value();
	 	project.header_files = phdr->value();

		if(pr_changed == 1) if(fl_ask("Rebuild Makefile?")) generate_makefile_cb();
		project.addToBrowser(window->pr_browser);
		project.modified=true;
	}
}


void open_pr_cb() {
  if (!check_save()) return;
  if(!check_project_save()) return;
  char *newfile = fl_file_chooser("Open Project File", "*.fldev","");
	if(newfile==NULL) return;
  project.pr_filename = newfile;
  project.load();

  char *dir = strdup(newfile);
  char *dirname = strrchr(dir,'/');
  *dirname = '\0';

  if(fl_filename_isdir(dir))  
  {
		
		chdir(dir);
  }
  project.addToBrowser(window->pr_browser); 
  load_file( (char *)window->pr_browser->text(1) , -1 );
  for(int i = 0; i < 9; i++) menuitems[rec_pr_menu_index+i].activate();
  add_recent_project_to_menu(newfile);
  menubar->copy(menuitems, window);
}


void save_pr_cb() {
  project.save();
}


void new_pr_cb() {
  if (!check_save()) return;
  if(!check_project_save()) return;
  pr_wiz_ok = false;
  proj_wiz->show();

  while(proj_wiz->visible()) {
	Fl::wait(5);
  }
  if(!pr_wiz_ok) return;
  
  project.name = trim(pr_name_input->value());
  project.pr_dir = trim(pr_dir_input->value());
  project.pr_filename = project.pr_dir + "/" + project.name + ".fldev";

  char *tmpbuf = strdup(project.name.c_str());

  for(int i = 0; i < strlen(tmpbuf); i++) {
		if(isupper(*(tmpbuf+i))) *(tmpbuf+i) = tolower(*(tmpbuf+i));
  }

  project.binfilename = tmpbuf;
  project.oDir = ".";
  project.Bin = ".";

  if(x_app->value()) {
  	project.libs = "-lX11";
  	project.libdirs = "-L/usr/X11R6/lib";
  	project.incdirs = "-I/usr/X11R6/include";
	project.run_in_console = false;
  }
  else {
  	project.libs = "";
  	project.libdirs = "";
  	project.incdirs = "";
	project.run_in_console = true;
	console_check->value(1);
  }
  

  chdir(project.pr_dir.c_str());
  
  window->pr_browser->clear();
  project.src_files = " ";
  project.header_files = "";
  
  //build main.cpp
  if(main_cpp_chk->value()) {
	  project.src_files = "main.cpp";
	  project.header_files = "";
	  build_template_main();
	  generate_makefile_cb();
	  project.addToBrowser(window->pr_browser);
	  load_file( (char *)(window->pr_browser)->text(1) , -1 );
	  add_recent_file_to_menu(filename);
  }
  else new_cb(0,0);
  project.save();
  project.load();
  

  for(int i = 0; i < 8; i++) menuitems[rec_pr_menu_index+i].activate();
  menubar->copy(menuitems, window);
  add_recent_project_to_menu(strdup(project.pr_filename.c_str()));
  
  //set_title(window);
}


void pref_cb(Fl_Widget*, void*) {
	text_size_choice->value((text_size - 6)/2);
	save_wsoe_check->value(save_window_size);
	hide_output_check->value(auto_hide);
	
	plain_btn->color(hl_plain);
	line_btn->color(hl_linecomment);
	block_btn->color(hl_blockcomment);
	string_btn->color(hl_string);
	directive_btn->color(hl_directive);
	type_btn->color(hl_type);
	keyword_btn->color(hl_keyword);
	character_btn->color(hl_character);
	bg_btn->color(background_color);


	pref_window->show();
	while(pref_window->visible()) {Fl::wait(5);}
	set_text_size(text_size);
	te->smart_indent = smart_indent_check->value();
}


void show_browser_cb()
{
	if(window->tabs->Fl_Widget::visible()) {
		window->hide_browser();
		menuitems[25].clear();
	}
	else {
		window->show_browser();
		menuitems[25].set();	
	}
	
	menubar->copy(menuitems, window);
}


void generate_makefile_cb()
{
	if(!project.assigned) return;
	char buf[555];
	int BUFSIZE=555;
	char *homedir = getenv("HOME");
	sprintf(buf,"Makefile");
	FILE *ptr = fopen(buf,"w");
	
	if(!window->output->visible()) window->show_output();

  	outputtb->remove(0,outputtb->length());
  	op_stylebuf->remove(0,op_stylebuf->length());
  	outputtb->append("Generating Makefile...");

	
	char *src_files = strdup(project.src_files.c_str());
		char *fname;
	string exobjs;

		while(1)
		{			
			fname = strsep(&src_files," ");

			char *first = strsep(&fname,".");
			exobjs += "$(oDir)/";
			exobjs += first;
			exobjs += ".o ";

			if(src_files == NULL || src_files == "") break;
		}

	if(ptr != NULL) 
	{
		fprintf(ptr,"# Standard defines:\n\nCC\t=\tg++\nLD\t=\tg++\nCCOMP\t=\tgcc\n");
		fprintf(ptr,"oDir\t=\t%s\n",	project.oDir.c_str());
		fprintf(ptr,"Bin\t=\t%s\n",		project.Bin.c_str());
		fprintf(ptr,"libDirs\t=\t%s\n",		project.libdirs.c_str());
		fprintf(ptr,"incDirs\t=\t%s\n",		project.incdirs.c_str());
		fprintf(ptr,"LD_FLAGS\t=\t%s\n",	project.ldflags.c_str());
		fprintf(ptr,"LIBS\t=\t%s\n",		project.libs.c_str());
		fprintf(ptr,"C_FLAGS\t=\t%s\n",		project.cflags.c_str());
		fprintf(ptr,"SRCS\t=\t%s\n",		project.src_files.c_str());
		fprintf(ptr,"EXOBJS\t=\t%s\n",		exobjs.c_str());
		fprintf(ptr,"ALLOBJS\t=\t%s\n",		"$(EXOBJS)");
		fprintf(ptr,"ALLBIN\t=\t$(Bin)/%s\n",		project.binfilename.c_str());
		fprintf(ptr,"ALLTGT\t=\t$(Bin)/%s\n",		project.binfilename.c_str());

		fprintf(ptr,"\n#@# Targets follow ---------------------------------\n");
		fprintf(ptr,"all:\t$(ALLTGT)\n");
		fprintf(ptr,"objs:\t$(ALLOBJS)\n");
		fprintf(ptr,"cleanobjs:\n\trm -f $(ALLOBJS)\n");
		fprintf(ptr,"cleanbin:\n\trm -f $(ALLBIN)\n");
		fprintf(ptr,"clean:\tcleanobjs cleanbin\n");
		fprintf(ptr,"cleanall:\tcleanobjs cleanbin\n");

		fprintf(ptr,"\n#@# Dependency rules follow -----------------------------\n");
		fprintf(ptr,"$(Bin)/%s: $(EXOBJS)\n",	project.binfilename.c_str());
		fprintf(ptr,"\t$(LD) -o $(Bin)/%s $(EXOBJS) $(incDirs) $(libDirs) $(LD_FLAGS) $(LIBS)\n",	project.binfilename.c_str());

		char *src_files = strdup(project.src_files.c_str());
		char *fname;

		while(1)
		{			
			fname = strsep(&src_files," ");

			char *first = strsep(&fname,".");
			char *ext = strsep(&fname,"\n");

			fprintf(ptr,"\n$(oDir)/%s.o: %s.%s %s\n",		first,first,ext,project.header_files.c_str());
			//fprintf(ptr,"\n$(oDir)/%s.o: %s.%s %s Makefile\n",		first,first,ext,project.header_files.c_str());
			fprintf(ptr,"\t$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<\n");
			if(src_files == NULL || src_files == "") break;
		}

		fclose(ptr);
	  	outputtb->append("OK");
	}

}



void pr_browser_cb(Fl_Widget* o, void*) {
	if(Fl::event_clicks()>0) {
		Fl::event_clicks(-1);
  		if (!check_save()) return;
		load_file( (char *)((Fl_Browser*)o)->text( ((Fl_Browser*)o)->value() ) , -1 );
		add_recent_file_to_menu(filename);
	}
}



void file_browser_cb(Fl_Widget* o, void*) {
	if(Fl::event_clicks()>0) {
		Fl::event_clicks(-1);
		int val = ((Fl_Browser*)o)->value();
		char *browser_text = (char *)((Fl_Browser*)o)->text( val );

		char *l = strdup(window->fb_label->value());
		string s = "";
		s += l;
		s += browser_text;
		free(l);
		l = strdup(s.c_str());
	
		if(fl_filename_isdir(l)) {

			if(strcmp(browser_text,"../") == 0) {
				if(strcmp(l,"/../") == 0) return;
				char *a = strrchr(l,'/');
				*a = '\0';
				a = strrchr(l,'/');
				*a = '\0';
				a = strrchr(l,'/');
				*(a+1) = '\0';
			}
			window->fb_label->value(l);
			window->file_browser->load(window->fb_label->value());
			return;
		}

  		if (!check_save()) return;
		load_file( l , -1 );
		add_recent_file_to_menu(filename);
	}
}





void show_help(const char *name) {
  const char	*docdir;
  char		helpname[1024];
  
  if (!help_dialog) help_dialog = new Fl_Help_Dialog();

  if ((docdir = getenv("FLTK_DOCDIR")) == NULL) {

    docdir = "/usr/local/share/doc/fltk";
  }
  snprintf(helpname, sizeof(helpname), "%s/%s", docdir, name);  

  help_dialog->load(helpname);
  help_dialog->resize(help_dialog->x(),help_dialog->y(),760,540);
  help_dialog->show();
}


int ask_for_download()
{
	char *homedir = getenv("HOME");
	if(!fl_ask("Cpp-Reference not found!\nDo you want to download?")) return 0;
	else {
		Fl::wait(5);
		FILE *ptr;
		char command[1024];
		char buf[1024];

		ptr = fopen("p.sh","w");
		fprintf(ptr, \
"wget http://www.cppreference.com/cppreference-files.tar.gz\n \
gunzip cppreference-files.tar.gz\n \
tar -xf cppreference-files.tar\n \
cp -ra html %s/.cppref\n \
cd html \n \
for file in $(find . -name '*.html') \n \
do \n\
	echo \"Processing $file\"\nperl -p -e 's/href=\"\\//href=\"file:\\/\\/$ENV{HOME}\\/.cppref\\//g;s/\\/\"/\\/index.html\"/g' $file > tmp.txt\nmv tmp.txt $HOME/.cppref/$file \n \
done\n",homedir);
		fclose(ptr);

		ptr = popen("xterm -e sh p.sh","r");
		pclose(ptr);

		ptr = popen("rm -f cppreference-files.tar ","r");
		pclose(ptr);

		ptr = popen("rm -f p.sh ","r");
		pclose(ptr);

		ptr = popen("rm -rf html ","r");
		pclose(ptr);
		
		return 1;
	}
}

void show_ref(const char *name) {
  char	*docdir;
  char		helpname[1024];
  char *homedir = getenv("HOME");

  if (!help_dialog) help_dialog = new Fl_Help_Dialog();

  if ((docdir = getenv("CPP_REF_DOCDIR")) == NULL) {
	free(docdir);
	docdir = (char*)malloc((strlen(homedir)+strlen("/.cppref")+1)*sizeof(char));
    sprintf(docdir,"%s/.cppref",homedir);
  }
  if(!fl_filename_isdir(docdir)) if(!ask_for_download()) return;
  snprintf(helpname, sizeof(helpname), "%s/%s", docdir, name);  

  help_dialog->load(helpname);
  help_dialog->resize(help_dialog->x(),help_dialog->y(),760,540);
  help_dialog->show();
}

void manual_cb(Fl_Widget *, void *) {
  show_help("index.html");
}


void ref_cb(Fl_Widget *, void *) {
  show_ref("index.html");
}



void load_config_file()
{
	char buf[255],file1[255],file2[255],file3[255],file4[255], pr1[255], pr2[255], pr3[255], pr4[255],scheme[255];
	memset(file1,'\0',255); memset(file2,'\0',255); memset(file3,'\0',255); memset(file4,'\0',255);
	int x,y,w,h,sm_in,rp_al;
	int BUFSIZE=255;
	char *homedir = getenv("HOME");
	sprintf(buf,"%s/.fldev_cfg",homedir);
	FILE *ptr = fopen(buf,"r");

	if(ptr != NULL) {
		if(fscanf(ptr,"%d %d %d %d %d\n%d\n%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
					&save_window_size,
					&x,
					&y,
					&w,
					&h,
					&auto_hide,
					&text_size,
					scheme,
					file1,
					file2,
					file3,
					file4,
					pr1,
					pr2,
					pr3,
					pr4,
					&sm_in,
					&rp_al,//
					&hl_plain,
					&hl_linecomment,
					&hl_blockcomment,
					&hl_string,
					&hl_directive,
					&hl_type,
					&hl_keyword,
					&hl_character,
					&background_color )) {
			if(strlen(file4)>3) add_recent_file_to_menu(file4+3);
			if(strlen(file3)>3) add_recent_file_to_menu(file3+3);
			if(strlen(file2)>3) add_recent_file_to_menu(file2+3);
			if(strlen(file1)>3) add_recent_file_to_menu(file1+3);
			if(strlen(pr4)>3) add_recent_project_to_menu(pr4+3);
			if(strlen(pr3)>3) add_recent_project_to_menu(pr3+3);
			if(strlen(pr2)>3) add_recent_project_to_menu(pr2+3);
			if(strlen(pr1)>3) add_recent_project_to_menu(pr1+3);
			
			Fl::scheme(scheme);
			if(save_window_size) window->resize(x,y,w,h);
			te->smart_indent = sm_in;
			smart_indent_check->value(sm_in!=0);
			rec_pr_check->value(rp_al!=0);
		}
		else fl_alert("Error reading config file!");
  		fclose(ptr);
	}

}






void save_config_file()
{
	char buf[255];
	int BUFSIZE=255;
	char *homedir = getenv("HOME");
	sprintf(buf,"%s/.fldev_cfg",homedir);
	FILE *ptr = fopen(buf,"w");
	int startitem = rec_pr_menu_index - 8;

	if(ptr != NULL) 
	{
		fprintf(ptr,"%d %d %d %d %d\n%d\n%d\n%s\nf1:%s\nf2:%s\nf3:%s\nf4:%s\np1:%s\np2:%s\np3:%s\np4:%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", save_window_size,window->x(),window->y(),window->w(),window->h(),auto_hide,text_size,Fl::scheme(),
				menuitems[4].label(),menuitems[5].label(),menuitems[6].label(),menuitems[7].label(),
				menuitems[startitem+4].label(),menuitems[startitem+5].label(),menuitems[startitem+6].label(),menuitems[startitem+7].label(),te->smart_indent,rec_pr_check->value(),
				hl_plain,
				hl_linecomment,
				hl_blockcomment,
				hl_string,
				hl_directive,
				hl_type,
				hl_keyword,
				hl_character,
				background_color);
		fclose(ptr);
	}

}








void recent_file_cb(Fl_Widget* w, void*)
{
  if (!check_save()) return;
	Fl_Menu_* mw = (Fl_Menu_*)w;
  	const Fl_Menu_Item* m = mw->mvalue();
  	if (m) {
		char *newfile = (char*)m->label();
		char *path = strdup(newfile);

	 	if (newfile != NULL) 
		{
			char *slash;
	    	slash = strrchr(path, '/');
			*slash = '\0';
			load_file(newfile, -1);
			add_recent_file_to_menu(newfile);
		}
	}
}





void recent_project_cb(Fl_Widget* w, void *v)
{
  if (!check_save()) return;
  if (!check_project_save()) return;

	char *newfile;
	Fl_Menu_* mw = (Fl_Menu_*)w;
	
	if(w) {
		const Fl_Menu_Item* m = mw->mvalue();
		if(m) newfile = (char*)m->label();
		else return;
  	}
	else newfile = (char*)v;
	
	char *path = strdup(newfile);

 	if (newfile != NULL) 
	{
		char *slash;
    	slash = strrchr(path, '/');
		*slash = '\0';
		project.pr_filename = newfile;
		if(!project.load()) return;
		//add_recent_project_to_menu(newfile);
		char *dir = strdup(newfile);
 
		  char *dirname = strrchr(dir,'/');
		  *dirname = '\0';

		  if(fl_filename_isdir(dir))  
		  {
				chdir(dir);
		  }
		  project.addToBrowser(window->pr_browser); 
		  if(window->pr_browser->text(1)!=NULL)
		  	load_file( (char *)window->pr_browser->text(1) , -1 );
		  else new_cb(0,0);
		  for(int i = 0; i < 9; i++) menuitems[rec_pr_menu_index+i].activate();
		  add_recent_project_to_menu(newfile);
		  menubar->copy(menuitems, window);
		 
	}
	
}





void add_recent_project_to_menu(char *filename)
{
	char buf[255];
	fl_filename_absolute(buf,filename);
	filename = buf;
	int startitem = rec_pr_menu_index - 8;

	if(filename=="") return;

	if(strcmp(filename,menuitems[startitem+4].label()) == 0) return;

	else if(strcmp(filename,menuitems[startitem+5].label()) == 0) {
		Fl_Menu_Item mi = menuitems[startitem+5];
		menuitems[startitem+5] = menuitems[startitem+4];
		menuitems[startitem+4] = mi;
	}

	else if(strcmp(filename,menuitems[startitem+6].label()) == 0) {
		Fl_Menu_Item mi = menuitems[startitem+6];
		menuitems[startitem+6] = menuitems[startitem+5];
		menuitems[startitem+5] = menuitems[startitem+4];
		menuitems[startitem+4] = mi;
	}

	else {
		Fl_Menu_Item mi = { strdup(filename), 0, recent_project_cb, 0 ,0 };
		menuitems[startitem+7] = menuitems[startitem+6];
		menuitems[startitem+6] = menuitems[startitem+5];
		menuitems[startitem+5] = menuitems[startitem+4];
    	menuitems[startitem+4] = mi;
	}
	
	menubar->copy(menuitems, window);
}





void add_recent_file_to_menu(char *filename)
{
	char buf[255];
	fl_filename_absolute(buf,filename);
	filename = buf;

	if(filename=="") return;

	if(strcmp(filename,menuitems[4].label()) == 0) return;

	else if(strcmp(filename,menuitems[5].label()) == 0) {
		Fl_Menu_Item mi = menuitems[5];
		menuitems[5] = menuitems[4];
		menuitems[4] = mi;
	}

	else if(strcmp(filename,menuitems[6].label()) == 0) {
		Fl_Menu_Item mi = menuitems[6];
		menuitems[6] = menuitems[5];
		menuitems[5] = menuitems[4];
		menuitems[4] = mi;
	}

	else {
		Fl_Menu_Item mi = { strdup(filename), 0, recent_file_cb };
		menuitems[7] = menuitems[6];
		menuitems[6] = menuitems[5];
		menuitems[5] = menuitems[4];
    	menuitems[4] = mi;
	}
	
	menubar->copy(menuitems, window);
}



void set_text_size(int t)
{
//Fl_Color hl_plain, hl_linecomment, hl_blockcomment, hl_string, hl_directive, hl_type, hl_keyword, hl_character¸background_color;
			styletable[0].size = t;
			styletable[0].color = hl_plain;
			
			styletable[1].size = t;			
			styletable[1].color = hl_linecomment;
			
			styletable[2].size = t;
			styletable[2].color = hl_blockcomment;
						
			styletable[3].size = t;
			styletable[3].color = hl_string;
			
			styletable[4].size = t;
			styletable[4].color = hl_directive;
			
			styletable[5].size = t;
			styletable[5].color = hl_type;
			
			styletable[6].size = t;
			styletable[6].color = hl_keyword;
			
			styletable[7].size = t;
			styletable[7].color = hl_character;
			
			te->color(background_color);
			te->mCursor_color = hl_plain;
		  
			op_styletable[0].size = t;
			op_styletable[1].size = t;
}



void home_cb(Fl_Widget* w, void*)
{
	window->fb_label->value(strdup((getenv("HOME")+string("/")).c_str()));
	window->file_browser->load(getenv("HOME"));
}

class SmartButton : public Fl_Button
{
	public:
		SmartButton(int x, int y, int w, int h) : Fl_Button(x,y,w,h) { 
			box(FL_NO_BOX); 
			down_box(FL_DOWN_BOX); 
			
		}
		
		void draw()
		{
			if (type() == FL_HIDDEN_BUTTON) return;
  			Fl_Color col = value() ? selection_color() : color();
			  draw_box(value() ? (down_box()?down_box():fl_down(box())) : box(), col);
			  draw_label();	
		}
		
};

Fl_Window* make_form() {
    EditorWindow* w = new EditorWindow(660, 400, title);
    window = w;
	int pr_browsersize = 150;
	int tabs_size = 150;
    w->begin();

    menubar = new Fl_Menu_Bar(0, 0, 660, 25);    
	menuitems[25].set();
    menubar->copy(menuitems, w);
    menubar->box(FL_THIN_UP_BOX);
    menubar->down_box(FL_BORDER_BOX);
	menubar->selection_color(FL_WHITE);
	
	
	/////////////////////////////////////////////////////////////////////
	//SmartIcons
	Fl_Group *smartbar = new Fl_Group(0,25,660,25);
	smartbar->box(FL_THIN_UP_BOX);
	int temp_space = 0, ts_size = 10;
	smartbar->begin();
	int count = 0;
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_new);
		o->callback(new_cb);
		o->tooltip("New File");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_open);
		o->callback(open_cb);
		o->tooltip("Open File");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_save);
		o->callback((Fl_Callback *)save_cb);
		o->tooltip("Save File");
		smartbar->add(o);
		count++;
	}
	{//SPACER
		Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX,2+22*count + temp_space + ts_size/2,26,1,22,"");
		smartbar->add(o);
		temp_space+=ts_size;
	}//
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_browser);
		o->callback((Fl_Callback *)show_browser_cb);
		o->tooltip("Toggle File Browser");
		smartbar->add(o);
		count++;
	}
	{//SPACER
		Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX,2+22*count + temp_space + ts_size/2,26,1,22,"");
		smartbar->add(o);
		temp_space+=ts_size;
	}//
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_copy);
		o->callback(copy_cb);
		o->tooltip("Copy");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_cut);
		o->callback(cut_cb);
		o->tooltip("Cut");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_paste);
		o->callback(paste_cb);
		o->tooltip("Paste");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_undo);
		o->callback(undo_cb);
		o->tooltip("Undo");
		smartbar->add(o);
		count++;
	}
	{//SPACER
		Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX,2+22*count + temp_space + ts_size/2,26,1,22,"");
		smartbar->add(o);
		temp_space+=ts_size;
	}//
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_search);
		o->callback(find_cb);
		o->tooltip("Search");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_search_and_replace);
		o->callback(replace_cb);
		o->tooltip("Replace");
		smartbar->add(o);
		count++;
	}
	{//SPACER
		Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX,2+22*count + temp_space + ts_size/2,26,1,22,"");
		smartbar->add(o);
		temp_space+=ts_size;
	}//
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_compile);
		o->callback(make_cb);
		o->tooltip("Make");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_run);
		o->callback(run_cb);
		o->tooltip("Run");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_compile_run);
		o->callback(make_run_cb);
		o->tooltip("Make and run");
		smartbar->add(o);
		count++;
	}
	{//SPACER
		Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX,2+22*count + temp_space + ts_size/2,26,1,22,"");
		smartbar->add(o);
		temp_space+=ts_size;
	}//
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_fluid);
		o->callback(fluid_cb);
		o->tooltip("Fluid Form Editor");
		smartbar->add(o);
		count++;
	}
	{
		SmartButton *o = new SmartButton(2+22*count + temp_space,26,22,22);
		o->image(image_term);
		o->callback(xterm_cb);
		o->tooltip("XTerm");
		smartbar->add(o);
		count++;
	}
	
	smartbar->resizable(NULL);
	smartbar->end();


	/////////////////////////////////////////////////////////////////////



    w->editor = new Fl_Text_Editor_ext(pr_browsersize, 50, 660-tabs_size, 334);	//edited
	te = w->editor;
	
    w->editor->buffer(textbuf);
    w->editor->highlight_data(stylebuf, styletable,
                              sizeof(styletable) / sizeof(styletable[0]),
			      'A', style_unfinished_cb, 0);
    w->editor->textfont(FL_COURIER);
    w->editor->end();
    
	te->color(background_color);
	te->mCursor_color = hl_plain;
	
    w->statusbar = new Fl_Box(FL_EMBOSSED_BOX,0,384,615,16,"Zeile 1"); 

    w->outputwindowbutton = new Fl_Button(620,384,40,16,"Output");
    w->outputwindowbutton->callback(owbt_callback);
    w->outputwindowbutton->visible_focus(0);
    w->outputwindowbutton->labelsize(10);

    w->output = new My_Text_Editor2(0,285,660,99);
	w->output->highlight_data(op_stylebuf, op_styletable,
                              sizeof(op_styletable) / sizeof(op_styletable[0]),
			      'A', style_unfinished_cb, 0);
    w->output->textsize(12);
    w->output->buffer(outputtb);
    w->output->hide(); 
    w->output->when(FL_WHEN_CHANGED); 
    w->output->hide_cursor();
    w->output->output();
    w->output->end();

	int browser_offset = 5;
	w->tabs = new Fl_Tabs(0,50,pr_browsersize,334);
		Fl_Group *o = new Fl_Group(0,w->tabs->y()+20,w->tabs->w(),w->tabs->h()-20,"Project");
			w->pr_browser = new Fl_Hold_Browser(o->x(),o->y()+browser_offset,o->w(),o->h()-browser_offset);
			w->pr_browser->callback(pr_browser_cb);
			o->resizable(w->pr_browser);
		o->end();
		o = new Fl_Group(0,w->tabs->y()+20,w->tabs->w(),w->tabs->h()-20,"Browse Dir");

			w->fb_label = new Fl_Output(o->x(),o->y()+browser_offset,o->w()-20,20);
			w->fb_label->value(strdup((getenv("PWD")+string("/")).c_str()));
			w->fb_home_btn = new Fl_Button(o->x()+o->w()-19,o->y()+browser_offset,18,20,"~/");
			w->fb_home_btn->box(FL_THIN_UP_BOX);
			w->fb_home_btn->callback(home_cb);
			w->file_browser = new My_File_Browser(o->x(),o->y()+20+browser_offset,o->w(),o->h()-20-browser_offset);
			w->file_browser->load(getenv("PWD"));
			w->file_browser->callback(file_browser_cb);
			o->resizable(w->file_browser);
		o->end();
		w->tabs->resizable(o);
	w->tabs->end();
  
    w->statusbar->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    w->statusbar->labelsize(10);
    w->resizable(w->editor);
    w->callback((Fl_Callback *)quit_cb, w);

    w->end();
    
    
    textbuf->add_modify_callback(style_update, w->editor);
    textbuf->add_modify_callback(changed_cb, w);
    textbuf->call_modify_callbacks();
    textbuf->tab_distance(4);


	make_pref_form();
	make_proj_window();
	make_proj_wizard();

    return w;
}



int main(int argc, char **argv) {
  Pixmap p, mask;
  fl_open_display();  
  XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display),fldevicon_xpm, &p, &mask, NULL);
  
  textbuf = new Fl_Text_Buffer;
  outputtb  = new Fl_Text_Buffer;
  style_init();
  op_stylebuf = new Fl_Text_Buffer();
  window = (EditorWindow*)make_form();
  window->icon((char *)p);
  window->show(1,argv);
  //window->show();
  
  //Set Icon Mask for transparent Icon
  XWMHints *hints = XGetWMHints(fl_display, fl_xid(window));
  hints->icon_mask = mask;
  hints->flags       |= IconMaskHint;// | IconPixmapHint;  
  XSetWMHints(fl_display, fl_xid(window), hints);
    
  file_hist = new File_History();
  text_size = 12;
  load_config_file();

  int r;
  if (argc > 1) {
 	r=load_file(argv[1], -1);
	//open file
	if(r == 1) {
		char buf[255];
		sprintf(buf,"%s/%s",getenv("PWD"),filename_wo_path);
		add_recent_file_to_menu(buf);
		window->hide_browser();
	}
	//open project
	else if(r==2) {
		//open first file of project
	  	if(window->pr_browser->text(1) != NULL) ( (char*)window->pr_browser->text(1) , -1 );
    }
  }
  else if(rec_pr_check->value()) {
		//open last opened project
		recent_project_cb(0,(void*)menuitems[rec_pr_menu_index-4].label());
  }
  else window->label("FLDev");
  
  set_text_size(text_size);

  int oldzeile = -1, oldspalte = -1;

  if(!project.assigned) {
		for(int i = 0; i < 9; i++) menuitems[rec_pr_menu_index+i].deactivate();
		menubar->copy(menuitems, window);
  }

  int old_ins_mode;
  
  while(1)
  {
	Fl::wait(5);
	if(!window->visible()) continue;
	char b[255];
 	int zeile = window->editor->count_lines(0,window->editor->insert_position(),0) + 1;
    int spalte = window->editor->insert_position() - window->editor->line_start(window->editor->insert_position());
  	int ins_mode = window->editor->insert_mode();
        if(zeile != oldzeile || spalte != oldspalte || ins_mode != old_ins_mode)
		{
			int gz = window->editor->count_lines(0,textbuf->length(),0)+1;
			if(ins_mode) sprintf(b,"row %d (%d), col %d \tINSERT",zeile, gz, spalte);
			else sprintf(b,"row %d (%d), col %d \tOVERWRITE",zeile, gz, spalte);
  			window->statusbar->label(b);
			oldzeile = zeile;
			oldspalte = spalte;
			old_ins_mode = ins_mode;
		}
	//Fl::focus(window);
		
  }
  return 1;
}
