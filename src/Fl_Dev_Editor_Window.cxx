/*
    FLDev - FLTK Integrated Development Environment
    Copyright (C) 2005-2016  Philipp Pracht, Georg Potthast, Neil McNeight

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Fl_Dev_Editor_Window.h"
#include "Fl_Dev_Callbacks.h"
#include "globals.h"

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Editor window functions and class...



Fl_Dev_Editor_Window::Fl_Dev_Editor_Window(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
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



Fl_Dev_Editor_Window::~Fl_Dev_Editor_Window() {
    delete replace_dlg;
}



void Fl_Dev_Editor_Window::draw() {
    Fl_Double_Window::draw();
}


void Fl_Dev_Editor_Window::show_linenrs() {
    if (line_nr_box->visible()) return;


    te->resize(gwn->x() + line_nr_size, te->y(), gwn->w() - line_nr_size, te->h());
    line_nr_box->resize(gwn->x(), te->y(), line_nr_size, te->h());
    gwn->add(line_nr_box);
    line_nr_box->show();

    redraw();
    Fl::check();
}


void Fl_Dev_Editor_Window::hide_linenrs() {
    if (!line_nr_box->visible()) return;

    gwn->remove(line_nr_box);
    te->resize(gwn->x() + 1, te->y(), gwn->w() - 1, te->h());
    //line_nr_box->resize(te->x()-line_nr_size,te->y(),line_nr_size,te->h());
    line_nr_box->hide();
    redraw();
    Fl::check();
}

//shows the compiler-output-widget at the bottom
void Fl_Dev_Editor_Window::show_output() {
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
void Fl_Dev_Editor_Window::hide_output() {
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
void Fl_Dev_Editor_Window::show_browser() {
    tabs->size(150, editor->h());
    gwn->resize(tabs->w(), gwn->y(), gwn->w() - tabs->w(), gwn->h());
    tabs->show();
    gwn->redraw();
    Fl::check();
}

//hides the project-/file-browser at the left
void Fl_Dev_Editor_Window::hide_browser() {
    gwn->resize(0, gwn->y(), w(), gwn->h());
    tabs->hide();
}
