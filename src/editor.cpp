/////////////////////////////////////////////////////////////////////////////////////////
/*
        FLDev

        Lightweight Integrated Development Environment
*/
#if MSDOS
#define VERSION 	"130_DOS-1.5.8"
#elif _WIN32
#define VERSION 	"130_WIN32-1.5.8"
#else
#define VERSION 	"130_x11-1.5.8"
#endif
/*		author:		Philipp Pracht
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
        improve navigator
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <iostream>
#include <deque>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <wchar.h>
#else
#include <unistd.h>
#include <X11/xpm.h>
#endif
#include "icon_pixmaps.h"
#include "globals.h"

#ifdef __MWERKS__ 
#define FL_DLL
#endif

#ifdef MSDOS
#include <sys/exceptn.h>
#include <crt0.h>
int _crt0_startup_flags = _CRT0_FLAG_NULLOK | _CRT0_FLAG_FILL_SBRK_MEMORY; //disable null pointer protection

#include <mwin/nano-X.h>
extern "C" {
    void toggle_text_mode();
}
#endif

/*
//the following defines are intended to be set with -D in the makefile - gp
USE_NXLIB
WITHDEBUG
WITHHINTS //do not use that?
FOR_NANOLINUX
*/

//#include "My_File_Browser.h"

#include "globals.h"

#include "highlight.h"
#include "build_tools.h"
#include "EditorWindow.h"


using namespace std;

// Index for insertion of most recent projects
int	rec_pr_menu_index = 49;
int bufchanged = 0;
char filename[256] = "";
char filename_wo_path[256] = "";
char title[256], usrdocdir[256];
bool hidden = true, cppfile, filelistopen = false, make_error = false;
bool exec_running = false, nav_expand = false, show_line_nrs = false;
int linecount, update_count;
int num_windows = 0;
int loading = 0;
int nav_sort = 0;
int show_nav = 0;
int line_nr_size = 50;


Fl_Text_Buffer *outputtb = 0;
Project project;
File_History *file_hist;
Fl_Menu_Bar* menubar;
Fl_Help_Dialog *help_dialog;
EditorWindow* window;
Fl_Group *smartbar;
Fl_Group *output_grp;
Fl_Tile *tile;
deque <string> bakfiles;

NavBrowser *navigator_browser;

deque<Nav_entry> Nav_entry::children;

Fl_Group *browser_file_grp, *browser_nav_grp;
My_Group_wo_Nav *all_but_menu_grp;
Fl_Menu_Button *navigator_pop_btn, *fb_pop_btn, *file_pop_btn;
My_Group_wo_Nav *gwn;



Fl_Text_Display::Style_Table_Entry
op_styletable[] = {	// Style table
  { FL_BLACK,      FL_HELVETICA, TEXTSIZE }, 	// A - Plain
  { FL_RED, 		  FL_HELVETICA, TEXTSIZE } 	// B - Error
};

Fl_Text_Display::Style_Table_Entry
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


//File Icon defs
enum				// Data opcodes
{
    END,			// End of primitive/icon
    COLOR,			// Followed by color value (2 shorts)
    LINE,			// Start of line
    CLOSEDLINE,			// Start of closed line
    POLYGON,			// Start of polygon
    OUTLINEPOLYGON,		// Followed by outline color (2 shorts)
    VERTEX			// Followed by scaled X,Y
};

//the strings that are translated
//Main Mode
string file_str = "File",
mod_str = "modified",
insert_str = "INSERT",
overwrite_str = "OVERWRITE",
row_str = "row",
col_str = "col",
cancel_str = "Cancel",
save_str = "Save",
discard_str = "Discard";

string strmsg[STR_MSG_SIZE];
string line_label_str;

void init_strings() {
    strmsg[0] = "Cpp-Reference not found!\nDo you want to download?";
    strmsg[1] = "The current file has not been saved.\nWould you like to save it now?";
    strmsg[2] = "The current Project has not been saved.\nWould you like to save it now?";
    strmsg[3] = "Open file";
    strmsg[4] = "Insert file";
    strmsg[5] = "Open Project File";
    strmsg[6] = "Rebuild Makefile?";
    strmsg[7] = "Save File As?";
    strmsg[8] = "Search String";
    strmsg[9] = "Replaced %d occurrences.";
    strmsg[10] = "You have to restart FLDev!";
    strmsg[11] = "Error reading from file";
    strmsg[12] = "Error writing to file";
    strmsg[13] = "Program already running!";
    strmsg[14] = "Name of executable file not defined.\nCheck project options!";
    strmsg[15] = "No occurrences of \'%s\' found!";
    strmsg[16] = "Error reading config file!";
    strmsg[17] = "Project File not found!";
}

string get_load_err_msg() {
    return strmsg[17];
}

//callback functions
void owbt_callback(Fl_Widget*, void*);
void pr_opt_cb(Fl_Widget*, void*);
void open_pr_cb();
void save_pr_cb();
void new_pr_cb();
void pref_cb(Fl_Widget*, void*);
void show_browser_cb();
void linenumbers_cb();
void show_nav_cb();
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
void run_cb(Fl_Widget*, void*);
void make_(Fl_Widget* w, void* v, int mode = 0);

deque <Nav_entry> funs;
deque <Nav_entry> classes;
deque <Nav_entry> structs;
deque <Nav_entry> enums;
deque <Nav_entry> unions;


/*
 * Mechanism to handle determining *where* the exe actually lives
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

 /********************************************************************************************/
#ifdef WIN32
#include <windows.h> /* GetModuleFileName */
#endif /* WIN32 */

#ifndef PATH_MAX
#  define PATH_MAX 2048
#endif

//http://msdn.microsoft.com/en-us/library/ms682512.aspx - Creating processes
void wprocess(char* cmdstring, int nowin) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    int proc_creation_flags = 0;
    if (nowin == 1) proc_creation_flags = CREATE_NO_WINDOW; // No console window
    if (!CreateProcess(NULL,   // No module name (use command line)
                       cmdstring,        // Command line
                       NULL,           // Process handle not inheritable
                       NULL,           // Thread handle not inheritable
                       FALSE,          // Set handle inheritance to FALSE
                       proc_creation_flags, // creation flags	
                       NULL,           // Use parent's environment block
                       NULL,           // Use parent's starting directory 
                       &si,            // Pointer to STARTUPINFO structure
                       &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}






int get_app_path(char *pname, size_t pathsize) {
    long result;

#ifdef WIN32
    result = GetModuleFileName(NULL, pname, pathsize);
    if (result > 0)
    {
        /* fix up the dir slashes... */
        int len = strlen(pname);
        int idx;
        for (idx = 0; idx < len; idx++)
        {
            if (pname[idx] == '\\') pname[idx] = '/';
        }
        //slash = strrchr(path, '/');               
        //*slash = '\0';

        //remove tailing slash if present
        if (pname[len - 1] == '/') pname[len - 1] = '\0';
        //remove filename
        char* slash = strrchr(pname, '/');
        *slash = '\0';
        //if ((access(pname, 0) == 0))
        return 0; /* file exists, return OK */
/*else name doesn't seem to exist, return FAIL (falls through) */
    }
#endif /* WIN32 */

    return -1; /* Path Lookup Failed */

} /* where_do_I_live */

/* Example:
int main()
{
        char pname[PATH_MAX];
        int err = get_app_path(pname, PATH_MAX);

        if (err) puts("Name lookup failed!");
        else printf("Got: %s\n", pname);
        return 0;
}
*/
/********************************************************************************************/


void navigator_update(char *text, char *style, int length) {

    int scroll_pos = navigator_browser->position();
    navigator_browser->clear();

    funs.clear();
    classes.clear();
    structs.clear();
    enums.clear();
    unions.clear();

    string tmp_str = "";
    deque <Class_stack_item> last_class;
    int last_class_depth = 0;
    //cout << "update" << endl;

    int depth = 0;

    for (int i = 0; i < length; i++)
    {
        if (style[i] > 'A' && style[i] < 'F')
        {

            continue;
        }

        /*if(i < length -6 && strncmp(text+i," class ",7))
        {
            cout << "class found" << endl;
        }*/
        if (text[i] == '}')
        {
            depth--;
            if (last_class.size() > 0 && depth < last_class[0].depth) last_class.pop_front();
        }

        else if (text[i] == '{')
        {

            depth++;

            int is_class_d = 0;
            int mode = 0;
            int pos = i - 1;

            string func_name = "";
            string func_type = "";
            string func_args = "";
            string class_str = "";
            string method_class_str = "";

            for (int k = i - 1; k >= 0; k--)
            {


                if (mode < 0) break;

                char chr = text[k];
                char sty = style[k];


                if (sty > 'A' && sty < 'F')
                { 	//ignore comments, strings and directives			
                    continue;
                }


                if (mode == 0)
                {		//search for )
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    if (chr == ')') mode = 1;
                    else if (chr == 't' && strncmp(text + (k - 4), "const", 5) == 0)
                    {
                        k -= 5;
                    }
                    else
                    {
                        if (chr < 48 || (chr > 58 && chr < 65) || chr > 122) break;
                        func_name = chr;
                        is_class_d = 1;
                        mode = 10; //maybe class
                    }
                }
                else if (mode == 1)
                {		//search for (
                    if (chr == '\t' || chr == '\n') continue;
                    else if (chr == '(') mode = 2;
                    else if (chr == ')') mode = -1;
                    else if (chr == ' ' && func_args[0] == ' ') continue;
                    else func_args = chr + func_args;
                }
                else if (mode == 2)
                {		//search for last character of function name
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    else
                    {
                        func_name = chr;
                        mode = 3;
                        if (sty != 'A')
                        {

                            func_name = "";
                            break;
                        }
                    }
                }
                else if (mode == 3)
                {		//search for rest of function name
                    if (chr == ' ' || chr == '\t' || chr == '\n') mode = 4;
                    else if (k > 0 && chr == ':' && text[k - 1] != ':' && text[k + 1] != ':')
                    {
                        mode = 0;
                        func_name = "";
                        func_args = "";
                        continue;

                    }
                    else if (k > 0 && chr == ':' && text[k - 1] == ':')
                    {	//class method
                        k--;
                        mode = 6;
                        continue;

                    }
                    else if (chr == ',')
                    {
                        func_name = "";
                        func_args = "";
                        mode = 0;
                        continue;

                    }
                    else
                    {
                        func_name = chr + func_name;
                        pos = k;
                        if (sty != 'A')
                        {

                            func_name = "";
                            break;
                        }
                    }
                }
                else if (mode == 4)
                {	//search for first char of func type
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    else if ((k > 0 && chr == ':' && text[k - 1] != ':') || chr == '}')
                    {
                        if (last_class.size() && last_class[0].name == func_name)
                        {
                            func_type = "";
                            break;
                        }
                        mode = 0;
                        func_name = "";
                        continue;
                        //break;
                    }
                    else if (chr == ';')
                    {
                        break;
                    }
                    else if (chr == ',')
                    {
                        func_name = "";
                        func_args = "";
                        mode = 0;
                        continue;

                    }
                    else if (!(sty == 'F' || sty == 'A'))
                    {
                        cout << func_name << " " << sty << endl;
                        func_name = "";
                        break;
                    }
                    else
                    {
                        func_type = chr;
                        mode = 5;
                    }
                }
                else if (mode == 5)
                { 	//search for rest of type
                    if (chr == '\t' || chr == '\n') chr = ' ';

                    if (chr == ' ' && func_type[0] == ' ') continue;
                    else if (isalnum(chr) || chr == '_' || chr == ' ') func_type = chr + func_type;
                    else
                    {
                        func_type = trim(func_type);
                        break;
                    }
                }

                else if (mode == 6)
                {
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    else
                    {
                        method_class_str = chr;
                        mode = 7;
                    }
                }
                else if (mode == 7)
                {
                    if (chr == ' ' || chr == '\t' || chr == '\n')
                    {
                        if (method_class_str == func_name)
                        {
                            func_type = "";
                            break;
                        }
                        mode = 4;
                    }
                    else method_class_str = chr + method_class_str;
                }




                //class parse
                else if (mode == 10)
                {		//search for last character of class name
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    else if (chr < 48 || (chr > 58 && chr < 65)) break;
                    else
                    {
                        func_name = chr + func_name;
                        mode = 11;
                        if (sty != 'A')
                        {
           //cout << "In 10 " << func_name << " " << sty << endl;
                            func_name = "";
                            break;
                        }
                    }
                }
                else if (mode == 11)
                {		//search for rest of class name
                    if (chr == ' ' || chr == '\t' || chr == '\n') mode = 12;
                    else if (chr == ':')
                    {
                        mode = 10;
                        func_name = "";
                        continue;

                    }
                    else
                    {
                        func_name = chr + func_name;
                        pos = k;
                        if (sty != 'A')
                        {

                            func_name = "";
                            break;
                        }
                    }
                }
                else if (mode == 12)
                {	//search for char before name
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    /*else if(!(sty == 'F')) {
                        cout << func_name << " " << sty << endl;
                        func_name = "";
                        break;
                    }*/
                    else if (chr == ':')
                    {
                        mode = 10;
                        func_name = "";
                        class_str = "";
                        continue;
                        //break;
                    }
                    else
                    {
                        mode = 13;
                        class_str = chr;
                    }
                }
                else if (mode == 13)
                {	//search for rest of word before name
                    if (chr == ' ' || chr == '\t' || chr == '\n')
                    {
                        if (class_str == "class")
                        {
                            is_class_d = 1;
                            break;
                        }
                        else if (class_str == "struct")
                        {
                            is_class_d = 2;
                            break;
                        }
                        else if (class_str == "enum")
                        {
                            is_class_d = 3;
                            break;
                        }
                        else if (class_str == "union")
                        {
                            is_class_d = 4;
                            break;
                        }

                        else if (class_str == "case")
                        {
                            is_class_d = 0;
                            func_name = "";
                            break;
                        }
                        else
                        {
                            mode = 14;
                        }

                    }
                    else
                    {
                        class_str = chr + class_str;
                    }
                }
                else if (mode == 14)
                {	//test if it is inheritance
                    if (chr == ' ' || chr == '\t' || chr == '\n') continue;
                    else if (chr == ':')
                    {
                        mode = 10;
                        //cout << "To 10 " << func_name << " " << class_str << endl;
                        //break;
                        class_str = "";
                        func_name = "";
                        continue;
                    }
                    else
                    {
                        cout << func_name << " " << class_str << endl;
                        //func_name = "";
                        break;
                    }
                }


                //while(style[i] > 'A' && style[i] < 'F') pos--;
            }

            //char line_nr[100];
            //sprintf(line_nr,"%d",pos);

            if (func_type != "") func_type += " ";
            if (last_class.size() > 0)
            {
                method_class_str = last_class[0].name;
            }

            if (func_name.size() > 0)
            {

                if (is_class_d == 1)
                {
                    classes.push_back(Nav_entry(func_name, pos, last_class.size()));
                    last_class.push_front(Class_stack_item(func_name, depth));
                }
                else if (is_class_d == 2) structs.push_back(Nav_entry(func_name, pos));
                else if (is_class_d == 3) enums.push_back(Nav_entry(func_name, pos));
                else if (is_class_d == 4) unions.push_back(Nav_entry(func_name, pos));

                else if (method_class_str != "")
                {
                    bool class_dec_av = false;
                    for (int l = 0; l < classes.size(); l++)
                    {
                        if (classes[l].name == method_class_str)
                        {
                            classes[l].children.push_back(Nav_entry(func_name, pos, func_type, func_args));
                            class_dec_av = true;
                        }
                    }
                    if (!class_dec_av)
                    {
                        Nav_entry tmp(method_class_str, pos);
                        tmp.children.push_back(Nav_entry(func_name, pos, func_type, func_args));
                        classes.push_back(tmp);
                    }
                }
                else
                {
                    if (nav_sort > 0)
                    {
                        if (funs.size() > 0)
                        {
                            bool ins_done = false;
                            for (deque <Nav_entry>::iterator it = funs.begin(); it != funs.end(); it++)
                            {
                                if (nav_sort == 2 && it->name > func_name)
                                {
                                    funs.insert(it, Nav_entry(func_name, pos, func_type, func_args));
                                    ins_done = true;
                                    break;
                                }
                                else if (nav_sort == 1 && (func_type == "" || it->type > func_type))
                                {
                                    funs.insert(it, Nav_entry(func_name, pos, func_type, func_args));
                                    ins_done = true;
                                    break;
                                }

                            }
                            if (!ins_done) funs.push_back(Nav_entry(func_name, pos, func_type, func_args));
                        }
                        else
                            funs.push_back(Nav_entry(func_name, pos, func_type, func_args));
                    }
                    else
                        funs.push_back(Nav_entry(func_name, pos, func_type, func_args));
                }

            }

        }

    }

    string typefrm = "\t@C72";

    for (int i = 0; i < classes.size(); i++)
    {
        char line_nr[100];
        sprintf(line_nr, "%d", classes[i].pos);
        string dstr = "";

        if (!nav_expand)
        {
            if (classes[i].depth > 0) continue;
        }

        for (int d = 0; d < classes[i].depth; d++)
            dstr += "+-";
        if (dstr != "") dstr += "\t";

        navigator_browser->add((dstr + "@C136@iclass \t@b@C136" + classes[i].name).c_str(), (void*)strdup(line_nr));

        if (!nav_expand) continue;

        for (int j = 0; j < classes[i].children.size(); j++)
        {
            char line_nr[100];
            sprintf(line_nr, "%d", classes[i].children[j].pos);
            char *nn = strdup(("+-" + dstr + typefrm + classes[i].children[j].type + "\t@b" + classes[i].children[j].name + "\t(" + classes[i].children[j].args + ")").c_str());
            navigator_browser->add(nn, (void*)strdup(line_nr));
            //navigator_browser->add(nn,(void*)strdup(line_nr));
            free(nn);
        }
    }
    for (int i = 0; i < structs.size(); i++)
    {
        char line_nr[100];
        sprintf(line_nr, "%d", structs[i].pos);
        navigator_browser->add((("@C72@istruct \t@b@C72" + structs[i].name).c_str()), (void*)strdup(line_nr));
    }
    for (int i = 0; i < enums.size(); i++)
    {
        char line_nr[100];
        sprintf(line_nr, "%d", enums[i].pos);
        navigator_browser->add((("@C72@ienum \t@b@C72" + enums[i].name).c_str()), (void*)strdup(line_nr));
    }
    for (int i = 0; i < unions.size(); i++)
    {
        char line_nr[100];
        sprintf(line_nr, "%d", unions[i].pos);
        navigator_browser->add((("@C72@ienum \t@b@C72" + unions[i].name).c_str()), (void*)strdup(line_nr));
    }
    for (int i = 0; i < funs.size(); i++)
    {
        char line_nr[100];
        sprintf(line_nr, "%d", funs[i].pos);
        navigator_browser->add(((typefrm + funs[i].type + "\t@b" + funs[i].name + "\t(" + funs[i].args + ")").c_str()), (void*)strdup(line_nr));
    }

    navigator_browser->position(scroll_pos);
    //navigator_browser->redraw();
}

void nav_exp_cb(Fl_Widget *w, void *v) {
    nav_expand = !nav_expand;

    if (nav_expand) navigator_pop_btn->replace(navigator_pop_btn->value(), "Collapse Classes");
    else navigator_pop_btn->replace(navigator_pop_btn->value(), "Expand Classes");

    char *style = stylebuf->text_range(0, textbuf->length());
    char *text = textbuf->text_range(0, textbuf->length());
    navigator_update(text, style, textbuf->length());
    free(text);
    free(style);
}

void nav_norm_cb(Fl_Widget *w, void *v) {
    nav_sort = 0;

    navigator_update(textbuf->text_range(0, textbuf->length()), stylebuf->text_range(0, textbuf->length()), textbuf->length());

}


void nav_type_cb(Fl_Widget *w, void *v) {
    nav_sort = 1;

    navigator_update(textbuf->text_range(0, textbuf->length()), stylebuf->text_range(0, textbuf->length()), textbuf->length());

}

void nav_name_cb(Fl_Widget *w, void *v) {
    nav_sort = 2;

    navigator_update(textbuf->text_range(0, textbuf->length()), stylebuf->text_range(0, textbuf->length()), textbuf->length());

}


void debug_cb(Fl_Widget *w, void *v) {

    //first compile with -g flag
    project.gdbflags = "-g";
    generate_makefile_cb();
    make_(w, v, 1); //make clean
    make_(w, v, 0);
    project.gdbflags = "-s";
    generate_makefile_cb();

#if defined(MSDOS ) || defined(_WIN32)
    string command;
    //command = "runincmd.bat gdb "+project.binfilename+".exe";
    command = "gdb -q " + project.pr_dir + "/" + project.binfilename + ".exe";
    char *CurrWorkingDir = (char *)malloc(512);
#ifdef MSDOS
    toggle_text_mode();
    _getcwd(CurrWorkingDir, 512);
#else //WIN32
    //int err = get_app_path(CurrWorkingDir, PATH_MAX);
    _getcwd(CurrWorkingDir, 512);
#endif   

    //   printf("\n-> Please enter EXIT to return to FlDev <-\n\n"); 
    //   printf("%s\n",command.c_str());      
    system((const char*)command.c_str());

    chdir(CurrWorkingDir); //back
    free(CurrWorkingDir);
#ifdef MSDOS   
    toggle_text_mode();
#endif   
    return;
#endif //MSDOS - Win32

#ifdef USE_NXLIB
    fl_message("Not yet implemented");//problem with overlapping console and fltk windows.
    return;
#endif

    run_cb(NULL, (void*)1); //run gdb if Linux X11

#if WITHDEBUG
    //gdb_start(project.binfilename);
    //debug(project.binfilename);	
#else
    //fl_message("Not yet implemented");
#endif
}

void file_source_cb(Fl_Widget *w, void *v) {
    const char *val;
    val = fl_input((strmsg[7] + ":").c_str(), ".cpp");
    if (val != NULL)
    {
        if (!create_file(val)) return;
        string src_files = trim(project.src_files);
        src_files += " ";
        src_files += val;
        project.src_files = trim(src_files);
        //if(fl_ask(strmsg[6].c_str())) generate_makefile_cb();
        if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();
        project.addToBrowser(window->pr_browser);
        project.modified = true;
    }
}


void file_header_cb(Fl_Widget *w, void *v) {
    const char *val;
    val = fl_input((strmsg[7] + ":").c_str(), ".h");
    if (val != NULL)
    {
        if (!create_file(val)) return;
        string header_files = trim(project.header_files);
        header_files += " ";
        header_files += val;
        project.header_files = trim(header_files);
        //		if(fl_ask(strmsg[6].c_str())) generate_makefile_cb();
        if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();
        project.addToBrowser(window->pr_browser);
        project.modified = true;
    }
}

void file_form_cb(Fl_Widget *w, void *v) {
    const char *val;
    val = fl_input((strmsg[7] + ":").c_str(), ".fl");
    if (val != NULL)
    {
        if (!create_file(val)) return;
        string gui_files = trim(project.gui_files);
        gui_files += " ";
        gui_files += val;
        project.gui_files = trim(gui_files);
        //		if(fl_ask(strmsg[6].c_str())) generate_makefile_cb();		
        if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();
        project.addToBrowser(window->pr_browser);
        project.modified = true;
    }
}


void navigator_browser_cb(Fl_Widget *w, void *v) {
    //cout << navigator_browser->value() << endl;
    int but = Fl::event_button();
    //cout << but << endl;
    if (but == 3)
    {
        if (navigator_browser->value() > 0)
        {
            navigator_pop_btn->popup();
        }
    }
    else
    {
        if (Fl::event_clicks() > 0 || but == 2)
        {
            Fl::event_clicks(-1);



            //char *result = strtok(strdup(navigator_browser->text( navigator_browser->value() ))," ");
            //if(result != NULL) result = strtok(NULL," ");
            if (navigator_browser->value() > 0)
            {
                int pos = atoi((char *)(navigator_browser->data(navigator_browser->value())));
                //int pos = atoi(result);

                te->insert_position(textbuf->length());
                te->show_insert_position();


                te->insert_position(textbuf->line_start(pos));
                te->show_insert_position();

                //textbuf->select(pos,pos+1);
                textbuf->select(textbuf->line_start(pos), textbuf->line_end(pos));
                Fl::focus(te);

                //cout << pos << endl;
            }
            //textbuf
        }
    }

}

int timeout_mutex = 0;



void nav_up_timeout(void *v) {
    navigator_update(textbuf->text(), stylebuf->text(), textbuf->length());
    timeout_mutex = 0;
}

void add_nav_timeout_handler() {
    if (timeout_mutex) return;
    timeout_mutex = 1;
    Fl::add_timeout(5, nav_up_timeout);
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Editor window functions and class...



EditorWindow::EditorWindow(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
    replace_dlg = new Fl_Window(320, 105, "Replace");
    replace_find = new Fl_Input(80, 10, 230, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 230, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(5, 70, 95, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 80, 25, "Cancel");
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


void EditorWindow::show_linenrs() {
    if (line_nr_box->visible()) return;


    te->resize(gwn->x() + line_nr_size, te->y(), gwn->w() - line_nr_size, te->h());
    line_nr_box->resize(gwn->x(), te->y(), line_nr_size, te->h());
    gwn->add(line_nr_box);
    line_nr_box->show();

    redraw();
    Fl::check();
}


void EditorWindow::hide_linenrs() {
    if (!line_nr_box->visible()) return;

    gwn->remove(line_nr_box);
    te->resize(gwn->x() + 1, te->y(), gwn->w() - 1, te->h());
    //line_nr_box->resize(te->x()-line_nr_size,te->y(),line_nr_size,te->h());
    line_nr_box->hide();
    redraw();
    Fl::check();
}

//shows the compiler-output-widget at the bottom
void EditorWindow::show_output() {
    if (output_grp->visible()) return;

    Fl_Group *o = output_grp;
    int he = o->h();

    if (he > h() - 100) he = h() / 2;

    tile->size(tile->w(), tile->h() - he);	//edited
    //tabs->size(tabs->w(),h()-167); 	//edited
    o->resize(0, tile->y() + tile->h() + 1, w(), he);
    all_but_menu_grp->add(o);
    //output->resize(output_grp->x(),output_grp->y()+4,output_grp->w(),output_grp->h()-6);
    o->show();
    Fl::check();
}

//hides the compiler-output-widget at the bottom
void EditorWindow::hide_output() {
    if (!output_grp->visible()) return;
    //Fl_Text_Editor *o = output;
    Fl_Group *o = output_grp;
    all_but_menu_grp->remove(o);

    //o->resize(0,tile->y() + tile->h()+1 ,w(),0);
    o->hide();

    tile->size(tile->w(), h() - 66);	//edited
    //tabs->size(tabs->w(),h()-66);	//edited
    //
}


//shows the project-/file-browser at the left
void EditorWindow::show_browser() {
    tabs->size(150, editor->h());
    gwn->resize(tabs->w(), gwn->y(), gwn->w() - tabs->w(), gwn->h());
    tabs->show();
    gwn->redraw();
    Fl::check();
}

//hides the project-/file-browser at the left
void EditorWindow::hide_browser() {
    gwn->resize(0, gwn->y(), w(), gwn->h());
    tabs->hide();
}





class DragButton : public Fl_Button {
public:
    DragButton(int x, int y, int w, int h) : Fl_Button(x, y, w, h) {}

    int handle(int event) {
        switch (event)
        {
            case FL_ENTER: fl_cursor(FL_CURSOR_NS); return 1;
            case FL_LEAVE: fl_cursor(FL_CURSOR_DEFAULT); return 1;
            case FL_DRAG:
            {
                int x = Fl::event_x();
                int y = Fl::event_y();

                if (y > output_grp->parent()->parent()->h() - 40 || y < 140) return 1;
                tile->size(tile->w(), y - tile->y() - 2);

                int oldh = output_grp->y() + output_grp->h();
                int newh = oldh - (tile->y() + tile->h() + 1);

                output_grp->resize(0, tile->y() + tile->h() + 1, output_grp->w(), newh);

                //Fl::check();
                //window->redraw();
                output_grp->parent()->parent()->redraw();
                //tile->redraw();
                return 1;
            }
        }
        return Fl_Button::handle(event);
    }
};





int check_save(void) {
    if (!bufchanged) return 1;

    int r = fl_choice(strmsg[1].c_str(),
                      cancel_str.c_str(), save_str.c_str(), discard_str.c_str());

    if (r == 1)
    {
        save_cb(); // Save the file...
        return !bufchanged;
    }

    return (r == 2) ? 1 : 0;
}



int check_project_save(void) {
    if (!project.modified) return 1;

    int r = fl_choice(strmsg[2].c_str(),
                      cancel_str.c_str(), save_str.c_str(), discard_str.c_str());

    if (r == 1)
    {
        project.save(); // Save the file...
        return !project.modified;
    }

    return (r == 2) ? 1 : 0;
}


void backupfile(const char *newfile) {
    char buf[1000];
    char *cpfile = strdup(newfile);
    char *tb = (char *)fl_filename_name(cpfile);
    char *filename = strdup(tb);
    //if(tb!=newfile) 
    tb[0] = '\0';
    sprintf(buf, "cp %s%s %s.%s~ &", tb, filename, tb, filename);
    string bf = string(tb) + "." + filename + "~";
    char absbuf[1000];
    fl_filename_absolute(absbuf, 1000, bf.c_str());
    bakfiles.push_back(string(absbuf));
    FILE *ptr = _popen(buf, "r");
    if (ptr)
        _pclose(ptr);
    free(filename);
    free(cpfile);
}


void removebackupfiles() {
    char buf[1000];
    for (int i = 0; i < bakfiles.size(); i++)
    {
        sprintf(buf, "rm -rf %s &", bakfiles[i].c_str());
        FILE *ptr = _popen(buf, "r");
        if (ptr)
            _pclose(ptr);
        cout << "Removed " << bakfiles[i] << endl;
    }
}

int  load_file(const char *newfile, int ipos) {
    loading = 1;
    cppfile = false;

    if (strncmp(newfile + strlen(newfile) - 4, ".cpp", 4) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 2, ".c", 2) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 2, ".h", 2) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 2, ".H", 2) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 4, ".hpp", 4) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 4, ".cxx", 4) == 0) cppfile = true;
    if (strncmp(newfile + strlen(newfile) - 4, ".hxx", 4) == 0) cppfile = true;

#ifdef MSDOS
    if ((strncmp(newfile + strlen(newfile) - 4, ".fld", 4) == 0) || \
        (strncmp(newfile + strlen(newfile) - 6, ".fldev", 4) == 0))
    {
    #else
    if (strncmp(newfile + strlen(newfile) - 6, ".fldev", 4) == 0)
    {
    #endif
        project.pr_filename = newfile;
        if (!project.load()) return 0;
        project.addToBrowser(window->pr_browser);
        window->file_browser->load(project.pr_dir.c_str());
        add_recent_project_to_menu(newfile);
        return 2;
    }


    int insert = (ipos != -1);
    bufchanged = insert;
    char *tmp = strdup(filename);
    if (!insert) strcpy(filename, "");
    int r;

    if (!insert)
    {
        if (tmp[0] != '\0') file_hist->add(tmp, te->insert_position());

        if (backup_file) backupfile(newfile);

        navigator_browser->clear();
        r = textbuf->loadfile(newfile);
        char *style = stylebuf->text_range(0, textbuf->length());
        char *text = textbuf->text_range(0, textbuf->length());
        navigator_update(text, style, textbuf->length());
        free(text);
        free(style);
    }
    else r = textbuf->insertfile(newfile, ipos);

    free(tmp);
    if (r)
        fl_alert("%s \'%s\':\n%s.", strmsg[11].c_str(), newfile, strerror(errno));
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


void save_file(const char *newfile) {
    if (textbuf->savefile(newfile))
        fl_alert("%s \'%s\':\n%s.", strmsg[12].c_str(), newfile, strerror(errno));
    else
        strcpy(filename, newfile);
    bufchanged = 0;
    textbuf->call_modify_callbacks();
    add_recent_file_to_menu(filename);
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


void run_cb(Fl_Widget* w, void* v) {
    FILE *ptr;
    string buf;
    string command;
    int mode;
    mode = (int)v;
    if (exec_running)
    {
        fl_alert(strmsg[13].c_str());
        return;
    }
    exec_running = true;

#if defined(MSDOS ) || defined(_WIN32)
    command = "runincmd.bat " + project.binfilename + ".exe";
#else //define command for Linux
    if (mode == 1)
    {
        command = "gdb -q " + project.Bin + "/" + project.binfilename;
    }
    else
    {
        command = project.Bin + "/" + project.binfilename;
    }
#endif

    if (command == "") (strmsg[14].c_str());

#if defined(MSDOS ) || defined(_WIN32) //nested #ifs follow!
    char *CurrWorkingDir = (char *)malloc(512);
#ifdef MSDOS
    toggle_text_mode();
    _getcwd(CurrWorkingDir, 512);
#else //WIN32
    int err = get_app_path(CurrWorkingDir, PATH_MAX);
#endif   

#if MSDOS
    system((const char*)command.c_str());
#else //WIN32
    if (console_check->value() || mode == 1)
    {	//console or gdb mode
        system((const char*)command.c_str());
    }
    else
    {
        command = project.binfilename + ".exe";
        wprocess((char*)command.c_str(), 0);
    }
#endif

    chdir(CurrWorkingDir);
    free(CurrWorkingDir);
#if MSDOS
    toggle_text_mode();
#endif
    exec_running = false;
    return; //quit here for MSDOS or WIN32
#endif //MSDOS + Win32

//now for Linux - first run in console mode
    if (console_check->value() || mode == 1)
    {
    #ifdef USE_NXLIB
        buf = "/usr/bin/deskapps/nxterm/nxterm " + command;
    #else
        ptr = fopen("fldevrun.sh", "w");
        fprintf(ptr, "#!/bin/sh\n%s\necho 'Press Enter to continue...'\nread", command.c_str());
        fclose(ptr); //add chmod +x here!
        buf = "xterm -e sh fldevrun.sh &";
    #endif
        ptr = _popen(buf.c_str(), "r");
        _pclose(ptr);

    }
    else
    { //if not in console mode

        buf = command + " > /dev/null 2>&1 &";
        ptr = _popen(buf.c_str(), "r");
        _pclose(ptr);
    }
    exec_running = false;
}



void xterm_cb(Fl_Widget* w, void* v) {
    FILE *ptr;

#ifdef _WIN32
    char *CurrWorkingDir = (char *)malloc(512);
    //int err = get_app_path(CurrWorkingDir, PATH_MAX);
    _getcwd(CurrWorkingDir, 512);
    chdir(project.pr_dir.c_str());
    //printf("\n-> Please enter EXIT to return to FlDev <-\n\n"); 
    system("command.com");

    chdir(CurrWorkingDir);
    free(CurrWorkingDir);
    return; //avoid pclose
#endif

#ifdef USE_NXLIB
#ifdef MSDOS
    toggle_text_mode();
    char *CurrWorkingDir = (char *)malloc(512);
    _getcwd(CurrWorkingDir, 512);

    printf("\n-> Please enter EXIT to return to FlDev <-\n\n");
    system("command.com");

    chdir(CurrWorkingDir);
    free(CurrWorkingDir);
    toggle_text_mode();
    return; //avoid pclose
#else
    ptr = _popen("/usr/bin/deskapps/nxterm/nxterm &", "r");
#endif
#else
    ptr = _popen("xterm &", "r");
#endif
    _pclose(ptr);
}


void fluid_cb(Fl_Widget* w, void* v) {
    FILE *ptr;
#ifdef MSDOS
    system("fluid");
#elif _WIN32
    char *projectdir;
    char homedir[PATH_MAX];
    int err = get_app_path(homedir, PATH_MAX);
    projectdir = (char*)malloc((strlen(homedir) + strlen("//fluid") + 1)*sizeof(char));
    sprintf(projectdir, "%s\\fluid", homedir);
    wprocess(projectdir, 0);
    free(projectdir);
#else  
    ptr = _popen("fluid &", "r");
    _pclose(ptr);
#endif

}


void find_cb(Fl_Widget* w, void* v) {
    EditorWindow* e = window;
    const char *val;
    val = fl_input((strmsg[8] + ":").c_str(), e->search);
    if (val != NULL)
    {
// User entered a string - go find it!
        strcpy(e->search, val);
        find2_cb(w, v);
    }
}


void find2_cb(Fl_Widget* w, void* v) {
    EditorWindow* e = (EditorWindow*)v;
    //  EditorWindow* e = window;

    if (e->search[0] == '\0')
    {
// Search string is blank; get a new one...
        find_cb(w, v);
        return;
    }

    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, e->search, &pos);
    if (found)
    {
// Found a match; select and update the position...
        textbuf->select(pos, pos + strlen(e->search));
        e->editor->insert_position(pos + strlen(e->search));
        e->editor->show_insert_position();
    }
    else fl_alert(strmsg[15].c_str(), e->search);
}


void set_title(Fl_Window* w) {
    char temp_title[511];
    if (filename[0] == '\0') strcpy(temp_title, "Untitled");
    else
    {
        char *slash;
        slash = strrchr(filename, '/');
    #ifdef WIN32
        if (slash == NULL) slash = strrchr(filename, '\\');
    #endif
        if (slash != NULL) strcpy(temp_title, slash + 1);
        else strcpy(temp_title, filename);
    }

    if (bufchanged)
    {
        strcat(temp_title, " (");
        strcat(temp_title, mod_str.c_str());
        strcat(temp_title, ")");
    }
    if (!project.assigned) sprintf(title, "FLDev %s    %s", VERSION, temp_title);
    else sprintf(title, "FLDev %s    %s: %s  %s: %s", VERSION, window->tabs->child(0)->label(), project.name.c_str(), file_str.c_str(), temp_title);
    sprintf(filename_wo_path, "%s", temp_title);
    ((EditorWindow *)w)->label(title);
}




void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v) {
    EditorWindow *w = (EditorWindow *)v;
    if ((nInserted || nDeleted) && !loading)
    {
        bufchanged = 1;
        if (auto_hide) w->hide_output();
        set_title(w);
    }
    if (loading)
    {
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
    navigator_browser->clear();
}





void open_cb(Fl_Widget*, void*) {
    if (!check_save())
        return;
    Fl_Native_File_Chooser fnfc;
    fnfc.title(strmsg[3].c_str());
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    if (fnfc.show())
        return;

    const char *newfile = fnfc.filename();
    char path[511];

    if (newfile == "")
        return;

    if (newfile != NULL)
    {
        strcpy(path, newfile);
        char *slash;
        slash = strrchr(path, '/');
        *slash = '\0';
        load_file(newfile, -1);
        add_recent_file_to_menu(filename);
    }
}




void insert_cb(Fl_Widget*, void *v) {
    Fl_Native_File_Chooser fnfc;
    fnfc.title(strmsg[4].c_str());
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    if (fnfc.show()) return;
    const char *newfile = fnfc.filename();
    if (newfile != NULL)
    {
        EditorWindow *w = (EditorWindow *)v;
        load_file(newfile, w->editor->insert_position());
    }
}

/*
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
  FILE *ptr = _popen("rm -f fldevrun.sh","r");
  _pclose(ptr);

  exit(0);
}*/

void quit_cb(Fl_Widget*, void*) {
    if (Fl::event_key() == FL_Escape)
        return;
    if (bufchanged && !check_save())
        return;
    if (!check_project_save())
        return;
    save_config_file();
    FILE *ptr = _popen("rm -f fldevrun.sh", "r");
    _pclose(ptr);
    if (delbak) removebackupfiles();
#ifdef MSDOS
    GrClose();
#endif
    exit(0);
}

void replace_cb(Fl_Widget*, void* v) {
    EditorWindow* e = (EditorWindow*)v;
    e->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v) {
    EditorWindow* e = window;
    const char *find = e->replace_find->value();
    const char *replace = e->replace_with->value();

    if (find[0] == '\0')
    {
// Search string is blank; get a new one...
        e->replace_dlg->show();
        return;
    }

    e->replace_dlg->hide();

    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, find, &pos);

    if (found)
    {
// Found a match; update the position and replace text...
        textbuf->select(pos, pos + strlen(find));
        textbuf->remove_selection();
        textbuf->insert(pos, replace);
        textbuf->select(pos, pos + strlen(replace));
        e->editor->insert_position(pos + strlen(replace));
        e->editor->show_insert_position();
    }
    else fl_alert(strmsg[15].c_str(), find);
}

void replall_cb(Fl_Widget*, void* v) {
    EditorWindow* e = (EditorWindow*)v;
    const char *find = e->replace_find->value();
    const char *replace = e->replace_with->value();

    find = e->replace_find->value();
    if (find[0] == '\0')
    {
// Search string is blank; get a new one...
        e->replace_dlg->show();
        return;
    }

    e->replace_dlg->hide();

    e->editor->insert_position(0);
    int times = 0;

    // Loop through the whole string
    for (int found = 1; found;)
    {
        int pos = e->editor->insert_position();
        found = textbuf->search_forward(pos, find, &pos);

        if (found)
        {
// Found a match; update the position and replace text...
            textbuf->select(pos, pos + strlen(find));
            textbuf->remove_selection();
            textbuf->insert(pos, replace);
            e->editor->insert_position(pos + strlen(replace));
            e->editor->show_insert_position();
            times++;
        }
    }

    if (times) fl_message(strmsg[9].c_str(), times);
    else fl_alert(strmsg[15].c_str(), find);
}

void replcan_cb(Fl_Widget*, void* v) {
    EditorWindow* e = window;
    e->replace_dlg->hide();
}

void save_cb() {
    if (filename[0] == '\0')
    {
// No filename - get one!
        saveas_cb();
        return;
    }
    else save_file(filename);
    set_title(window);
}

void saveas_cb() {
    Fl_Native_File_Chooser fnfc;
    fnfc.title(strmsg[7].c_str());
    fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if (fnfc.show())
        return;

    const char *newfile = fnfc.filename();
    if (newfile != NULL)
    {
        save_file(newfile);
        set_title(window);
    }
}



void make_(Fl_Widget* w, void* v, int mode) {
    EditorWindow* e = window;
    FILE *ptr;
    Fl_Text_Editor *o = e->output;
    char buf[255];
    int BUFSIZE = 255;
    make_error = false;
    if (!stylebuf) stylebuf = new Fl_Text_Buffer();


    string outp, erroutp;
    if (bufchanged) save_cb();
    outputtb->remove(0, outputtb->length());
    e->show_output();
    e->redraw();

#ifdef MSDOS
    if (mode == 2) //compile
    {
        char *fname = strdup(filename_wo_path);
        char *name_wo_ext = fl_filename_setext(fname, ".o");
        sprintf(buf, "redir -e errorlog.mak -o makelog.mak make %s", name_wo_ext);
        if (fname) free(fname);
        //cout << buf << endl;
    }
    //make clean
    else if (mode == 1) sprintf(buf, "redir -e errorlog.mak -o makelog.mak make clean");
    //make
    else if (mode == 0) sprintf(buf, "redir -e errorlog.mak -o makelog.mak make");

    else return;

    system(buf); //output to errorlog.make, read into output window below as errors

    ptr = fopen("makelog.mak", "r"); //will be read below

#elif _WIN32
    //ptr = _popen("rm errorlog.make 2> /dev/null","r"); 
    //_pclose(ptr);

    if (mode == 2) //compile
    {
        char *fname = strdup(filename_wo_path);
        char *name_wo_ext = fl_filename_setext(fname, ".o");
        sprintf(buf, "%s/domake.bat %s", project.pr_dir.c_str(), name_wo_ext); //"mingw32-make %s 2>errorlog.make >makelog.make",name_wo_ext);
        if (fname) free(fname);
        //cout << buf << endl;
    }
    //make clean
    else if (mode == 1) sprintf(buf, "%s/domake.bat clean", project.pr_dir.c_str()); //"mingw32-make clean 2>errorlog.make >makelog.make"); 
    //make
    else if (mode == 0) sprintf(buf, "%s/domake.bat", project.pr_dir.c_str()); //"""mingw32-make 2>errorlog.make >makelog.make");

    else return;
    wprocess((char*)buf, 1);
    //system(buf);

    sprintf(buf, "%s/makelog.make", project.pr_dir.c_str());
    ptr = fopen(buf, "r"); //will be read below
    //ptr = fopen("makelog.make","r"); //will be read below
    //ptr = _popen(buf,"r");

#else  //Linux
    ptr = _popen("rm errorlog.make 2> /dev/null", "r");
    _pclose(ptr);

    if (mode == 2) //compile
    {
        char *fname = strdup(filename_wo_path);
        char *name_wo_ext = fl_filename_setext(fname, ".o");
        sprintf(buf, "make %s 2> errorlog.make", name_wo_ext);
        if (fname) free(fname);
        //cout << buf << endl;
    }
    //make clean
    else if (mode == 1) sprintf(buf, "make clean 2> errorlog.make &");
    //make
    else if (mode == 0) sprintf(buf, "make 2> errorlog.make &");

    else return;

    ptr = _popen(buf, "r");
#endif //MSDOS, _Win32, Linux

    op_stylebuf->remove(0, op_stylebuf->length());

    op_stylebuf->append("AAAAA");
    outputtb->append("make:\n");

    int count = 0;
    while (fgets(buf, BUFSIZE, ptr) != NULL)
    {
        for (int i = 0; i < strlen(buf); i++)
            op_stylebuf->append("A");
        outputtb->append(buf);
        Fl::check();
        count++;
    }
#if defined(MSDOS ) || defined(_WIN32)
    fclose(ptr);
#else  
    _pclose(ptr);
#endif
    outp = "";
    //#endif //MSDOS

#ifdef MSDOS
    ptr = fopen("errorlog.mak", "r");
#elif _WIN32
    sprintf(buf, "%s/errorlog.make", project.pr_dir.c_str());
    ptr = fopen(buf, "r");
#else  
    ptr = fopen("errorlog.make", "r");
#endif
    erroutp = "";
    while (fgets(buf, BUFSIZE, ptr) != NULL)
    {
        erroutp += buf;
        for (int i = 0; i < strlen(buf); i++)
            op_stylebuf->append("B");
        Fl::check();
        count++;
    }

    if (erroutp == "")
    {
        erroutp += "make finished.\n";
        op_stylebuf->append("AAAAAAAAAAAAAAA");
    }
    else
    {
        erroutp += "make finished with errors.\n";
        op_stylebuf->append("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        make_error = true;
    }
    outputtb->append((outp + erroutp).c_str());
    o->insert_position(outputtb->length());
    o->show_insert_position();
#if defined(MSDOS ) || defined(_WIN32)
    fclose(ptr);
#else  
    fclose(ptr);
#endif  
    linecount = count + 2;

}

void make_cb(Fl_Widget* w, void* v) {
    make_(w, v, 0);
}

void compile_cb(Fl_Widget* w, void* v) {
    make_(w, v, 2);
}

void make_clean_cb(Fl_Widget* w, void* v) {
    make_(w, v, 1);
}

void make_run_cb(Fl_Widget* w, void* v) {
    make_(w, v, 0);
    if (!make_error) run_cb(w, v);
}



void theme_none_cb() {
    Fl::scheme("none");
}

void theme_plastic_cb() {
    Fl::scheme("plastic");
}

void theme_gtk_cb() {
    Fl::scheme("gtk+");
}

void theme_gleam_cb() {
    Fl::scheme("gleam");
}



void mcomment_cb() {
    int start, end;
    te->buffer()->selection_position(&start, &end);
    te->buffer()->insert(end, "*/");
    te->buffer()->insert(start, "/*");
}

void mstring_cb() {
    int start, end;
    te->buffer()->selection_position(&start, &end);
    te->buffer()->insert(end, "\"");
    te->buffer()->insert(start, "\"");
}

void mindent_cb() {
    int firstlinepos, actuallinepos;
    te->buffer()->selection_position(&firstlinepos, &actuallinepos);
    actuallinepos = te->buffer()->line_start(actuallinepos);
    firstlinepos = te->buffer()->line_start(firstlinepos);
    while (actuallinepos >= firstlinepos)
    {
        te->buffer()->insert(actuallinepos, "\t");
        actuallinepos = te->buffer()->line_start(actuallinepos - 1);
    }
}

void munindent_cb() {
    int firstlinepos, actuallinepos;
    te->buffer()->selection_position(&firstlinepos, &actuallinepos);
    actuallinepos = te->buffer()->line_start(actuallinepos);
    firstlinepos = te->buffer()->line_start(firstlinepos);
    while (actuallinepos >= firstlinepos)
    {
        if (strcmp(te->buffer()->text_range(actuallinepos, actuallinepos + 1), " ") == 0 ||
            strcmp(te->buffer()->text_range(actuallinepos, actuallinepos + 1), "\t") == 0)
            te->buffer()->remove(actuallinepos, actuallinepos + 1);
        actuallinepos = te->buffer()->line_start(actuallinepos - 1);
    }
}


void about_cb() {
    fl_message("FLDev IDE\nVersion %s\n\nPorted to FLTK 1.3.0 in 2014 by Georg Potthast\n\nbased on FLDev 0.5.7 which is\n\nCopyright (C) 2005 by Philipp Pracht\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n", VERSION);
}





void snipp_gpl_cb(Fl_Widget *w, void *v) {
    te->buffer()->insert(te->insert_position(), "/*\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n*/");

}

void snipp_header_cb(Fl_Widget *w, void *v) {
    te->buffer()->insert(te->insert_position(), "\n#ifndef \n#define\n\n\n#endif\n");

}

void snipp_if_cb(Fl_Widget *w, void *v) {
    int ip = te->insert_position();
    int post_ip = ip + 3;

    char *line;
    line = strdup(te->buffer()->line_text(ip));
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ' && line[i] != '\t') line[i] = '\0';
    }

    string ip_str = "if()\n" + string(line) + "{\n" + line + "\t\n" + line + "}\n";
    te->buffer()->insert(ip, ip_str.c_str());

    te->insert_position(post_ip);
    free(line);
}

void snipp_while_cb(Fl_Widget *w, void *v) {
    int ip = te->insert_position();
    int post_ip = ip + 6;

    char *line;
    line = strdup(te->buffer()->line_text(ip));
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ' && line[i] != '\t') line[i] = '\0';
    }

    string ip_str = "while()\n" + string(line) + "{\n" + line + "\t\n" + line + "}\n";
    te->buffer()->insert(ip, ip_str.c_str());

    te->insert_position(post_ip);
    free(line);
}

void snipp_for_cb(Fl_Widget *w, void *v) {
    int ip = te->insert_position();
    int post_ip = ip + 4;

    char *line;
    line = strdup(te->buffer()->line_text(ip));
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ' && line[i] != '\t') line[i] = '\0';
    }

    string ip_str = "for(;;)\n" + string(line) + "{\n" + line + "\t\n" + line + "}\n";
    te->buffer()->insert(ip, ip_str.c_str());

    te->insert_position(post_ip);
    free(line);
}

void snipp_class_cb(Fl_Widget *w, void *v) {
    int ip = te->insert_position();
    int post_ip = ip + 6;

    char *line;
    line = strdup(te->buffer()->line_text(ip));
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ' && line[i] != '\t') line[i] = '\0';
    }

    string ip_str = "class  \n" + string(line) + "{\n" + line + "  public:\n" + line + "\t\n" + line + "};\n";
    te->buffer()->insert(ip, ip_str.c_str());

    te->insert_position(post_ip);
    free(line);
}





Fl_Menu_Item menuitems[] = {
  { "&File",            0, 0, 0, FL_SUBMENU }, // 0
    { "&New File",        0, (Fl_Callback *)new_cb }, // 1
    { "&Open File...",    FL_COMMAND + 'o', (Fl_Callback *)open_cb },
    { "Open Recent File", 0, 0, 0, FL_SUBMENU },
      { "" }, // #define FL_MENU_RECENT_FILES 4
      { "" },
      { "" },
      { "" },
      { 0 },
    { "&Insert File...",  FL_COMMAND + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
    { "&Save File",       FL_COMMAND + 's', (Fl_Callback *)save_cb }, // 10
    { "Save File &As...", FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
    { "E&xit",            FL_COMMAND + 'q', (Fl_Callback *)quit_cb, 0 },
    { 0 },

  { "&Edit", 0, 0, 0, FL_SUBMENU },
    { "&Undo",            FL_COMMAND + 'z', (Fl_Callback *)undo_cb, 0, FL_MENU_DIVIDER },
    { "Cu&t",             FL_COMMAND + 'x', (Fl_Callback *)cut_cb },
    { "&Copy",            FL_COMMAND + 'c', (Fl_Callback *)copy_cb },
    { "&Paste",           FL_COMMAND + 'v', (Fl_Callback *)paste_cb },
    { "&Delete",          0, (Fl_Callback *)delete_cb },
    { "Delete",           0, (Fl_Callback *)delete_cb, 0, FL_MENU_DIVIDER }, // 20
    { "C&omment out",     FL_ALT + 'c', (Fl_Callback *)mcomment_cb },
    { "&Stringize",       FL_ALT + 's', (Fl_Callback *)mstring_cb },
    { "&Indent Selection", FL_COMMAND + FL_Tab, (Fl_Callback *)mindent_cb },
    { "U&nindent Selection", FL_COMMAND + FL_SHIFT + FL_Tab, (Fl_Callback *)munindent_cb , 0, FL_MENU_DIVIDER},
    { "Pre&ferences...",  0, (Fl_Callback *)pref_cb },
    { 0 },

  { "&View", 0, 0, 0, FL_SUBMENU },
    { "Show Filebrowser", 0, (Fl_Callback *)show_browser_cb, 0, FL_MENU_TOGGLE },
    { "Show Navigator",   0, (Fl_Callback *)show_nav_cb, 0, FL_MENU_TOGGLE },
    { "Show Linenumbers", 0, (Fl_Callback *)linenumbers_cb, 0, FL_MENU_TOGGLE }, // 30
    { "Theme",            0, 0, 0, FL_SUBMENU },
      { "None",           0, (Fl_Callback *)theme_none_cb },
      { "Plastic",        0, (Fl_Callback *)theme_plastic_cb },
      { "GTK+",           0, (Fl_Callback *)theme_gtk_cb },
      { "Gleam",          0, (Fl_Callback *)theme_gleam_cb },
      { 0 },
    { 0 },

  { "&Search", 0, 0, 0, FL_SUBMENU },
    { "&Find...",         FL_COMMAND + 'f', (Fl_Callback *)find_cb },
    { "F&ind Again",      FL_COMMAND + 'g', find2_cb }, // 40
    { "&Replace...",      FL_COMMAND + 'r', replace_cb },
    { "Re&place Again",   FL_COMMAND + 't', replace2_cb },
    { 0 },

  { "Project", 0, 0, 0, FL_SUBMENU },
    { "&New Project",     FL_COMMAND + FL_SHIFT + 'n', (Fl_Callback *)new_pr_cb},
    { "Open &Project",    FL_COMMAND + FL_SHIFT + 'p', (Fl_Callback *)open_pr_cb },
    { "Previous Project", 0, 0, 0, FL_SUBMENU },
      { "" }, // #define FL_MENU_RECENT_PROJECTS 48
      { "" },
      { "" },
      { "" },
      { 0 },
    { "&Save Project",    FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)save_pr_cb, 0, FL_MENU_DIVIDER },
    { "&Make",            FL_ALT + 'm', (Fl_Callback *)make_cb },
    { "&Run",             FL_ALT + 'r', (Fl_Callback *)run_cb },
    { "&Compile",         FL_ALT + 'C', (Fl_Callback *)compile_cb },
    { "Make &and Run",    FL_ALT + 'a', (Fl_Callback *)make_run_cb },
    { "&Debug",           FL_ALT + 'd', (Fl_Callback *)debug_cb },
    { "Make Clean",       0, (Fl_Callback *)make_clean_cb , 0, FL_MENU_DIVIDER},
    { "Generate Makefile", 0, (Fl_Callback *)generate_makefile_cb , 0, FL_MENU_DIVIDER},
    { "Options...",       FL_ALT + 'p', (Fl_Callback *)pr_opt_cb },
    { 0 },

  { "&Tools", 0, 0, 0, FL_SUBMENU },
    { "&Fluid",       FL_ALT + 'f', (Fl_Callback *)fluid_cb },
    { "&XTerm",       FL_ALT + 't', (Fl_Callback *)xterm_cb },
    { 0 },

  { "E&xtras", 0, 0, 0, FL_SUBMENU },
    { "&Code Snippets",   0, 0, 0, FL_SUBMENU},
      { "GPL", 	FL_COMMAND + FL_ALT + 'g',	snipp_gpl_cb },
      { "Header", FL_COMMAND + FL_ALT + 'h',	snipp_header_cb},
      { "class",	FL_COMMAND + FL_ALT + 'c',	snipp_class_cb },
      { "if",		FL_COMMAND + FL_ALT + 'i',	snipp_if_cb },
      { "while",	FL_COMMAND + FL_ALT + 'w',	snipp_while_cb },
      { "for",	FL_COMMAND + FL_ALT + 'f',	snipp_for_cb },
      { 0 },
    { 0 },

  { "&Help", 0, 0, 0, FL_SUBMENU },
    //{ "Fltk &Manual",       FL_CTRL + FL_SHIFT + FL_ALT + 'M', (Fl_Callback *)manual_cb },
    { "&C / C++ Reference", FL_COMMAND + FL_SHIFT + FL_ALT + 'C', (Fl_Callback *)ref_cb },
    { "About...",       0, (Fl_Callback *)about_cb },
    { 0 },

  { 0 }
};












void owbt_callback(Fl_Widget*, void*) {
    if (output_grp == NULL) return;
    if (!output_grp->visible()) window->show_output();
    else window->hide_output();
    window->redraw();
    Fl::check();
    output_grp->redraw();
}


void pr_opt_cb(Fl_Widget*, void*) {
    project_options_window->show();
    pr_changed = 0;
    while (project_options_window->visible()) { Fl::wait(5); }
    if (pr_changed > 0)
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
        project.gui_files = pgui->value();

        //		if(pr_changed == 1) if(fl_ask(strmsg[6].c_str())) generate_makefile_cb();
        if (pr_changed == 1) if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();
        project.addToBrowser(window->pr_browser);
        project.modified = true;
    }
}





void open_pr_cb() {
    if (!check_save())
        return;
    if (!check_project_save())
        return;
    const char *projectdir;
    const char *homedir = getenv("HOME");

    Fl_Native_File_Chooser fnfc;
    fnfc.title(strmsg[5].c_str());

#ifdef MSDOS
    fnfc.filter("FLDev Project Files\t*.fld");
#elif _WIN32
    fnfc.filter("FLDev Project Files\t*.fldev");
#else
    projectdir = (char*)malloc((strlen(homedir) + strlen("/fldev/") + 1)*sizeof(char));
    if (homedir != NULL) sprintf(projectdir, "%s/fldev/", homedir);
    fnfc.filter("FLDev Project Files\t*.fldev");
    fnfc.directory(projectdir);
    free(projectdir);
#endif

    if (fnfc.show())
        return;

    const char *newfile = fnfc.filename();
    if (newfile == NULL) return;

    project.pr_filename = newfile;
    project.load();

    char *dir = strdup(newfile);
    char *dirname = strrchr(dir, '/');
    if (dirname != NULL) *dirname = '\0';

    if (fl_filename_isdir(dir))
    {
        chdir(dir);
    }
    project.addToBrowser(window->pr_browser);
    window->fb_label->value(strdup((project.pr_dir + string("/")).c_str()));
    window->file_browser->load(project.pr_dir.c_str());
    //window->file_browser->reload();


    for (int i = 0; i < 10; i++)
        menuitems[rec_pr_menu_index + i].activate();
    add_recent_project_to_menu(newfile);
    menubar->copy(menuitems, window);

    if ((char *)window->pr_browser->text(1) != NULL)
    {
        load_file((char *)window->pr_browser->text(1), -1);
        if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();
    }

    free(dir);
}



void save_pr_cb() {
    project.save();
}


void new_pr_cb() {

    char *tmpbuf;

    if (!check_save()) return;
    if (!check_project_save()) return;
    pr_wiz_ok = false;
    proj_wiz->show();

    while (proj_wiz->visible())
    {
        Fl::wait(5);
    }
    if (!pr_wiz_ok) return;

    project.name = trim(pr_name_input->value());
    project.pr_dir = trim(pr_dir_input->value());
    project.own_makefile = trim(pr_own_makefile->value());
    project.use_own_makefile = 0;
    if (own_make_chk->value()) project.use_own_makefile = 1;

    tmpbuf = strdup(project.pr_dir.c_str());
    if (tmpbuf[strlen(tmpbuf) - 1] == '/') tmpbuf[strlen(tmpbuf) - 1] = '\0';
    project.pr_dir = tmpbuf;
    free(tmpbuf);
#ifdef MSDOS
    project.pr_filename = project.pr_dir + "/" + project.name + ".fld";
#else  
    project.pr_filename = project.pr_dir + "/" + project.name + ".fldev";
#endif
    tmpbuf = strdup(project.name.c_str());
    for (int i = 0; i < strlen(tmpbuf); i++)
    {
        if (isupper(*(tmpbuf + i))) *(tmpbuf + i) = tolower(*(tmpbuf + i));
    }

    project.binfilename = tmpbuf;
#ifdef MSDOS
    project.oDir = ".";
    project.Bin = ".";
#else
    char *projectdir;
#ifdef _WIN32
    char homedir[PATH_MAX];
    int err = get_app_path(homedir, PATH_MAX);
    projectdir = (char*)malloc((strlen(homedir) + strlen("1234567") + 1)*sizeof(char));
    if (homedir != NULL) sprintf(projectdir, "%s", homedir);
#else  
    char *homedir = getenv("HOME");
    projectdir = (char*)malloc((strlen(homedir) + strlen("/fldev/") + 1)*sizeof(char));
    if (homedir != NULL) sprintf(projectdir, "%s/fldev/", homedir);
#endif
    project.oDir = project.pr_dir; //projectdir;
    project.Bin = project.pr_dir; //projectdir;
#endif
    if (x_app->value())
    {
    #ifdef MSDOS
        project.libs = "-lfltk_images -lfltk_forms -lfltk -lpng -lz -ljpeg -lNX11 -lnano-X -lfreetype -lm";
        project.libdirs = "-L/djgpp/lib";
        project.incdirs = "-I/djgpp/include";
    #else
    #if USE_NXLIB
        project.libs = "-lNX11 -lnano-X -lfltk -lfltk_images -lfltk_forms -lfltk_png -lfltk_z -lfltk_jpeg -lfreetype -lm";
        project.libdirs = "-L/usr/local/lib";
        project.incdirs = "-I/usr/include";
    #elif _WIN32
        char tmpbuffer[512];
        project.libs = "-mwindows -static-libgcc -static-libstdc++ -lfltk_images -lfltk_forms -lfltk_png -lfltk_z -lfltk_jpeg -lfltk -lm -lole32 -luuid -lcomctl32 -lwsock32";
        sprintf(tmpbuffer, "-L%s/lib", homedir);
        project.libdirs = tmpbuffer;
        sprintf(tmpbuffer, "-I%s/include", homedir);
        project.incdirs = tmpbuffer;
    #else
        project.libs = "-lX11 -lfltk_images -lfltk_forms -lfltk -lpng -lz -ljpeg -lm";
        project.libdirs = "-L/usr/X11R6/lib";
        project.incdirs = "-I/usr/X11R6/include";
    #endif
    #endif //MSDOS
        project.run_in_console = false;
        console_check->value(0);
    }
    else
    { //console program
        project.libs = "-static-libgcc -static-libstdc++";
        project.libdirs = "";
        project.incdirs = "";
        project.run_in_console = true;
        console_check->value(1);
    }

    project.cflags = "-O2"; //default compiler flags

    chdir(project.pr_dir.c_str());

    window->pr_browser->clear();
    project.src_files = " ";
    project.header_files = "";
    project.gui_files = "";

    //build main.cpp
    if (main_cpp_chk->value())
    {
        project.src_files = "main.cpp";
        project.header_files = "";
        build_template_main();
        generate_makefile_cb();
        project.addToBrowser(window->pr_browser);
        load_file((char *)(window->pr_browser)->text(1), -1);
        add_recent_file_to_menu(filename);
    }
    else new_cb(0, 0);

    //do this to load option panel with defaults
    project.save();
    project.load();

    bufchanged = 1;
    project.modified = 1;
    pr_opt_cb(NULL, (void*)0); //set options for new project

    project.save();
    project.load();

    window->fb_label->value(strdup((project.pr_dir + string("/")).c_str()));
    window->file_browser->load(project.pr_dir.c_str());

    for (int i = 0; i < 10; i++) menuitems[rec_pr_menu_index + i].activate();
    menubar->copy(menuitems, window);
    add_recent_project_to_menu(strdup(project.pr_filename.c_str()));

    free(tmpbuf);
    //set_title(window);
    //pr_opt_cb; //set options for new project
}


void pref_cb(Fl_Widget*, void*) {
    text_size_choice->value((text_size - 6) / 2);
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
    while (pref_window->visible()) { Fl::wait(5); }
    set_text_size(text_size);
    te->smart_indent = smart_indent_check->value();
}

void linenumbers_cb() {
    if (window->line_nr_box->visible())
    {
        window->hide_linenrs();
        menuitems[29].clear();
    }
    else
    {
        window->show_linenrs();
        menuitems[29].set();
    }

    menubar->copy(menuitems, window);
}

void show_browser_cb() {
    if (window->tabs->Fl_Widget::visible())
    {
        window->hide_browser();
        menuitems[27].clear();
    }
    else
    {
        window->show_browser();
        menuitems[27].set();
    }

    menubar->copy(menuitems, window);
}

void show_nav_cb() {
    int y = browser_file_grp->parent()->y();
    int h = browser_file_grp->parent()->h();



    if (browser_nav_grp->visible())
    {
        menuitems[28].clear();
        browser_nav_grp->hide();
        show_nav = 0;
        browser_file_grp->size(browser_file_grp->w(), h);
    }
    else
    {
        menuitems[28].set();
        show_nav = 1;
        browser_file_grp->size(browser_file_grp->w(), h / 2);
        browser_nav_grp->resize(browser_nav_grp->x(), browser_file_grp->y() + h / 2, browser_nav_grp->w(), h - h / 2);
        browser_nav_grp->show();
    }

    menubar->copy(menuitems, window);
    window->redraw();

}


void pr_browser_cb(Fl_Widget* o, void*) {
    if (Fl::event_button() == 3)
    {
        file_pop_btn->popup();
    }
    else if (Fl::event_clicks() > 0)
    {
        Fl::event_clicks(-1);

        char *name = (char *)((Fl_Browser*)o)->text(((Fl_Browser*)o)->value());

        if (fl_filename_match(name, "*.fl"))
        {
            char buf[255];
            sprintf(buf, "fluid %s &", name);
            FILE *ptr;
            ptr = _popen(buf, "r");
            _pclose(ptr);
            return;
        }
        else if (!check_save()) return;
        load_file(name, -1);
        add_recent_file_to_menu(filename);
    }
}



void file_browser_cb(Fl_Widget* o, void*) {

    if (Fl::event_button() == 3)
    {
        fb_pop_btn->popup();
    }
    else if (Fl::event_clicks() > 0)
    {
        Fl::event_clicks(-1);
        int val = ((Fl_Browser*)o)->value();
        char *browser_text = (char *)((Fl_Browser*)o)->text(val);

        char *l = strdup(window->fb_label->value());
        string s = "";
        s += l;
        s += browser_text;
        free(l);
        l = strdup(s.c_str());

        if (fl_filename_isdir(l))
        {

            if (strcmp(browser_text, "../") == 0)
            {
                if (strcmp(l, "/../") == 0) return;
                char *a = strrchr(l, '/');
                *a = '\0';
                a = strrchr(l, '/');
                *a = '\0';
                a = strrchr(l, '/');
                *(a + 1) = '\0';
            }
            window->fb_label->value(l);
            window->file_browser->load(window->fb_label->value());
            return;
        }

        if (!check_save()) return;
        load_file(l, -1);
        add_recent_file_to_menu(filename);

        free(l);
    }
}


void fb_showhidden_cb(Fl_Widget* o, void*) {
    //window->file_browser->toggle_show_hidden();
    //window->file_browser->reload();
}


void show_help(const char *name) {
#if defined(MSDOS ) || defined(_WIN32)
    return;
#endif
    const char	*docdir;
    char		helpname[1024];

    if (!help_dialog) help_dialog = new Fl_Help_Dialog();

    if (usrdocdir[0] != '/')
    { //full path required
        docdir = getenv("FLTK_DOCDIR");
        if (docdir[0] != '/')
        {
//docdir = "/usr/local/share/doc/fltk";
            snprintf(helpname, sizeof(helpname), "/usr/local/share/doc/fltk/index.html");
        }
        else
        { //read with getenv
            snprintf(helpname, sizeof(helpname), "%s/%s", docdir, name);
        }
    }
    else
    { //valid usrdocdir
        docdir = usrdocdir;
        snprintf(helpname, sizeof(helpname), "%s/%s", docdir, name);
    }


    help_dialog->load(helpname);
    help_dialog->resize(help_dialog->x(), help_dialog->y(), 760, 540);
    help_dialog->show();
}


int ask_for_download() {
#if defined(MSDOS ) || defined(_WIN32)
    return 0;
#endif

    char *homedir = getenv("HOME");
    //	if(!fl_ask(strmsg[0].c_str())) return 0;
    if (!fl_choice(strmsg[0].c_str(), fl_no, fl_yes, 0)) return 0;
    else
    {
        Fl::wait(5);
        FILE *ptr;
        char command[1024];
        char buf[1024];

        ptr = fopen("p.sh", "w");
        fprintf(ptr, \
                "wget http://www.cppreference.com/cppreference-files.tar.gz\n \
gunzip cppreference-files.tar.gz\n \
tar -xf cppreference-files.tar\n \
mv www.cppreference.com %s/.cppref\n", homedir);

        /*
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

        */
        fclose(ptr);

    #ifdef USE_NXLIB
        ptr = _popen("/usr/bin/deskapps/nxterm/nxterm p.sh", "r");
    #else
        ptr = _popen("xterm -e sh p.sh", "r");
    #endif
        _pclose(ptr);

        ptr = _popen("rm -f cppreference-files.tar ", "r");
        _pclose(ptr);

        ptr = _popen("rm -f p.sh ", "r");
        _pclose(ptr);
        /*
                ptr = _popen("rm -rf html ","r");
                _pclose(ptr);
            */
        return 1;
    }
}

void show_ref(const char *name) {
    char	*docdir;
    char		helpname[1024];
#ifdef _WIN32
    char homedir[PATH_MAX];
    int err = get_app_path(homedir, PATH_MAX);
#else  
    char *homedir = getenv("HOME");
#endif

    if (!help_dialog) help_dialog = new Fl_Help_Dialog();

#ifdef MSDOS
    //snprintf(helpname, sizeof(helpname), "%s/%s", ".", "cppref");  
    snprintf(helpname, sizeof(helpname), "%s", "cppref/index.html");
#else
    if ((docdir = getenv("CPP_REF_DOCDIR")) == NULL)
    {
        free(docdir);
        docdir = (char*)malloc((strlen(homedir) + strlen("/.cppref") + 1)*sizeof(char));
        sprintf(docdir, "%s/.cppref", homedir);
    }
    //if(!fl_filename_isdir(docdir)) if(!ask_for_download()) return;
    if (!fl_filename_isdir(docdir)) return; //just return if no .cppref dir
    snprintf(helpname, sizeof(helpname), "%s/%s", docdir, name);
#endif

    help_dialog->load(helpname);
    help_dialog->resize(help_dialog->x(), help_dialog->y(), 760, 540);
    help_dialog->show();
    free(docdir);
}

void manual_cb(Fl_Widget *, void *) {
    show_help("index.html");
}


void ref_cb(Fl_Widget *, void *) {
    show_ref("index.html");
}



void load_config_file() {
    //cout << "Loading config file\t";
    char buf[255], file1[255], file2[255], file3[255], file4[255], pr1[255], pr2[255], pr3[255], pr4[255], scheme[255];
    memset(file1, '\0', 255); memset(file2, '\0', 255); memset(file3, '\0', 255); memset(file4, '\0', 255);
    int x, y, w, h, sm_in, rp_al;
    int BUFSIZE = 255;
    char *homedir = getenv("HOME");
#if defined(MSDOS ) || defined(_WIN32)
    sprintf(buf, "fldev.cfg");
#else
    sprintf(buf, "%s/.fldev_cfg", homedir);
#endif
    FILE *ptr = fopen(buf, "r");

    if (ptr != NULL)
    {
        if (fscanf(ptr, "%d %d %d %d %d\n%d\n%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%d\n",
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
                   &background_color,
                   &usrdocdir,
                   &auto_brace_mode,
                   &backup_file,
                   &delbak,
                   &show_nav))
        {
            if (strlen(file4) > 3) add_recent_file_to_menu(file4 + 3);
            if (strlen(file3) > 3) add_recent_file_to_menu(file3 + 3);
            if (strlen(file2) > 3) add_recent_file_to_menu(file2 + 3);
            if (strlen(file1) > 3) add_recent_file_to_menu(file1 + 3);
            if (strlen(pr4) > 3) add_recent_project_to_menu(pr4 + 3);
            if (strlen(pr3) > 3) add_recent_project_to_menu(pr3 + 3);
            if (strlen(pr2) > 3) add_recent_project_to_menu(pr2 + 3);
            if (strlen(pr1) > 3) add_recent_project_to_menu(pr1 + 3);

            if (strlen(usrdocdir) > 0 && fl_filename_isdir(usrdocdir)) usrdoc_input->value(usrdocdir);
            else usrdoc_input->value("/usr/local/share/doc/fltk");

            Fl::scheme(scheme);
            if (save_window_size)
            {
                window->resize(x, y, w, h);
                int y = browser_file_grp->parent()->y();
                int h = browser_file_grp->parent()->h();

                browser_file_grp->size(browser_file_grp->w(), h / 2);
                browser_nav_grp->resize(browser_nav_grp->x(), browser_file_grp->y() + h / 2, browser_nav_grp->w(), h - h / 2);

            }
            te->smart_indent = sm_in;
            smart_indent_check->value(sm_in != 0);
            auto_brace_check->value(auto_brace_mode != 0);
            rec_pr_check->value(rp_al != 0);
            bak_check->value(backup_file != 0);
            delbak_check->value(delbak != 0);
        }
        else fl_alert(strmsg[16].c_str());
        fclose(ptr);
    }
    //cout << "Done." << endl;

}




void load_language_file() {
    char buf[255], line[255];
    int mode = 0;
    char *homedir = getenv("HOME");
#if defined(MSDOS ) || defined(_WIN32)
    sprintf(buf, "fldev.lng");
#else
    sprintf(buf, "%s/.fldev_lng", homedir);
#endif
    FILE *ptr = fopen(buf, "r");

    char *lineorig, *linetrans;


    if (ptr != NULL)
    {

        while (fgets(line, 255, ptr))
        {
            line[strlen(line) - 1] = '\0';

            if (line[0] == '[')
            {
                if (strncmp(line, "[Menu]", 6) == 0) mode = 1;
                if (strncmp(line, "[Main]", 6) == 0) mode = 2;
                if (strncmp(line, "[Smart]", 7) == 0) mode = 3;
                if (strncmp(line, "[Pref]", 6) == 0) mode = 4;
                if (strncmp(line, "[Opt]", 5) == 0) mode = 5;
                if (strncmp(line, "[Wiz]", 5) == 0) mode = 6;
                if (strncmp(line, "[Msg]", 5) == 0) mode = 7;
            }

            else
            {
                lineorig = strtok(line, ":");
                if (lineorig == NULL) continue;
                linetrans = strtok(NULL, ":");
                if (linetrans == NULL) continue;

                if (mode == 0)
                {
                    if (strcmp(lineorig, "LANG") == 0)
                    {
                        cur_lang_output->value(strdup(linetrans));
                        //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                    }
                }
                else if (mode == 1) //Menu mode
                {
                    for (int i = 0; i < sizeof(menuitems) / sizeof(menuitems[0]); i++)
                    {
                        //printf( "Mode %d, orig \"%s\", trans \"%s\"\n", mode, menuitems[i].label(), linetrans);					

                        if (menuitems[i].label() == 0) continue;

                        if (strcmp(menuitems[i].label(), lineorig) == 0)
                        {
                            replace_text_in_menu(i, linetrans);
                            //menuitems[i].text = linetrans;
                            //printf( "Mode %d, orig %s, trans %s\n", mode, menuitems[i].label(), linetrans);
                        }
                    }
                }
                else if (mode == 2) //Main mode
                {
                    if (strncmp(lineorig, "Project", 7) == 0)
                    {
                        window->tabs->child(0)->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "Browse Dir", 10) == 0)
                    {
                        window->tabs->child(1)->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "Output", 6) == 0)
                    {
                        window->outputwindowbutton->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "INSERT", 6) == 0)
                    {
                        insert_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "OVERWRITE", 9) == 0)
                    {
                        overwrite_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "File", 4) == 0)
                    {
                        file_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "modified", 8) == 0)
                    {
                        mod_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "row", 3) == 0)
                    {
                        row_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "col", 3) == 0)
                    {
                        col_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "Save", 4) == 0)
                    {
                        save_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "Discard", 7) == 0)
                    {
                        discard_str = strdup(linetrans);
                    }
                    else if (strncmp(lineorig, "Cancel", 6) == 0)
                    {
                        cancel_str = strdup(linetrans);
                        window->replace_cancel->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "Replace All", 11) == 0)
                    {
                        window->replace_all->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "Find", 4) == 0)
                    {
                        char buf[64];
                        sprintf(buf, "%s:", strdup(linetrans));
                        window->replace_find->label(strdup(buf));
                    }
                    else if (strncmp(lineorig, "Replace Next", 12) == 0)
                    {
                        window->replace_next->label(strdup(linetrans));
                    }
                    else if (strncmp(lineorig, "Replace", 7) == 0)
                    {
                        char buf[64];
                        sprintf(buf, "%s:", strdup(linetrans));
                        window->replace_dlg->label(strdup(linetrans));
                        window->replace_with->label(strdup(buf));
                    }
                }
                else if (mode == 3) //Smartbar mode
                {
                    for (int i = 0; i < smartbar->children(); i++)
                    {
                        if (smartbar->child(i)->tooltip() == NULL) continue;
                        if (strcmp(smartbar->child(i)->tooltip(), lineorig) == 0)
                        {
                            smartbar->child(i)->tooltip(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                }
                else if (mode == 4) //Preferences mode
                {
                    //cout << "Children:" << 	pref_window->children()	<< endl;	
                    if (strcmp(pref_window->label(), lineorig) == 0)
                    {
                        pref_window->label(strdup(linetrans));
                        //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                    }

                    for (int i = 0; i < pref_window->children(); i++)
                    {
                        if (pref_window->child(i)->label() == NULL) continue;
                        if (strcmp(pref_window->child(i)->label(), lineorig) == 0)
                        {
                            pref_window->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }

                    for (int i = 0; i < pref_tabs->children(); i++)
                    {
                        if (pref_tabs->child(i)->label() == NULL) continue;
                        if (strcmp(pref_tabs->child(i)->label(), lineorig) == 0)
                        {
                            pref_tabs->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < ed_group->children(); i++)
                    {
                        if (ed_group->child(i)->label() == NULL) continue;
                        if (strcmp(ed_group->child(i)->label(), lineorig) == 0)
                        {
                            ed_group->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < env_group->children(); i++)
                    {
                        if (env_group->child(i)->label() == NULL) continue;
                        if (strcmp(env_group->child(i)->label(), lineorig) == 0)
                        {
                            env_group->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }


                }
                else if (mode == 5) //Options mode
                {
                    if (strcmp(project_options_window->label(), lineorig) == 0)
                    {
                        project_options_window->label(strdup(linetrans));
                        //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                    }

                    for (int i = 0; i < project_options_window->children(); i++)
                    {
                        if (project_options_window->child(i)->label() == NULL) continue;
                        if (strcmp(project_options_window->child(i)->label(), lineorig) == 0)
                        {
                            project_options_window->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < pr_op_tabs->children(); i++)
                    {
                        if (pr_op_tabs->child(i)->label() == NULL) continue;
                        if (strcmp(pr_op_tabs->child(i)->label(), lineorig) == 0)
                        {
                            pr_op_tabs->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < pr_op_pr_grp->children(); i++)
                    {
                        if (pr_op_pr_grp->child(i)->label() == NULL) continue;
                        if (strcmp(pr_op_pr_grp->child(i)->label(), lineorig) == 0)
                        {
                            pr_op_pr_grp->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < pr_op_cl_grp->children(); i++)
                    {
                        if (pr_op_cl_grp->child(i)->label() == NULL) continue;
                        if (strcmp(pr_op_cl_grp->child(i)->label(), lineorig) == 0)
                        {
                            pr_op_cl_grp->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                    for (int i = 0; i < pr_op_fi_grp->children(); i++)
                    {
                        if (pr_op_fi_grp->child(i)->label() == NULL) continue;
                        if (strcmp(pr_op_fi_grp->child(i)->label(), lineorig) == 0)
                        {
                            pr_op_fi_grp->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }
                }
                else if (mode == 6) //Wizard mode
                {
                    if (strcmp(proj_wiz->label(), lineorig) == 0)
                    {
                        proj_wiz->label(strdup(linetrans));
                        //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                    }

                    for (int i = 0; i < proj_wiz->children(); i++)
                    {
                        if (proj_wiz->child(i)->label() == NULL) continue;
                        if (strcmp(proj_wiz->child(i)->label(), lineorig) == 0)
                        {
                            proj_wiz->child(i)->label(strdup(linetrans));
                            //printf( "Mode %d, orig %s, trans %s\n", mode, lineorig, linetrans);
                        }
                    }

                }
                else if (mode == 7) //Message mode
                {
                    int nr;
                    if (!sscanf(lineorig, "%d", &nr)) continue;
                    char *nl = strstr(linetrans, "\\n");
                    if (nl)
                    {
                        *nl = ' ';
                        *(nl + 1) = '\n';
                    }
                    if (nr < STR_MSG_SIZE) strmsg[nr] = strdup(linetrans);
                }

            }
        }
        menubar->copy(menuitems, window);
        fclose(ptr);
    }

}



void load_lang_cb(const char *file) {
    char *homedir = getenv("HOME");
    char buf[255];

#if defined(MSDOS ) || defined(_WIN32)
    if (file == NULL)
    {
        _unlink("fldev.lng");
    }
    else
    {
        sprintf(buf, "copy %s fldev.lng", file);
        system(buf);
    }
#else	
    if (file == NULL)
        sprintf(buf, "rm %s/.fldev_lng", homedir);
    else
        sprintf(buf, "cp %s %s/.fldev_lng", file, homedir);
    FILE *ptr = _popen(buf, "r");
    _pclose(ptr);
#endif
    fl_message(strmsg[10].c_str());
}




void save_config_file() {
    char buf[255];
    int BUFSIZE = 255;
    char *homedir = getenv("HOME");
#if defined(MSDOS ) || defined(_WIN32)
    sprintf(buf, "fldev.cfg");
#else
    sprintf(buf, "%s/.fldev_cfg", homedir);
#endif
    FILE *ptr = fopen(buf, "w");
    int startitem = rec_pr_menu_index - 8;

    if (strlen(usrdocdir) < 1) usrdocdir[0] = '\0';

    if (ptr != NULL)
    {
        fprintf(ptr, "%d %d %d %d %d\n%d\n%d\n%s\nf1:%s\nf2:%s\nf3:%s\nf4:%s\np1:%s\np2:%s\np3:%s\np4:%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%d\n",
                save_window_size, window->x(), window->y(), window->w(), window->h(),
                auto_hide,
                text_size,
                Fl::scheme(),
                menuitems[FL_MENU_RECENT_FILES].label(),
                menuitems[FL_MENU_RECENT_FILES + 1].label(),
                menuitems[FL_MENU_RECENT_FILES + 2].label(),
                menuitems[FL_MENU_RECENT_FILES + 3].label(),
                menuitems[FL_MENU_RECENT_PROJECTS].label(),
                menuitems[FL_MENU_RECENT_PROJECTS + 1].label(),
                menuitems[FL_MENU_RECENT_PROJECTS + 2].label(),
                menuitems[FL_MENU_RECENT_PROJECTS + 3].label(),
                te->smart_indent, rec_pr_check->value(),
                hl_plain,
                hl_linecomment,
                hl_blockcomment,
                hl_string,
                hl_directive,
                hl_type,
                hl_keyword,
                hl_character,
                background_color,
                usrdocdir,
                auto_brace_mode,
                backup_file,
                delbak,
                show_nav);
        fclose(ptr);
    }

}








void recent_file_cb(Fl_Widget* w, void*) {
    if (!check_save()) return;
    Fl_Menu_* mw = (Fl_Menu_*)w;
    const Fl_Menu_Item* m = mw->mvalue();
    if (m)
    {
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





void recent_project_cb(Fl_Widget* w, void *v) {
    if (!check_save()) return;
    if (!check_project_save()) return;

    char *newfile;
    Fl_Menu_* mw = (Fl_Menu_*)w;

    if (w)
    {
        const Fl_Menu_Item* m = mw->mvalue();
        if (m) newfile = (char*)m->label();
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
        if (!project.load()) return;
        //add_recent_project_to_menu(newfile);
        char *dir = strdup(newfile);

        char *dirname = strrchr(dir, '/');
        if (dirname != NULL) *dirname = '\0';

        if (fl_filename_isdir(dir))
        {
            chdir(dir);
        }
        project.addToBrowser(window->pr_browser);
        if (window->pr_browser->text(1) != NULL)
            load_file((char *)window->pr_browser->text(1), -1);
        else new_cb(0, 0);

        window->fb_label->value(strdup((project.pr_dir + string("/")).c_str()));
        window->file_browser->load(project.pr_dir.c_str());

        for (int i = 0; i < 10; i++) menuitems[rec_pr_menu_index + i].activate();
        add_recent_project_to_menu(newfile);
        menubar->copy(menuitems, window);

    }

    if (fl_choice(strmsg[6].c_str(), fl_no, fl_yes, 0)) generate_makefile_cb();

    free(path);

}





void add_recent_project_to_menu(const char *filename) {
    char buf[255];
    fl_filename_absolute(buf, filename);
    filename = buf;
    int startitem = FL_MENU_RECENT_PROJECTS;

    if (filename == "") return;

    if (strcmp(filename, menuitems[startitem].label()) == 0) return;

    else if (strcmp(filename, menuitems[startitem + 1].label()) == 0)
    {
        Fl_Menu_Item mi = menuitems[startitem + 1];
        menuitems[startitem + 1] = menuitems[startitem];
        menuitems[startitem] = mi;
    }

    else if (strcmp(filename, menuitems[startitem + 2].label()) == 0)
    {
        Fl_Menu_Item mi = menuitems[startitem + 2];
        menuitems[startitem + 2] = menuitems[startitem + 1];
        menuitems[startitem + 1] = menuitems[startitem];
        menuitems[startitem] = mi;
    }

    else
    {
        Fl_Menu_Item mi = { strdup(filename), 0, recent_project_cb, 0 ,0 };
        menuitems[startitem + 3] = menuitems[startitem + 2];
        menuitems[startitem + 2] = menuitems[startitem + 1];
        menuitems[startitem + 1] = menuitems[startitem];
        menuitems[startitem] = mi;
    }

    menubar->copy(menuitems, window);
}


char *old_rec_men_entry;


void add_recent_file_to_menu(const char *filename) {
    char buf[255];
    fl_filename_absolute(buf, filename);
    //filename = buf;

    if (buf == "") return;
    if (strcmp(buf, menuitems[FL_MENU_RECENT_FILES].label()) == 0) return;

    else if (strcmp(buf, menuitems[FL_MENU_RECENT_FILES + 1].label()) == 0)
    {
        Fl_Menu_Item mi = menuitems[FL_MENU_RECENT_FILES + 1];
        menuitems[FL_MENU_RECENT_FILES + 1] = menuitems[FL_MENU_RECENT_FILES];
        menuitems[FL_MENU_RECENT_FILES] = mi;
    }

    else if (strcmp(buf, menuitems[FL_MENU_RECENT_FILES + 2].label()) == 0)
    {
        Fl_Menu_Item mi = menuitems[FL_MENU_RECENT_FILES + 2];
        menuitems[FL_MENU_RECENT_FILES + 2] = menuitems[FL_MENU_RECENT_FILES + 1];
        menuitems[FL_MENU_RECENT_FILES + 1] = menuitems[FL_MENU_RECENT_FILES];
        menuitems[FL_MENU_RECENT_FILES] = mi;
    }

    else
    {
        Fl_Menu_Item mi = { strdup(buf), 0, recent_file_cb };
        char *t = (char *)menuitems[FL_MENU_RECENT_FILES + 3].text;
        //cout << ">" << t << "<" << endl;
        //a great variable name, cannot find it - apparently unused - memory leak now?
        //if(t) free(t);
        menuitems[FL_MENU_RECENT_FILES + 3] = menuitems[FL_MENU_RECENT_FILES + 2];
        menuitems[FL_MENU_RECENT_FILES + 2] = menuitems[FL_MENU_RECENT_FILES + 1];
        menuitems[FL_MENU_RECENT_FILES + 1] = menuitems[FL_MENU_RECENT_FILES];
        menuitems[FL_MENU_RECENT_FILES] = mi;
    }
    menubar->copy(menuitems, window);
}


void replace_text_in_menu(int index, char *newtext) {
    Fl_Menu_Item mi = { strdup(newtext),
                        menuitems[index].shortcut_,
                        menuitems[index].callback_ ,
                        menuitems[index].user_data_ ,
                        menuitems[index].flags ,
    };
    menuitems[index] = mi;
}






void set_text_size(int t) {
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
    //			te->mCursor_color = hl_plain;

    op_styletable[0].size = t;
    op_styletable[0].color = hl_plain;
    op_styletable[1].size = t;
    //styletable[1].color = hl_plain;

    window->line_nr_box->labelfont(FL_HELVETICA);

    if (t == 12)
    {
        window->line_nr_box->labelsize(t + 2); //14 fits best to text font 12
    }
    else
    {
        window->line_nr_box->labelsize(t); //no matter what you do it won't fit
    }

    window->output->color(background_color);
#if 1
    window->editor->highlight_data(stylebuf, styletable,
                                   sizeof(styletable) / sizeof(styletable[0]),
                                   'A', style_unfinished_cb, 0);
#endif	

#if 0		//do not change navigation window etc as well to new colors -gp
    window->pr_browser->textcolor(hl_plain);
    window->pr_browser->color(background_color);
    navigator_browser->textcolor(hl_plain);
    navigator_browser->color(background_color);
    window->file_browser->textcolor(hl_plain);
    window->file_browser->color(background_color);
#endif
    window->redraw();
}



void home_cb(Fl_Widget* w, void*) {
#if defined(MSDOS ) || defined(_WIN32)
    static char pwd_path[512];
#if MSDOS
    getwd(pwd_path);
#else
    int err = get_app_path(pwd_path, PATH_MAX);
#endif
    window->file_browser->load(pwd_path);
    strcat(pwd_path, "/");
    window->fb_label->value(strdup(pwd_path));

#else     
    window->fb_label->value(strdup((getenv("HOME") + string("/")).c_str()));
    window->file_browser->load(getenv("HOME"));
#endif	
}


void projdir_cb(Fl_Widget* w, void*) {
    char cwd[255];
    _getcwd(cwd, 255);
    window->fb_label->value(strdup((cwd + string("/")).c_str()));
    window->file_browser->load(cwd);
}




void tile_resize(Fl_Widget *w, void *v) {
    //cout << "cb" << endl;

    window->redraw();
    //Fl::check();
    //output_grp->redraw();
}



Fl_Window* make_form() {
    EditorWindow* w = new EditorWindow(800, 600, title);
    window = w;
    int pr_browsersize = 180;
    //int tabs_size = 150;
    w->begin();

    menubar = new Fl_Menu_Bar(0, 0, w->w(), 25);
    menuitems[27].set();
    menuitems[28].set();
    menubar->copy(menuitems, w);
    menubar->box(FL_THIN_UP_BOX);
    menubar->down_box(FL_BORDER_BOX);
    menubar->selection_color(FL_WHITE);


    /////////////////////////////////////////////////////////////////////
    //SmartIcons
    smartbar = new Fl_Group(0, 25, w->w(), 25);
    smartbar->box(FL_THIN_UP_BOX);
    int temp_space = 0, ts_size = 10;
    smartbar->begin();
    int count = 0;
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_new);
        o->callback(new_cb);
        o->tooltip("New File");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_open);
        o->callback(open_cb);
        o->tooltip("Open File");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_save);
        o->callback((Fl_Callback *)save_cb);
        o->tooltip("Save File");
        smartbar->add(o);
        count++;
    }
    {//SPACER
        Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX, 2 + 22 * count + temp_space + ts_size / 2, 26, 1, 22, "");
        smartbar->add(o);
        temp_space += ts_size;
    }//
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_browser);
        o->callback((Fl_Callback *)show_browser_cb);
        o->tooltip("Toggle File Browser");
        smartbar->add(o);
        count++;
    }
    {//SPACER
        Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX, 2 + 22 * count + temp_space + ts_size / 2, 26, 1, 22, "");
        smartbar->add(o);
        temp_space += ts_size;
    }//
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_copy);
        o->callback(copy_cb);
        o->tooltip("Copy");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_cut);
        o->callback(cut_cb);
        o->tooltip("Cut");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_paste);
        o->callback(paste_cb);
        o->tooltip("Paste");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_undo);
        o->callback(undo_cb);
        o->tooltip("Undo");
        smartbar->add(o);
        count++;
    }
    {//SPACER
        Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX, 2 + 22 * count + temp_space + ts_size / 2, 26, 1, 22, "");
        smartbar->add(o);
        temp_space += ts_size;
    }//
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_search);
        o->callback(find_cb);
        o->tooltip("Search");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_search_and_replace);
        o->callback(replace_cb);
        o->tooltip("Replace");
        smartbar->add(o);
        count++;
    }
    {//SPACER
        Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX, 2 + 22 * count + temp_space + ts_size / 2, 26, 1, 22, "");
        smartbar->add(o);
        temp_space += ts_size;
    }//
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_compile);
        o->callback(make_cb);
        o->tooltip("Make");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_run);
        o->callback(run_cb);
        o->tooltip("Run");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_compile_run);
        o->callback(make_run_cb);
        o->tooltip("Make and run");
        smartbar->add(o);
        count++;
    }
    {//SPACER
        Fl_Box *o = new Fl_Box(FL_ENGRAVED_BOX, 2 + 22 * count + temp_space + ts_size / 2, 26, 1, 22, "");
        smartbar->add(o);
        temp_space += ts_size;
    }//
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_fluid);
        o->callback(fluid_cb);
        o->tooltip("Fluid Form Editor");
        smartbar->add(o);
        count++;
    }
    {
        SmartButton *o = new SmartButton(2 + 22 * count + temp_space, 26, 22, 22);
        o->image(image_term);
        o->callback(xterm_cb);
        o->tooltip("XTerm");
        smartbar->add(o);
        count++;
    }

    smartbar->resizable(NULL);
    smartbar->end();


    /////////////////////////////////////////////////////////////////////


    //maintile = new Fl_Tile(0,50, 660, 333);
    all_but_menu_grp = new My_Group_wo_Nav(0, 50, w->w(), 533); //333);


    {
        output_grp = new Fl_Group(0, 50 + 233 + 1, w->w(), 99);
        Fl_Tabs *ttab = new Fl_Tabs(output_grp->x(), output_grp->y() + 7, output_grp->w(), output_grp->h() - 6);
        Fl_Group *tgrp = new Fl_Group(output_grp->x(), output_grp->y() + 7 + 15, output_grp->w(), output_grp->h() - 6 - 15, "Output");

        w->output = new My_Text_Editor2(tgrp->x(), tgrp->y(), tgrp->w(), tgrp->h());
        w->output->highlight_data(op_stylebuf, op_styletable,
                                  sizeof(op_styletable) / sizeof(op_styletable[0]),
                                  'A', style_unfinished_cb, 0);
        w->output->textsize(12);
        w->output->buffer(outputtb);
        w->output->when(FL_WHEN_CHANGED);
        w->output->hide_cursor();
        w->output->output();

        tgrp->labelsize(10);
        tgrp->end();
    #if 0		
        Fl_Group *tdgrp = new Fl_Group(tgrp->x(), tgrp->y(), tgrp->w(), tgrp->h(), "Debug");
        tdgrp->labelsize(10);

        int button_x = 5;
        int button_w = 50;
        {
            Fl_Button *o = new Fl_Button(output_grp->x() + button_x, output_grp->y() + 7 + 31, button_w, 20, "Run");
        #if WITHDEBUG	    		
            o->callback((Fl_Callback *)gdb_run_cb);
        #endif
        }
        button_x += button_w + 5;
        {
            Fl_Button *o = new Fl_Button(output_grp->x() + button_x, output_grp->y() + 7 + 31, button_w, 20, "Step");
        #if WITHDEBUG	    		
            o->callback((Fl_Callback *)gdb_step_cb);
        #endif
        }
        button_x += button_w + 5;
        {
            Fl_Button *o = new Fl_Button(output_grp->x() + button_x, output_grp->y() + 7 + 31, button_w, 20, "Next");
        #if WITHDEBUG	    		
            o->callback((Fl_Callback *)gdb_next_cb);
        #endif
        }
        button_x += button_w + 5;
        {
            Fl_Button *o = new Fl_Button(output_grp->x() + button_x, output_grp->y() + 7 + 31, button_w, 20, "Kill");
        #if WITHDEBUG	    		
            o->callback((Fl_Callback *)gdb_kill_cb);
        #endif
        }


        //tdgrp->deactivate();    


        tdgrp->resizable(NULL);
        tdgrp->end();
    #endif
        ttab->resizable(tgrp);
        ttab->end();

        DragButton *o = new DragButton(0, output_grp->y() + 1, output_grp->w(), 4);
        o->box(FL_EMBOSSED_BOX);
        o->down_box(FL_NO_BOX);
        o->visible_focus(0);
        output_grp->resizable(ttab);

        output_grp->end();
    }


    //Fl_Tile *
    tile = new My_Tile_wo_Nav(0, 50, w->w(), 233);

    gwn = new My_Group_wo_Nav(pr_browsersize, 50, w->w() - pr_browsersize, 233);

    Fl_Box *line_nr_box = new Fl_Box(gwn->x(), 50, line_nr_size, gwn->h());
    line_nr_box->box(FL_DOWN_BOX);
    line_nr_box->align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT | FL_ALIGN_TOP);
    line_nr_box->labelsize(14); //TEXTSIZE);
    //line_nr_box->labelfont(FL_BOLD); 		
    line_nr_box->label("1\n2");
    w->line_nr_box = line_nr_box;

    w->editor = new Fl_Text_Editor_ext(gwn->x() + line_nr_size, 50, w->w() - pr_browsersize - line_nr_size, 233);	//edited
    te = w->editor;
    w->editor->buffer(textbuf);
    w->editor->highlight_data(stylebuf, styletable,
                              sizeof(styletable) / sizeof(styletable[0]),
                              'A', style_unfinished_cb, 0);
    w->editor->textfont(FL_COURIER);
    te->color(background_color);
    //			te->mCursor_color = hl_plain;
    w->editor->end();
    gwn->resizable(te);
    gwn->end();


    int browser_offset = 5;
    w->tabs = new Fl_Tabs(0, 50, pr_browsersize, 233);


    Fl_Group *o = new Fl_Group(0, w->tabs->y() + 20, w->tabs->w(), w->tabs->h() - 20, "Project");
    //o->labeltype(FL_EMBOSSED_LABEL);
    Fl_Tile *tt = new Fl_Tile(0, w->tabs->y() + 20, w->tabs->w(), w->tabs->h() - 20);
    //int gheight = tt->h()-25;
    int gheight = tt->h() / 2;
    //int gheight = tt->h();
    {
        Fl_Group *tg = new Fl_Group(tt->x(), tt->y(), tt->w(), gheight);
        browser_file_grp = tg;
        {
            Fl_Box *labelb = new Fl_Box(FL_EMBOSSED_BOX, tg->x(), tg->y() + 2, tg->w() - 2, 20, "Files");
            labelb->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            labelb->labeltype(FL_EMBOSSED_LABEL);
            labelb->labelsize(12);

            file_pop_btn = new Fl_Menu_Button(tg->x(), tg->y() + 2, 0, 0);
            file_pop_btn->type(Fl_Menu_Button::POPUP3);
            file_pop_btn->add("New/Source", 0, file_source_cb);
            file_pop_btn->add("New/Header", 0, file_header_cb);
            file_pop_btn->add("New/Form", 0, file_form_cb);

            w->pr_browser = new Fl_File_Browser(tg->x(), tg->y() + 22, tg->w() - 2, tg->h() - 22);
            w->pr_browser->type(FL_HOLD_BROWSER);
            w->pr_browser->iconsize(16);
            tg->resizable(w->pr_browser);
        }
        tg->end();
    }

    {
        Fl_Group *tg = new Fl_Group(tt->x(), tt->y() + gheight, tt->w(), tt->h() - gheight);
        browser_nav_grp = tg;
        {
            Fl_Box *labelb = new Fl_Box(FL_EMBOSSED_BOX, tt->x(), tg->y(), tg->w() - 2, 20, "Navigator");
            labelb->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            labelb->labeltype(FL_EMBOSSED_LABEL);
            labelb->labelsize(12);

            navigator_pop_btn = new Fl_Menu_Button(tt->x(), tg->y(), 0, 0);
            navigator_pop_btn->type(Fl_Menu_Button::POPUP3);
            navigator_pop_btn->add("Expand Classes", 0, nav_exp_cb);
            navigator_pop_btn->add("Normal Order", 0, nav_norm_cb);
            navigator_pop_btn->add("Sort by Name", 0, nav_name_cb);
            navigator_pop_btn->add("Sort by Return Type", 0, nav_type_cb);

            //Fl_Hold_Browser *hb = new Fl_Hold_Browser(tt->x(),tg->y()+20,tg->w()-2,tg->h()-20);
            NavBrowser *hb = new NavBrowser(tt->x(), tg->y() + 20, tg->w() - 2, tg->h() - 20);
            navigator_browser = hb;
            navigator_browser->textsize(12);
            navigator_browser->callback(navigator_browser_cb);
            tg->box(FL_FLAT_BOX);
            tg->resizable(hb);
        }
        tg->end();
    }
    tt->box(FL_FLAT_BOX);
    w->pr_browser->callback(pr_browser_cb);
    tt->end();

    o->resizable(w->pr_browser);

    o->end();
    o = new Fl_Group(0, w->tabs->y() + 20, w->tabs->w(), w->tabs->h() - 20, "Browse Dir");
    //o->labeltype(FL_EMBOSSED_LABEL);
    char buf[1024];
    _getcwd(buf, 1024);

    w->fb_label = new Fl_Output(o->x(), o->y() + browser_offset, o->w() - 25, 20);
    w->fb_label->value(strdup((buf + string("/")).c_str()));
    w->fb_home_btn = new Fl_Menu_Button(o->x() + o->w() - 24, o->y() + browser_offset, 23, 20);
    w->fb_home_btn->box(FL_THIN_UP_BOX);
    w->fb_home_btn->add("Home Dir", 0, home_cb);
    w->fb_home_btn->add("Project Dir", 0, projdir_cb);

    fb_pop_btn = new Fl_Menu_Button(o->x(), o->y() + browser_offset, 0, 0);
    fb_pop_btn->type(Fl_Menu_Button::POPUP3);
    fb_pop_btn->add("Show Hidden", 0, fb_showhidden_cb);

    w->file_browser = new Fl_File_Browser(o->x(), o->y() + 20 + browser_offset, o->w(), o->h() - 20 - browser_offset);

    w->file_browser->load(buf);
    w->file_browser->iconsize(16);
    w->file_browser->callback(file_browser_cb);
    o->resizable(w->file_browser);
    o->end();
    w->tabs->resizable(o);
    w->tabs->end();

    tile->when(FL_WHEN_RELEASE);
    tile->callback(tile_resize);
    tile->end();

    //maintile->resizable(te);
    all_but_menu_grp->end();
    //w->resizable(all_but_menu_grp);
    w->resizable(tile);


    w->statusbar = new Fl_Box(FL_EMBOSSED_BOX, 0, w->h() - 16, w->w() - 45, 16, "Zeile 1");
    w->statusbar->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    w->statusbar->labelsize(10);
    w->callback((Fl_Callback *)quit_cb, w);

    w->outputwindowbutton = new Fl_Button(w->w() - 40, w->h() - 16, 40, 16, "Output");
    w->outputwindowbutton->callback(owbt_callback);
    w->outputwindowbutton->visible_focus(0);
    w->outputwindowbutton->labelsize(10);

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



void load_default_icons(void) {
    Fl_File_Icon	*icon;		// New icons
    char		filename[1024];	// Filename
    static int	init = 0;	// Have the icons been initialized?


    {

        // Create the default icons...fldevicon_xpm
        //
        new Fl_File_Icon("*", Fl_File_Icon::PLAIN, sizeof(unknown_file_icon) / sizeof(unknown_file_icon[0]), unknown_file_icon);

        //new Fl_File_Icon("*", Fl_File_Icon::PLAIN, sizeof(plain) / sizeof(plain[0]), plain);
        new Fl_File_Icon("*.{cpp|cxx|c|cc|CPP|CXX|C|CC}", Fl_File_Icon::PLAIN, sizeof(src_file_icon) / sizeof(src_file_icon[0]), src_file_icon);
        new Fl_File_Icon("*.{h|hpp|H|HPP}", Fl_File_Icon::PLAIN, sizeof(hdr_file_icon) / sizeof(hdr_file_icon[0]), hdr_file_icon);
        new Fl_File_Icon("*.{f|fl}", Fl_File_Icon::PLAIN, sizeof(fl_file_icon) / sizeof(fl_file_icon[0]), fl_file_icon);

        new Fl_File_Icon("*", Fl_File_Icon::DIRECTORY, sizeof(folder_file_icon) / sizeof(folder_file_icon[0]), folder_file_icon);


        /*
        Fl_File_Icon	*icon2 = new Fl_File_Icon("*", Fl_File_Icon::DIRECTORY, sizeof(folder_file_icon) / sizeof(folder_file_icon[0]), folder_file_icon);
        icon2->load("conv_icon.png");
        short *l = icon2->value();
        cout << "static short icon[] = { ";
        for(int i = 0; i < icon2->size(); i++)
        {
              cout << l[i] << ", ";
        }
        cout << " }" << endl << flush;
          */
    }

    // Mark things as initialized...
    init = 1;

}

//void update_line_nrs(EditorWindow* window, int oldzeile, int oldspalte, int old_ins_mode, int old_first_line, int old_last_line){
//void update_line_nrs(EditorWindow* window){       
void update_line_nrs(void*) {

    if (!window->visible()) return;

    static int oldzeile = -1, oldspalte = -1;

    static int old_ins_mode, old_first_line, old_last_line;

    static char b[255];
    int zeile = window->editor->count_lines(0, window->editor->insert_position(), 0) + 1;
    int spalte = window->editor->insert_position() - window->editor->line_start(window->editor->insert_position());
    int ins_mode = window->editor->insert_mode();

    int first_line;
    int last_line;
    te->get_line_nrs(&first_line, &last_line);

    //printf("topline:%d\n",first_line);
    //printf("lastline:%d\n",last_line);

    if (window->line_nr_box->visible() && (first_line != old_first_line || last_line != old_last_line))
    {
        line_label_str = "";
        for (int i = first_line; i < (last_line + 1); i++)
        {
            char buf[20];
            sprintf(buf, "%d\n", i);
            line_label_str += buf;
        }
        //cout << first_line << " " << last_line << endl;
        window->line_nr_box->label(line_label_str.c_str());
        window->redraw();
        old_first_line = first_line;
        old_last_line = last_line;
    }

    if (zeile != oldzeile || spalte != oldspalte || ins_mode != old_ins_mode)
    {
        int gz = window->editor->count_lines(0, textbuf->length(), 0) + 1;
        if (ins_mode) sprintf(b, "%s %d (%d), %s %d \t%s", row_str.c_str(), zeile, gz, col_str.c_str(), spalte, insert_str.c_str());
        else sprintf(b, "%s %d (%d), %s %d \t%s", row_str.c_str(), zeile, gz, col_str.c_str(), spalte, overwrite_str.c_str());
        window->statusbar->label(b);
        oldzeile = zeile;
        oldspalte = spalte;
        old_ins_mode = ins_mode;
    }
    Fl::repeat_timeout(0.25, update_line_nrs);
}


int main(int argc, char **argv) {
#ifndef _WIN32
    Fl_Pixmap p, mask;
#endif
#ifdef MSDOS
    __djgpp_set_ctrl_c(0);
    //select the grad1 scheme - looks a bit like Win7
    setenv("FLTK_SCHEME", "grad1", 0); //do not replace existing
    //  Fl::scheme("grad1");
#else
    Fl::scheme("base");
#endif


    //  fl_open_display();  
    //  XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display),fldevicon_xpm, &p, &mask, NULL);

    textbuf = new Fl_Text_Buffer;
    outputtb = new Fl_Text_Buffer;
    style_init();
    op_stylebuf = new Fl_Text_Buffer();

    fl_register_images();
    //Fl_File_Icon::load_system_icons();
    load_default_icons();

    window = (EditorWindow*)make_form();
#ifndef _WIN32  
    window->icon((char *)p);
#else
    window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
    window->show(1, argv);
    //window->show();

#if WITHHINTS  
  //Set Icon Mask for transparent Icon
    XWMHints *hints = XGetWMHints(fl_display, fl_xid(window));
    hints->icon_mask = mask;
    hints->flags |= IconMaskHint;// | IconPixmapHint;  
    XSetWMHints(fl_display, fl_xid(window), hints);
#endif

    init_strings();

    file_hist = new File_History();
    text_size = 12;
    load_config_file();

    //cout << "Loading language file\t";
    load_language_file();
    //cout << "Done."<<endl;

    int r;
    if (argc > 1)
    {
        r = load_file(argv[1], -1);
        //open file
        if (r == 1)
        {
            char buf[255];
        #if defined(MSDOS ) || defined(_WIN32)
            char pwd_buf[512];
        #ifdef MSDOS        
            getwd(pwd_buf);
        #else
            int err = get_app_path(pwd_buf, PATH_MAX);
        #endif        
            sprintf(buf, "%s/%s", pwd_buf, filename_wo_path);
        #else		
            sprintf(buf, "%s/%s", getenv("PWD"), filename_wo_path);
        #endif
            add_recent_file_to_menu(buf);
            window->hide_browser();
        }
        //open project
        else if (r == 2)
        {
//open first file of project
            if (window->pr_browser->text(1) != NULL) ((char*)window->pr_browser->text(1), -1);
        }
    }
    else if (rec_pr_check->value())
    {
//open last opened project
        recent_project_cb(0, (void*)menuitems[rec_pr_menu_index - 4].label());
    }
    else window->label("FLDev");

    set_text_size(text_size);

    if (!project.assigned)
    {
        for (int i = 0; i < 10; i++) menuitems[rec_pr_menu_index + i].deactivate();
        menubar->copy(menuitems, window);
    }

    window->hide_output();
    window->hide_linenrs();
    output_grp->size(output_grp->w(), 100);

    if (show_nav == 0) show_nav_cb();

    Fl::add_timeout(0.25, update_line_nrs);

    return Fl::run();

#if WITHHINTS
    free(hints);
#endif
    return 1;
}
