// generated by Fast Light User Interface Designer (fluid) version 1.0303

#include "proj_form.h"
int pr_changed; 

Fl_Double_Window *project_options_window=(Fl_Double_Window *)0;

Fl_Tabs *pr_op_tabs=(Fl_Tabs *)0;

Fl_Group *pr_op_pr_grp=(Fl_Group *)0;

Fl_Input *pname=(Fl_Input *)0;

static void cb_pname(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *ppr_dir=(Fl_Input *)0;

static void cb_ppr_dir(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *ptarg=(Fl_Input *)0;

static void cb_ptarg(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pobdir=(Fl_Input *)0;

static void cb_pobdir(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pbin=(Fl_Input *)0;

static void cb_pbin(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Check_Button *console_check=(Fl_Check_Button *)0;

static void cb_console_check(Fl_Check_Button*, void*) {
  if(pr_changed == 0) pr_changed = 2;
}

Fl_Group *pr_op_cl_grp=(Fl_Group *)0;

Fl_Input *plib=(Fl_Input *)0;

static void cb_plib(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *plibdir=(Fl_Input *)0;

static void cb_plibdir(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pincl=(Fl_Input *)0;

static void cb_pincl(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pcflags=(Fl_Input *)0;

static void cb_pcflags(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pldflags=(Fl_Input *)0;

static void cb_pldflags(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Group *pr_op_fi_grp=(Fl_Group *)0;

Fl_Input *psrc=(Fl_Input *)0;

static void cb_psrc(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *phdr=(Fl_Input *)0;

static void cb_phdr(Fl_Input*, void*) {
  pr_changed = true;
}

Fl_Input *pgui=(Fl_Input *)0;

static void cb_pgui(Fl_Input*, void*) {
  pr_changed = true;
}

static void cb_add(Fl_Button*, void*) {
  Fl_Native_File_Chooser fnfc;
fnfc.title(strmsg[3].c_str());
fnfc.filter("C & C++ Source Files\t*.{cpp,cxx,c,cc,CPP,CXX,C,CC}");
fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
if (fnfc.show())
    return;

const char *newfile = fnfc.filename();

if(newfile=="") return;
if (newfile != NULL) 
{
	char rel[255];
	fl_filename_relative(rel,newfile);
	psrc->value(trim(string(psrc->value())+ " " + rel).c_str());
	pr_changed = true;
};
}

static void cb_add1(Fl_Button*, void*) {
  Fl_Native_File_Chooser fnfc;
fnfc.title(strmsg[3].c_str());
fnfc.filter("C & C++ Header Files\t*.{h|hpp|H|HPP}");
fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
if (fnfc.show())
    return;

const char *newfile = fnfc.filename();

if(newfile=="") return;
if (newfile != NULL) 
{
	char rel[255];
	fl_filename_relative(rel,newfile);
	phdr->value(trim(string(phdr->value())+ " " + rel).c_str());
	pr_changed = true;
};
}

static void cb_add2(Fl_Button*, void*) {
  Fl_Native_File_Chooser fnfc;
fnfc.title(strmsg[3].c_str());
fnfc.filter("FLUID Files\t*.{fl,fd}");
fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
if (fnfc.show())
    return;

const char *newfile = fnfc.filename();

if(newfile=="") return;
if (newfile != NULL) 
{
	char rel[255];
	fl_filename_relative(rel,newfile);
	pgui->value(trim(string(pgui->value())+ " " + rel).c_str());
	pr_changed = true;
};
}

static void cb_OK(Fl_Button*, void*) {
  project_options_window->hide();
}

static void cb_Cancel(Fl_Button*, void*) {
  pr_changed = 0;
project_options_window->hide();
}

Fl_Double_Window* make_proj_window() {
  { project_options_window = new Fl_Double_Window(410, 370, "Project Options");
    { pr_op_tabs = new Fl_Tabs(5, 5, 400, 291);
      { pr_op_pr_grp = new Fl_Group(5, 35, 400, 261, "Project");
        pr_op_pr_grp->hide();
        { pname = new Fl_Input(15, 60, 380, 25, "Project Name");
          pname->callback((Fl_Callback*)cb_pname);
          pname->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pname->when(FL_WHEN_CHANGED);
        } // Fl_Input* pname
        { ppr_dir = new Fl_Input(15, 105, 380, 25, "Project Directory");
          ppr_dir->callback((Fl_Callback*)cb_ppr_dir);
          ppr_dir->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Input* ppr_dir
        { ptarg = new Fl_Input(15, 150, 380, 25, "Target File Name");
          ptarg->callback((Fl_Callback*)cb_ptarg);
          ptarg->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          ptarg->when(FL_WHEN_CHANGED);
        } // Fl_Input* ptarg
        { pobdir = new Fl_Input(15, 195, 380, 25, "Object Directory");
          pobdir->callback((Fl_Callback*)cb_pobdir);
          pobdir->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pobdir->when(FL_WHEN_CHANGED);
        } // Fl_Input* pobdir
        { pbin = new Fl_Input(15, 240, 380, 25, "Bin Directory");
          pbin->callback((Fl_Callback*)cb_pbin);
          pbin->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pbin->when(FL_WHEN_CHANGED);
        } // Fl_Input* pbin
        { console_check = new Fl_Check_Button(125, 261, 150, 35, "Execute in Console");
          console_check->down_box(FL_DOWN_BOX);
          console_check->callback((Fl_Callback*)cb_console_check);
          console_check->when(FL_WHEN_CHANGED);
        } // Fl_Check_Button* console_check
        pr_op_pr_grp->end();
      } // Fl_Group* pr_op_pr_grp
      { pr_op_cl_grp = new Fl_Group(5, 35, 400, 260, "Compiler and Linker");
        pr_op_cl_grp->hide();
        { plib = new Fl_Input(15, 60, 380, 25, "Libraries");
          plib->callback((Fl_Callback*)cb_plib);
          plib->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          plib->when(FL_WHEN_CHANGED);
        } // Fl_Input* plib
        { plibdir = new Fl_Input(15, 105, 380, 25, "Library Dirs");
          plibdir->callback((Fl_Callback*)cb_plibdir);
          plibdir->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          plibdir->when(FL_WHEN_CHANGED);
        } // Fl_Input* plibdir
        { pincl = new Fl_Input(15, 150, 380, 25, "Include Dirs");
          pincl->callback((Fl_Callback*)cb_pincl);
          pincl->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pincl->when(FL_WHEN_CHANGED);
        } // Fl_Input* pincl
        { pcflags = new Fl_Input(15, 195, 380, 25, "Compiler Flags");
          pcflags->callback((Fl_Callback*)cb_pcflags);
          pcflags->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pcflags->when(FL_WHEN_CHANGED);
        } // Fl_Input* pcflags
        { pldflags = new Fl_Input(15, 240, 380, 25, "Linker Flags");
          pldflags->callback((Fl_Callback*)cb_pldflags);
          pldflags->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pldflags->when(FL_WHEN_CHANGED);
        } // Fl_Input* pldflags
        pr_op_cl_grp->end();
      } // Fl_Group* pr_op_cl_grp
      { pr_op_fi_grp = new Fl_Group(5, 35, 400, 260, "Files");
        { psrc = new Fl_Input(15, 60, 340, 25, "Sources");
          psrc->callback((Fl_Callback*)cb_psrc);
          psrc->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          psrc->when(FL_WHEN_CHANGED);
        } // Fl_Input* psrc
        { phdr = new Fl_Input(15, 105, 340, 25, "Headers");
          phdr->callback((Fl_Callback*)cb_phdr);
          phdr->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          phdr->when(FL_WHEN_CHANGED);
        } // Fl_Input* phdr
        { pgui = new Fl_Input(15, 150, 340, 25, "Fluid GUI Files");
          pgui->callback((Fl_Callback*)cb_pgui);
          pgui->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          pgui->when(FL_WHEN_CHANGED);
        } // Fl_Input* pgui
        { Fl_Button* o = new Fl_Button(360, 60, 35, 25, "add");
          o->callback((Fl_Callback*)cb_add);
        } // Fl_Button* o
        { Fl_Button* o = new Fl_Button(360, 105, 35, 25, "add");
          o->callback((Fl_Callback*)cb_add1);
        } // Fl_Button* o
        { Fl_Button* o = new Fl_Button(360, 150, 35, 25, "add");
          o->callback((Fl_Callback*)cb_add2);
        } // Fl_Button* o
        pr_op_fi_grp->end();
      } // Fl_Group* pr_op_fi_grp
      pr_op_tabs->end();
    } // Fl_Tabs* pr_op_tabs
    { Fl_Button* o = new Fl_Button(70, 319, 75, 25, "OK");
      o->callback((Fl_Callback*)cb_OK);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(255, 319, 75, 25, "Cancel");
      o->callback((Fl_Callback*)cb_Cancel);
    } // Fl_Button* o
    project_options_window->set_modal();
    project_options_window->end();
  } // Fl_Double_Window* project_options_window
  return project_options_window;
}
