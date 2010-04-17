                           eCS (OS/2) Universal Port Driver 
                           Version _VERSION_
                           Build _BUILD_
                           ________________________________
                                  preliminary readme
CONTENTS
________

 1. Introduction
 2. License
 3. Žnderungshistorie


1. Introduction
_______________

The eCS (OS/2) Universal Port Driver (UNI.PDR) redirects the datastream 
from a printer port to a program. It has been tested to work with ePDF 
successfully, but should work with any program being capable to accept a 
file as an argument on the commandline.

uni.pdr is (like smb.pdr and cups.pdr) based upon Serge Starcks lpr32.pdr.
Unlike the above 2 pdrs, which have the program to service hardcoded into
the driver, both program and parameters may be configured individually in
uni.pdr.

When configuring the parameter field, make sure to have the %file% token
on the commandline as a placeholder where the pdr will put the real filename
during operation.

By and large uni.pdr has the same functionality that eRedMan provides. However
while eRedMan has a smarter GUI right now and some nice assistant to create
proper commandlines, uni.pdr is much more robust in operation. Especially
uni.pdr honors the order of the jobs being sent to the port, while eRedMan
does not.

2. License
__________

Same as smb.pdr, cups.pdr and lpr32.pdr. To be specified yet.


3. Žnderungshistorie
____________________

 v1.0 alpha1:
   - changelog created
   - got it working
