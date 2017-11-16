Universal Port driver for OS/2 (ArcaOS, eCS)

Introduction

Universal Port Driver (uni.pdr) will eventually become the universal successor to smb.pdr, cups.pdr, ecups.pdr, eRedMan, PrintMon, lpr32.pdr, quePDF.

The principle behind all these solutions is to take data coming from an eCS (OS/2) printer port and pass it as a temporary file to a program as commandline parameter (plus additional parameters if these are required).

uni.pdr is based upon smb.pdr which is in turn based upon Serge Starck's lpr32.pdr (which is also the base cups.pdr). 