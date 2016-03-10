// generated by Fast Light User Interface Designer (fluid) version 1.0303

#ifndef pref_form_h
#define pref_form_h
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_show_colormap.H>
#include "globals.h"
extern void load_lang_cb(const char *file); 
extern int text_size; 
extern Fl_Color hl_plain;
extern Fl_Color hl_linecomment;
extern Fl_Color hl_blockcomment;
extern Fl_Color hl_string;
extern Fl_Color hl_directive;
extern Fl_Color hl_type;
extern Fl_Color hl_keyword;
extern Fl_Color hl_character;
extern Fl_Color background_color;
extern char usrdocdir[256];
extern int save_window_size; 
extern bool auto_brace_mode; 
extern int auto_hide; 
extern int delbak; 
extern int backup_file; 
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *pref_window;
#include <FL/Fl_Button.H>
extern Fl_Button *pref_ok_btn;
extern Fl_Button *pref_cancel_btn;
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *pref_tabs;
#include <FL/Fl_Group.H>
extern Fl_Group *ed_group;
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
extern Fl_Choice *text_size_choice;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *smart_indent_check;
extern Fl_Check_Button *save_wsoe_check;
extern Fl_Check_Button *hide_output_check;
extern Fl_Check_Button *rec_pr_check;
extern Fl_Box *pref_highlight_label;
extern Fl_Button *plain_btn;
extern Fl_Button *line_btn;
extern Fl_Button *block_btn;
extern Fl_Button *string_btn;
extern Fl_Button *directive_btn;
extern Fl_Button *type_btn;
extern Fl_Button *keyword_btn;
extern Fl_Button *character_btn;
extern Fl_Button *bg_btn;
extern Fl_Check_Button *auto_brace_check;
extern Fl_Check_Button *bak_check;
extern Fl_Check_Button *delbak_check;
extern Fl_Group *env_group;
#include <FL/Fl_Input.H>
extern Fl_Input *usrdoc_input;
extern Fl_Button *lang_btn;
#include <FL/Fl_Output.H>
extern Fl_Output *cur_lang_output;
extern Fl_Button *def_lang_btn;
Fl_Double_Window* make_pref_form();
#endif
