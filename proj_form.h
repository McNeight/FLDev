// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef proj_form_h
#define proj_form_h
#include <FL/Fl.H>
extern int pr_changed;
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *project_options_window;
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
extern Fl_Input *pname;
extern Fl_Input *ptarg;
extern Fl_Input *pbin;
extern Fl_Input *pobdir;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *console_check;
extern Fl_Input *plib;
extern Fl_Input *plibdir;
extern Fl_Input *pincl;
extern Fl_Input *pcflags;
extern Fl_Input *pldflags;
extern Fl_Input *psrc;
extern Fl_Input *phdr;
#include <FL/Fl_Button.H>
Fl_Double_Window* make_proj_window();
#endif
