#run this in MSYS
echo 101 ICON DISCARDABLE "fldev.ico" >ico.rc
windres ico.rc icores.o
#now link icores.o with the fldev.o files to fldev.exe
