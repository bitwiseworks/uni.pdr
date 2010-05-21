#define  LINT_ARGS                     /* argument checking enabled          */


int acrtused=1;                      /* Define variable to say this is a DLL */

#include    "uni.h"

#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdarg.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <process.h>
#include    "utils.h"

MRESULT EXPENTRY CommDlg( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
	PUNIDATA    pUniData;
	ULONG       ulTimeOut = 0 ;
	CHAR        szDesc[ STR_LEN_PORTDESC ];
	CHAR        szShareName[ STR_LEN_PORTDESC ];
	CHAR        szTitle[ STR_LEN_TITLE + 1];
	CHAR        szTemp[ STR_LEN_PORTDESC ];
	CHAR        pwBuffer[256];
	CHAR        setupwiz[256];
	CHAR        setupparms[256];
	USHORT      i;
	ULONG       rc = 0;
	PUCHAR      token;
	BOOL        ulResult = MBID_ERROR;
	BOOL        fResult;
	FILEDLG     fd;
	CHAR        szMessage[ _MAX_PATH];
	CHAR        szDrive[ _MAX_PATH];
	CHAR        szDir[ _MAX_PATH];
	PSZ         pszButtonCode;
	PUCHAR      ON = "1";

	switch (msg)
	{
		case WM_INITDLG:
//			WinSendDlgItemMsg(hDlg,ID_BINARY,BM_SETCHECK,MPFROM2SHORT(1,0),NULL);
			WinSendDlgItemMsg(hDlg,ID_PROGRAM,   EM_SETTEXTLIMIT, MPFROMSHORT(128),NULL);
			WinSendDlgItemMsg(hDlg,ID_PARAMETERS,EM_SETTEXTLIMIT, MPFROMSHORT(128),NULL);
			WinSendDlgItemMsg(hDlg,ID_DIRECTORY, EM_SETTEXTLIMIT, MPFROMSHORT(128),NULL);

			pUniData = (PUNIDATA)mp2;

			WinSetWindowULong (hDlg, QWL_USER, (ULONG)pUniData);
			if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
										pUniData->pszAppName,
										KEY_DESCRIPTION,
										NULL,
										(PSZ)szDesc,
										STR_LEN_PORTDESC))
			{
				WinSetWindowText (WinWindowFromID (hDlg, (USHORT)IDD_UNI),szDesc);
				rc = WinLoadString(pUniData->hAB,
									pUniData->hModule,
									PDR_ID_PROPERTIES,
									STR_LEN_PORTDESC, szTemp);
				if (rc) {
					strcpy ( szTitle, pUniData->pszPortName );
					strcat ( szTitle, " - " );
					strcat ( szTitle, szTemp );
					WinSetWindowText (hDlg, szTitle);
				}
			}
			if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
										pUniData->pszAppName,
										KEY_INITIALIZATION,
										NULL,
										szTemp,
										sizeof(szTemp)))
			{
				i = 0;
				token = lprtok(szTemp,"#");
				while (token != NULL)
				{
					switch(i)
					{
						case 0:
							WinSetDlgItemText(hDlg,ID_PROGRAM,token);
						case 1:
							if (token[ strlen(token)-1 ] == ';')
								token[ strlen(token)-1 ] = '\0';
							WinSetDlgItemText(hDlg,ID_PARAMETERS,token);
							break;
						case 2:
							if (token[ strlen(token)-1 ] == ';')
								token[ strlen(token)-1 ] = '\0';
							WinSetDlgItemText(hDlg,ID_DIRECTORY,token);
							break;
						case 3:
							if (token[ strlen(token)-1 ] == ';')
								token[ strlen(token)-1 ] = '\0';
								
							if (strncmp(token, ON ,1) == 0) {
								WinSendDlgItemMsg(hDlg,ID_ADDPS,BM_SETCHECK,MPFROMSHORT(1),NULL);
							}
							else {
								WinSendDlgItemMsg(hDlg,ID_ADDPS,BM_SETCHECK,MPFROMSHORT(0),NULL);
							}
							break;
					}
					i++;
					token = lprtok(NULL,"#");
				}
			}
			break;

		case WM_COMMAND:
			pUniData = (PUNIDATA)WinQueryWindowULong (hDlg, QWL_USER);
			switch (SHORT1FROMMP(mp1))
			{
				case DID_OK:
					sprintf(szDesc," ");
					/* Program */
					WinQueryDlgItemText (hDlg, ID_PROGRAM, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,szTemp);
					strcat(pUniData->szSaveLprSetting,"#");

					if (strlen(szTemp) > 0) {
						strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
						strncat(szDesc, szTemp, STR_LEN_PORTDESC - 1);
					}
					/* strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
					strncat(szDesc, szShareName, STR_LEN_PORTDESC - 1); */
					
					/* Parameters */
					WinQueryDlgItemText (hDlg, ID_PARAMETERS, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,szTemp);
					strcat(pUniData->szSaveLprSetting,"#");

					if (strlen(szTemp) > 0) {
						strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
						strncat(szDesc, szTemp, STR_LEN_PORTDESC - 1);
					} 
					/* strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
					strncat(szDesc, szShareName, STR_LEN_PORTDESC - 1); */
					
					/* Working directory */
					WinQueryDlgItemText (hDlg, ID_DIRECTORY, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,szTemp);
					strcat(pUniData->szSaveLprSetting,"#");
					
					/* if (strlen(szTemp) > 0) {
						strncat(szShareName, "\\", STR_LEN_PORTDESC - 1);
						strncat(szShareName, szTemp, STR_LEN_PORTDESC - 1);
					} */
					
					/* Add PS header */
					if ((int) WinSendDlgItemMsg(hDlg,ID_ADDPS,BM_QUERYCHECK,NULL,NULL) == 1)
						strcat(pUniData->szSaveLprSetting,"1");
					else strcat(pUniData->szSaveLprSetting,"0");
					
					/* strcat(pUniData->szSaveLprSetting,"#"); */

					/* Username */
/*					WinQueryDlgItemText (hDlg, ID_USER, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,"#");
					strcat(pUniData->szSaveLprSetting,szTemp); */
					/* Number of copies */
/*					WinQueryDlgItemText (hDlg, ID_COPIES, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,"#");
					strcat(pUniData->szSaveLprSetting,szTemp); */
					/* Password - must be the last item! */
/*					WinQueryDlgItemText (hDlg, ID_PASSWORD, sizeof(szTemp), szTemp );
					strcat(pUniData->szSaveLprSetting,"#");
					strcpy(pwBuffer,szTemp);
					encryptPassword(pwBuffer,szTemp);
					strcat(pUniData->szSaveLprSetting,szTemp);
 */
					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												pUniData->pszAppName,
												KEY_INITIALIZATION,
												pUniData->szSaveLprSetting))
					WinDismissDlg(hDlg, MBID_CANCEL);

					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												APPNAME_PM_SPOOLER_PORT,
												pUniData->pszPortName,
												pUniData->szSaveLprSetting))
						WinDismissDlg(hDlg, MBID_CANCEL);

					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												pUniData->pszAppName,
												KEY_DESCRIPTION,
												szDesc ))
						WinDismissDlg(hDlg, MBID_CANCEL);

					WinDismissDlg(hDlg, TRUE);
					break;
				case DID_FIND:
					// setup data
					memset( &fd, 0, sizeof( fd));
					fd.cbSize    = sizeof( fd);
					fd.fl        = FDS_CENTER | FDS_HELPBUTTON | FDS_OPEN_DIALOG;
					fd.pszTitle  = "Find executable";
					
					WinQueryDlgItemText (hDlg, ID_PROGRAM, sizeof(szTemp), szTemp );
					if (strlen(szTemp) > 0) {
						_splitpath(szTemp, szDrive, szDir, NULL, NULL);
						strncat(szDrive, szDir, strlen(szDir) -1);
						strcat(szDrive,"\\*");
						strcpy( fd.szFullFile,szDrive);
					}
					else strcpy( fd.szFullFile, "C:\\*");

					// launch dialog
					fResult = WinFileDlg( HWND_DESKTOP, hDlg, &fd);
					if (!fResult)
						sprintf( szMessage, "error launching WinFileDlg\n");
					else if (fd.lReturn == DID_OK) {
						WinSetDlgItemText(hDlg,ID_PROGRAM,fd.szFullFile);
						_splitpath(fd.szFullFile, szDrive, szDir, NULL, NULL);
						strncat(szDrive, szDir, strlen(szDir) -1);
						WinSetDlgItemText(hDlg,ID_DIRECTORY,szDrive);
					}
					else
					    sprintf( szMessage, "error occurred!\n");
					break;
				case DID_SETUPWIZARD:
					// preliminary
					WinDismissDlg(hDlg, TRUE);
					sprintf(setupwiz,"unipdrcfg.exe");
					rc = spawnlp(P_NOWAIT,setupwiz,setupwiz,pUniData->pszPortName,NULL);
					// preliminary
					WinDismissDlg(hDlg, TRUE);
					break;
				case DID_CANCEL:
					WinDismissDlg(hDlg, MBID_CANCEL);
					break;
			}
			break;
		default:
			return WinDefDlgProc(hDlg, msg, mp1, mp2) ;
			break;
	}
return FALSE;
}

ULONG OpenUniPortDlg ( HAB hab,
						HMODULE hModule,
						PSZ pszPortName,
						PSZ pszAppName )
{
	UNIDATA UniData;

	memset (&UniData, 0, sizeof (UNIDATA));
	UniData.hAB = hab;
	UniData.hModule = hModule;
	UniData.pszPortName = pszPortName;
	UniData.pszAppName = pszAppName;

	WinDlgBox  (HWND_DESKTOP,
				HWND_DESKTOP,
				(PFNWP)CommDlg,
				(HMODULE)hModule,
				IDD_UNI,
				&UniData);

	return UniData.lfModified;
}
