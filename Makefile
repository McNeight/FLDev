# Standard defines:

CC	=	g++
LD	=	g++
CCOMP	=	gcc
oDir	=	.
Bin	=	.
libDirs	=	-L/usr/X11R6/lib
incDirs	=	-I/usr/X11R6/include
LD_FLAGS	=	
LIBS	=	-lX11 -lXpm -lfltk_images -lm -lfltk_forms -lfltk -lpng -lz -ljpeg -lutil
C_FLAGS	=	-Os
SRCS	=	editor.cpp highlight.cpp extras.cpp build_tools.cpp My_Text_Display.cpp My_Text_Editor.cpp NavBrowser.cpp My_File_Browser.cpp debug.cpp proj_form.cpp pref_form.cpp proj_wiz_form.cpp
EXOBJS	=	$(oDir)/editor.o $(oDir)/highlight.o $(oDir)/extras.o $(oDir)/build_tools.o $(oDir)/My_Text_Display.o $(oDir)/My_Text_Editor.o $(oDir)/NavBrowser.o $(oDir)/My_File_Browser.o $(oDir)/debug.o $(oDir)/proj_form.o $(oDir)/pref_form.o $(oDir)/proj_wiz_form.o 
ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/fldev
ALLTGT	=	$(Bin)/fldev

#@# Targets follow ---------------------------------
all:	$(ALLTGT)
objs:	$(ALLOBJS)
cleanobjs:
	rm -f $(ALLOBJS)
cleanbin:
	rm -f $(ALLBIN)
clean:	cleanobjs cleanbin
cleanall:	cleanobjs cleanbin

#@# Dependency rules follow -----------------------------
$(Bin)/fldev: $(EXOBJS)
	$(LD) -o $(Bin)/fldev $(EXOBJS) $(incDirs) $(libDirs) $(LD_FLAGS) $(LIBS)

$(oDir)/editor.o: editor.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/highlight.o: highlight.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/extras.o: extras.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/build_tools.o: build_tools.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/My_Text_Display.o: My_Text_Display.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/My_Text_Editor.o: My_Text_Editor.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/NavBrowser.o: NavBrowser.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/My_File_Browser.o: My_File_Browser.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/debug.o: debug.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/proj_form.o: proj_form.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/pref_form.o: pref_form.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<

$(oDir)/proj_wiz_form.o: proj_wiz_form.cpp extras.h globals.h EditorWindow.h highlight.h build_tools.h icon_pixmaps.h My_Text_Display.h My_Text_Editor.h NavBrowser.h My_File_Browser.h debug.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
