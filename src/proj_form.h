// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef proj_form_h
#define proj_form_h
#include <FL/Fl.H>
#include "globals.h"
extern int pr_changed; 
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *project_options_window;
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *pr_op_tabs;
#include <FL/Fl_Group.H>
extern Fl_Group *pr_op_pr_grp;
#include <FL/Fl_Input.H>
extern Fl_Input *pname;
extern Fl_Input *ppr_dir;
extern Fl_Input *ptarg;
extern Fl_Input *pobdir;
extern Fl_Input *pbin;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *console_check;
extern Fl_Group *pr_op_cl_grp;
extern Fl_Input *plib;
extern Fl_Input *plibdir;
extern Fl_Input *pincl;
extern Fl_Input *pcflags;
extern Fl_Input *pldflags;
extern Fl_Group *pr_op_fi_grp;
extern Fl_Input *psrc;
extern Fl_Input *phdr;
extern Fl_Input *pgui;
#include <FL/Fl_Button.H>
Fl_Double_Window* make_proj_window();
#endif