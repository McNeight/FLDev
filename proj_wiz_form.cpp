// generated by Fast Light User Interface Designer (fluid) version 1.0104

#include "proj_wiz_form.h"
bool pr_wiz_ok;

Fl_Double_Window *proj_wiz=(Fl_Double_Window *)0;

Fl_Input *pr_dir_input=(Fl_Input *)0;

Fl_Button *pr_wiz_browse_btn=(Fl_Button *)0;

static void cb_pr_wiz_browse_btn(Fl_Button*, void*) {
  pr_dir_input->value(fl_dir_chooser("Choose Directory",0,0));
}

Fl_Input *pr_name_input=(Fl_Input *)0;

Fl_Button *pr_wiz_ok_btn=(Fl_Button *)0;

static void cb_pr_wiz_ok_btn(Fl_Button*, void*) {
  pr_wiz_ok=true;
proj_wiz->hide();
}

Fl_Button *pr_wiz_cancel_btn=(Fl_Button *)0;

static void cb_pr_wiz_cancel_btn(Fl_Button*, void*) {
  proj_wiz->hide();
}

Fl_Round_Button *x_app=(Fl_Round_Button *)0;

Fl_Round_Button *con_app=(Fl_Round_Button *)0;

Fl_Double_Window* make_proj_wizard() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = proj_wiz = new Fl_Double_Window(500, 210, "New Project");
    w = o;
    pr_dir_input = new Fl_Input(130, 65, 320, 25, "Project Directory:");
    { Fl_Button* o = pr_wiz_browse_btn = new Fl_Button(450, 65, 35, 25, "...");
      o->callback((Fl_Callback*)cb_pr_wiz_browse_btn);
    }
    pr_name_input = new Fl_Input(130, 25, 320, 25, "Project Name:");
    { Fl_Button* o = pr_wiz_ok_btn = new Fl_Button(320, 170, 80, 25, "OK");
      o->callback((Fl_Callback*)cb_pr_wiz_ok_btn);
    }
    { Fl_Button* o = pr_wiz_cancel_btn = new Fl_Button(410, 170, 80, 25, "Cancel");
      o->callback((Fl_Callback*)cb_pr_wiz_cancel_btn);
    }
    { Fl_Group* o = new Fl_Group(130, 100, 145, 50, "Type:");
      o->box(FL_THIN_UP_BOX);
      o->align(FL_ALIGN_LEFT);
      { Fl_Round_Button* o = x_app = new Fl_Round_Button(155, 125, 85, 25, "X11 App");
        o->type(102);
        o->down_box(FL_ROUND_DOWN_BOX);
      }
      { Fl_Round_Button* o = con_app = new Fl_Round_Button(155, 100, 110, 25, "Console App");
        o->type(102);
        o->down_box(FL_ROUND_DOWN_BOX);
        con_app->value(1);
      }
      o->end();
    }
    o->set_modal();
    o->end();
  }
  return w;
}