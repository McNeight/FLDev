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
#include "Fl_Dev_I18N.h"
#include "Fl_Dev_Project.h"
#include "Fl_Dev_Util.h"

void Fl_Dev_Project::addToBrowser(Fl_File_Browser *b) {
    b->clear();

    Fl_File_Icon *icon;

    icon = Fl_File_Icon::find("*.cpp", Fl_File_Icon::PLAIN);

    char *tmp_src_files = strdup(src_files.c_str());
    while (src_files != "")
    {
        char *fname = strsep(&tmp_src_files, " ");

        char *first = strsep(&fname, ".");

        char fn[255];


        sprintf(fn, "%s.%s", first, fname);
        if (fn != NULL && fn != "") b->add(fn, icon);

        if (tmp_src_files == NULL || tmp_src_files == "") break;
    }

    icon = Fl_File_Icon::find("*.h", Fl_File_Icon::PLAIN);

    char *tmp_hdr_files = strdup(header_files.c_str());
    while (header_files != "")
    {
        char *fname = strsep(&tmp_hdr_files, " ");

        char *first = strsep(&fname, ".");

        char fn[255];

        sprintf(fn, "%s.%s", first, fname);
        if (fn != NULL && fn != "") b->add(fn, icon);

        if (tmp_hdr_files == NULL || tmp_hdr_files == "") break;
    }

    icon = Fl_File_Icon::find("*.fl", Fl_File_Icon::PLAIN);

    char *tmp_gui_files = strdup(gui_files.c_str());
    while (gui_files != "")
    {
        char *fname = strsep(&tmp_gui_files, " ");

        char *first = strsep(&fname, ".");

        char fn[255];

        sprintf(fn, "%s.%s", first, fname);
        if (fn != NULL && fn != "") b->add(fn, icon);

        if (tmp_gui_files == NULL || tmp_gui_files == "") break;
    }
}


void Fl_Dev_Project::save() {
    if (trim(name) == "") return;

    char buf[255];
    int BUFSIZE = 255;
#if MSDOS
    sprintf(buf, "%s.fld", name.c_str());
#elif _WIN32
    sprintf(buf, "%s\\%s.fldev", pr_dir.c_str(), name.c_str());
#else	
    sprintf(buf, "%s/%s.fldev", pr_dir.c_str(), name.c_str());
#endif
    FILE *ptr = fopen(buf, "w");
    run_in_console = console_check->value();
    if (ptr != NULL)
    {
        fprintf(ptr, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n%s\n%s\n%s\n%s\n%d\n",
                name.c_str(),
                binfilename.c_str(),
                oDir.c_str(),
                Bin.c_str(),
                libs.c_str(),
                libdirs.c_str(),
                incdirs.c_str(),
                ldflags.c_str(),
                cflags.c_str(),
                run_in_console,
                src_files.c_str(),
                header_files.c_str(),
                gui_files.c_str(),
                pr_dir.c_str(),
                own_makefile.c_str(),
                use_own_makefile
                );
        fclose(ptr);
        modified = false;
    }
}

int Fl_Dev_Project::load() {
    char buf[255];
    int BUFSIZE = 255;

    sprintf(buf, "%s", pr_filename.c_str());

    FILE *ptr = fopen(buf, "r");

    char aname[255], abinf[255], aodir[255], abin[255], alibs[255], alibd[255], ainc[255], ald[255], ac[255], asrc[255], ahdr[255], agui[255], acons[255], aprdir[255], amake[255], amake_chk[255];
    if (ptr != NULL)
    {
        fgets(aname, 255, ptr);
        fgets(abinf, 255, ptr);
        fgets(aodir, 255, ptr);
        fgets(abin, 255, ptr);
        fgets(alibs, 255, ptr);
        fgets(alibd, 255, ptr);
        fgets(ainc, 255, ptr);
        fgets(ald, 255, ptr);
        fgets(ac, 255, ptr);
        fgets(acons, 255, ptr);
        fgets(asrc, 255, ptr);
        fgets(ahdr, 255, ptr);
        char *guil = fgets(agui, 255, ptr);
        fgets(aprdir, 255, ptr);
        fgets(amake, 255, ptr);
        fgets(amake_chk, 255, ptr);

        name = trim(aname);
        binfilename = trim(abinf);
        oDir = trim(aodir);
        Bin = trim(abin);
        libs = trim(alibs);
        libdirs = trim(alibd);
        incdirs = trim(ainc);
        ldflags = trim(ald);
        cflags = trim(ac);
        src_files = trim(asrc);
        header_files = trim(ahdr);
        if (guil) gui_files = trim(agui);
        else gui_files = "";
        pr_dir = trim(aprdir);
        sscanf(acons, "%d", &run_in_console);
        own_makefile = trim(amake);
        sscanf(amake_chk, "%d", &use_own_makefile);

        pname->value(name.c_str());
        ptarg->value(binfilename.c_str());
        ppr_dir->value(pr_dir.c_str());
        psrc->value(src_files.c_str());
        phdr->value(header_files.c_str());
        pgui->value(gui_files.c_str());
        pobdir->value(oDir.c_str());
        pbin->value(Bin.c_str());
        plib->value(libs.c_str());
        plibdir->value(libdirs.c_str());
        pincl->value(incdirs.c_str());
        pldflags->value(ldflags.c_str());
        pcflags->value(cflags.c_str());
        console_check->value(run_in_console);
        pr_own_makefile->value(own_makefile.c_str());
        own_make_chk->value(use_own_makefile);

        fclose(ptr);
        assigned = true;
        modified = false;
        return 1;
    }
    else
    {
        fl_alert(get_load_err_msg().c_str());
        return 0;
    }
}

