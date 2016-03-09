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
#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#ifdef _WIN32
extern char *strsep(char **, const char *);
#endif

//output for make, could be replaced by Fl_Browser in the future
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

				if(strcmp(t_filename,filename) != 0) {
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
		else return Fl_Group::handle(event);
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
    void hide_output();	
    void show_output();
    void hide_browser();	
    void show_browser();
    void hide_linenrs();	
    void show_linenrs();
	
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
	Fl_Menu_Button		*fb_home_btn;
    Fl_Text_Editor_ext 	*editor;
    My_Text_Editor2		*output;
    Fl_Box				*line_nr_box;

	Fl_File_Browser	 	*file_browser;
	Fl_File_Browser 	*pr_browser;
    char               	search[256];

};



#endif
