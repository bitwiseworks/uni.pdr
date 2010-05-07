@echo off
echo Cleaning.
@del uni.o
@del uni.pdr
@del .\en\uni.res
@del .\de\uni.res
echo Done.
echo Compiling.
gcc -o utils.o utils.c -c >build.log
gcc -o splpd.o splpd.c -c >>build.log
gcc -o uni.o uni.c -c     >>build.log
echo Done.
echo Compiling resources.
cd de
RC -r uni.rc uni.res      >>..\build.log
cd ..
echo Done.
echo Linking.
gcc -Zdll -Zbin-files -Zomf -o uni.pdr utils.o splpd.o uni.o uni.def .\de\uni.res >>build.log
echo Done.
echo Attaching EAS.
ea2 -e DEFAULT_PORT=UNI uni.pdr >>build.log
echo Done.
d:
cd \OS2\DLL
lxunlock uni.pdr
