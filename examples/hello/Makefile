# Standard defines:

CC	=	gpp
LD	=	gpp
CCOMP	=	gcc
oDir	=	.
Bin	=	.
libDirs	=	
incDirs	=	
LD_FLAGS	=	-g
LIBS	=	
C_FLAGS	=	-g
SRCS	=	main.cpp
EXOBJS	=	$(oDir)/main.o 
ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/hello
ALLTGT	=	$(Bin)/hello

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
hello: $(EXOBJS)
	$(LD) $(LD_FLAGS) -o hello.exe $(EXOBJS) $(incDirs) $(libDirs) $(LIBS)

main.o: main.cpp 
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
