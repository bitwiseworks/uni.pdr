#include <version.h>
                      Universal Port Driver fr <$OS>
                      Version <$UNIPDR_VERSION> Build <$UNIPDR_BUILD>
                      ________________________________
                                  Vorl„ufiges Readme
INHALT
________

ÿ1. Einleitung
ÿ2. Lizenz
ÿ3. nderungshistorie


1. Einleitung
_______________

Der Universal Port Driver (UNI.PDR) fr OS/2 leitet den Datenstrom von einem Drucker-Port 
zu einem Programm weiter. Er wurde mit folgenden Programmen getestet:

ÿÿ- ePDF.exe
ÿÿ- smbspool.exe
ÿÿ
Er sollte mit jedem Programm arbeiten, das in der Lage ist, eine Datei als Argument auf 
der Kommandozeile zu akzeptieren.

UNI.PDR basiert (wie SMB.PDR und CUPS.PDR) auf Serge Starcks lpr32.PDR.
Im Gegensatz zu den oben genannten 2 pdrs, die das Programm im Treiber hart codiert haben,
erlaubt UNI.PDR Programm, Parameter und Arbeitsverzeichnis zu sein individuell zu konfigurieren.

Wenn Sie das Parameterfeld konfigurieren, stellen Sie sicher, dass ein %file% Token vorhanden ist
auf der Kommandozeile als Platzhalter, an den der pdr den richtigen Dateinamen setzen wird
w„hrend der Operation.

Im groáen und ganzen hat UNI.PDR hat die gleiche Funktionalit„t, die eRedMan bietet. aber
w„hrend eRedMan eine intelligentere GUI bietet und einige nette Assistenten bietet um die 
richtigen Kommandozeilenparameter anzulegen, ist uni.PDR viel robuster im Betrieb. Insbesondere
bercksichtigt uni.PDR die Reihenfolge der Auftr„ge, die an den Anschluss geschickt werden, 
w„hrend eRedMan dies nicht tut.


2. LIZENZ
__________

Wie smb.PDR, cups.PDR und lpr32.PDR. .


3. nderungshistorie
____________________

#include <changelog.txt>
