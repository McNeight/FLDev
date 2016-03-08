// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef pref_form_h
#define pref_form_h
#include <FL/Fl.H>
extern int text_size;
extern int save_window_size;
extern int auto_hide;
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *pref_window;
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
extern Fl_Button *pref_ok_btn;
extern Fl_Button *pref_cancel_btn;
#include <FL/Fl_Choice.H>
extern Fl_Choice *text_size_choice;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *smart_indent_check;
extern Fl_Check_Button *save_wsoe_check;
extern Fl_Check_Button *hide_output_check;
extern Fl_Check_Button *rec_pr_check;
Fl_Double_Window* make_pref_form();
#endif