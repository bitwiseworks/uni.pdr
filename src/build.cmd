del smb.o
del smb.pdr
gcc -o smb.o smb.c -c
RC -r smb.RC smb.RES
gcc -Zdll -Zbin-files -Zomf -o smb.pdr smb.o smb.def smb.res
ea2 -e DEFAULT_PORT=SMB smb.pdr
rem d:
rem cd \OS2\DLL
rem lxunlock smb.pdr
