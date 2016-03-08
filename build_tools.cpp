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
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "globals.h"
#include "build_tools.h"

#include "My_File_Browser.h" 
#include "EditorWindow.h" 


using namespace std;


void generate_makefile_cb()
{
	if(!project.assigned) return;
	char buf[555];
	int BUFSIZE=555;
	char *homedir = getenv("HOME");
	sprintf(buf,"Makefile");
	FILE *ptr = fopen(buf,"w");
	
	if(!window->output->visible()) window->show_output();

  	outputtb->remove(0,outputtb->length());
  	op_stylebuf->remove(0,op_stylebuf->length());
  	outputtb->append("Generating Makefile...");

	
	string sourcefiles = "" + project.src_files;
	string conv_gui_files = "";
	char *gui_files = strdup(project.gui_files.c_str());
	char *result = strtok(gui_files," ");
	
	char *fname;
	
	FILE *fl_file;
	//integrate forms
	
	while(result)
	{
		fl_file = fopen(result,"r");
		if(fl_file)
		{
			//find extensions
			char line[100];
			string extension = ".cpp";

			while(fgets(line,100,fl_file))
			{
				int mode=0;
				for(int i = 0; i < strlen(line); i++)
				{
					if(mode == 0 && strncmp(line+i,"code_name",9)==0)
					{
						extension = "";
						mode = 1;
					}
					else if(mode == 1)
					{
						if(line[i]=='{') mode = 2;
					}
					else if(mode == 2)
					{
						if(line[i]=='}') 
						{
							mode = 3; 
							break;
						}
						else extension += line[i]; 
					}
				}
				if(mode == 3) break;
			}
			
			
			char *first = strsep(&result,".");
			if(first && extension != "") sourcefiles += " "+string(first)+extension;
			fclose(fl_file);
		}
		result = strtok(NULL," ");
	}
	
	
	char *src_files = strdup(sourcefiles.c_str());
	string exobjs;

		while(1)
		{			
			fname = strsep(&src_files," ");

			char *first = strsep(&fname,".");
			exobjs += "$(oDir)/";
			exobjs += first;
			exobjs += ".o ";

			if(src_files == NULL || src_files == "") break;
		}

	if(ptr != NULL) 
	{
#if MSDOS
		fprintf(ptr,"# Standard defines:\n\nCC\t=\tgpp\nLD\t=\tgpp\nCCOMP\t=\tgcc\n");
#else           
		fprintf(ptr,"# Standard defines:\n\nCC\t=\tg++\nLD\t=\tg++\nCCOMP\t=\tgcc\n");
#endif
		fprintf(ptr,"oDir\t=\t%s\n",	project.oDir.c_str());
		fprintf(ptr,"Bin\t=\t%s\n",		project.Bin.c_str());
		fprintf(ptr,"libDirs\t=\t%s\n",		project.libdirs.c_str());
		fprintf(ptr,"incDirs\t=\t%s\n",		project.incdirs.c_str());
		fprintf(ptr,"LD_FLAGS\t=\t%s\n",	project.ldflags.c_str());
		fprintf(ptr,"LIBS\t=\t%s\n",		project.libs.c_str());
		fprintf(ptr,"C_FLAGS\t=\t%s\n",		project.cflags.c_str());
		fprintf(ptr,"SRCS\t=\t%s\n",		sourcefiles.c_str());
		fprintf(ptr,"EXOBJS\t=\t%s\n",		exobjs.c_str());
		fprintf(ptr,"ALLOBJS\t=\t%s\n",		"$(EXOBJS)");
		fprintf(ptr,"ALLBIN\t=\t$(Bin)/%s\n",		project.binfilename.c_str());
		fprintf(ptr,"ALLTGT\t=\t$(Bin)/%s\n",		project.binfilename.c_str());

		fprintf(ptr,"\n#@# Targets follow ---------------------------------\n");
		fprintf(ptr,"all:\t$(ALLTGT)\n");
		fprintf(ptr,"objs:\t$(ALLOBJS)\n");
		fprintf(ptr,"cleanobjs:\n\trm -f $(ALLOBJS)\n");
		fprintf(ptr,"cleanbin:\n\trm -f $(ALLBIN)\n");
		fprintf(ptr,"clean:\tcleanobjs cleanbin\n");
		fprintf(ptr,"cleanall:\tcleanobjs cleanbin\n");

		fprintf(ptr,"\n#@# Dependency rules follow -----------------------------\n");

#if MSDOS
    if (project.Bin=="." || project.Bin=="") {
		fprintf(ptr,"%s: $(EXOBJS)\n",	project.binfilename.c_str());
		fprintf(ptr,"\t$(LD) $(LD_FLAGS) -o %s.exe $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)\n",	project.binfilename.c_str());                    
     } else {
		fprintf(ptr,"$(Bin)/%s: $(EXOBJS)\n",	project.binfilename.c_str());
		fprintf(ptr,"\t$(LD) $(LD_FLAGS) -o $(Bin)/%s.exe $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)\n",	project.binfilename.c_str());
    }
#else		
		fprintf(ptr,"$(Bin)/%s: $(EXOBJS)\n",	project.binfilename.c_str());
		fprintf(ptr,"\t$(LD) $(LD_FLAGS) -o $(Bin)/%s $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)\n",	project.binfilename.c_str());
#endif
		char *src_files = strdup(sourcefiles.c_str());
		char *fname;

		while(1)
		{			
			fname = strsep(&src_files," ");

			char *first = strsep(&fname,".");
			char *ext = strsep(&fname,"\n");
#if MSDOS
     if (project.oDir=="." || project.oDir=="") {
            fprintf(ptr,"\n%s.o: %s.%s %s\n",		first,first,ext,project.header_files.c_str());
     } else {
			fprintf(ptr,"\n$(oDir)/%s.o: %s.%s %s\n",		first,first,ext,project.header_files.c_str());            
     }
#else
			fprintf(ptr,"\n$(oDir)/%s.o: %s.%s %s\n",		first,first,ext,project.header_files.c_str());
#endif
			//fprintf(ptr,"\n$(oDir)/%s.o: %s.%s %s Makefile\n",		first,first,ext,project.header_files.c_str());
			fprintf(ptr,"\t$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<\n");
			if(src_files == NULL || src_files == "") break;
		}
	
		free(src_files);
		fclose(ptr);
	  	outputtb->append("OK");
	}
	free(src_files);
	free(gui_files);
}

