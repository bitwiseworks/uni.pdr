@echo off
@del uni.o
@del uni.pdr
@del .\de\uni.res
echo Done cleaning.
gcc -o uni.o uni.c -c >build.log
echo Done compiling.
cd de
RC -r uni.RC uni.RES 
cd ..
echo Done compiling resource.
gcc -Zdll -Zbin-files -Zomf -o uni.pdr uni.o uni.def .\de\uni.res >>build.log
echo Done linking.
ea2 -e DEFAULT_PORT=uni uni.pdr >>build.log
echo Done attaching EA.
rem d:
rem cd \OS2\DLL
rem lxunlock uni.pdr
