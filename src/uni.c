#define  LINT_ARGS                     /* argument checking enabled          */

#define  INCL_DOS
#define  INCL_GPI
#undef   INCL_GPI
#define  INCL_DEV
#define  INCL_DOSMEMMGR                /* Include standard OS/2 support      */
#define  INCL_DOSMODULEMGR             /* For DosLoadModule                  */
#define  INCL_DOSPROCESS
#define  INCL_GPILCIDS
#define  INCL_WINCOMMON                /* Include Window Management support  */
#define  INCL_WINDOWMGR
#define  INCL_WINSWITCHLIST
#define  INCL_WINPROGRAMLIST
#define  INCL_WINMENUS
#define  INCL_WINWINDOWMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINDIALOGS
#define  INCL_WINSTATICS
#define  INCL_WINLISTBOXES
#define  INCL_WINMENUS
#define  INCL_WINSYS
#define  INCL_WINFRAMEMGR
#define  INCL_INCLWINACCELERATORS
#define  INCL_WINPOINTERS
#define  INCL_WINERRORS
#define  INCL_WINSHELLDATA

#define  INCL_WINTYPES
#define  INCL_WINACCELERATORS
#define  INCL_WINBUTTONS
#define  INCL_WINENTRYFIELDS
#define  INCL_WINRECTANGLES
#define  INCL_WINTIMER
#define  INCL_WINSCROLLBARS
#define  INCL_WINHEAP
#define  INCL_SHLERRORS
#define  INCL_WININPUT
#define  INCL_WINHELP
#define  INCL_WINSTDSPIN

#define  INCL_SPL
#define  INCL_SPLP
#define  INCL_SPLERRORS
#define  INCL_SHLERRORS
#define  INCL_DOSERRORS
#define  INCL_WINHOOKS

int acrtused=1;                      /* Define variable to say this is a DLL */

#include    <os2.h>

#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdarg.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <process.h>
#include    "uni.h"
#include    "utils.h"

//
// If port driver is not defined in INI file yet
//   assume it exists in the boot drive's \OS2\DLL directory
//

CHAR szDefaultPortDrvPath[] = { PATH_UNI_PDR };


//
// Below definition of PORTNAMES structure should be defined in
// common header file pmspl.h.
//

typedef struct _PORTNAMES
{
	PSZ pszPortName;         /* -> name of port(ie "LPT1)                    */
	PSZ pszPortDesc;         /* -> description of port(ie "Parallel Port 1") */
} PORTNAMES, *PPORTNAMES;

/* Alex Taylor's new version of lprtok() that can handle missing fields */
char * lprtok (char *string,char *control)
{
	char *c;
	static char *next;

	if ( control == NULL ) return string;
	if ( string  == NULL ) string = next;
	if ( string  == NULL ) return NULL;

	if (( c = strpbrk( string, control )) == NULL ) {
		next = NULL;
	}
	else {
		next = c+1;
		*c = '\0';
	}

	return ( string );
}
MRESULT EXPENTRY CommDlg( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
	PLPRDATA    pLprData;
	ULONG       ulTimeOut = 0 ;
	CHAR        szDesc[ STR_LEN_PORTDESC ];
	CHAR        szShareName[ STR_LEN_PORTDESC ];
	CHAR        szTitle[ STR_LEN_TITLE + 1];
	CHAR        szTemp[ STR_LEN_PORTDESC ];
	CHAR        pwBuffer[256];
	USHORT      i;
	ULONG       rc = 0;
	PUCHAR      token;

	switch (msg)
	{
		case WM_INITDLG:
//			WinSendDlgItemMsg(hDlg,ID_BINARY,BM_SETCHECK,MPFROM2SHORT(1,0),NULL);
			pLprData = (PLPRDATA)mp2;
			WinSetWindowULong (hDlg, QWL_USER, (ULONG)pLprData);
			if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
										pLprData->pszAppName,
										KEY_DESCRIPTION,
										NULL,
										(PSZ)szDesc,
										STR_LEN_PORTDESC))
			{
				WinSetWindowText (WinWindowFromID (hDlg, (USHORT)IDD_UNI),szDesc);
				rc = WinLoadString(pLprData->hAB,
									pLprData->hModule,
									PDR_ID_PROPERTIES,
									STR_LEN_PORTDESC, szTemp);
				if (rc) {
					strcpy ( szTitle, pLprData->pszPortName );
					strcat ( szTitle, " - " );
					strcat ( szTitle, szTemp );
					WinSetWindowText (hDlg, szTitle);
				}
			}
			if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
										pLprData->pszAppName,
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
							if (token[ strlen(token) - 1 ] == ';')
								token[ strlen(token)-1 ] = '\0';
							WinSetDlgItemText(hDlg,ID_PARAMETERS,token);
							break;
						case 2:
							if (token[ strlen(token) - 1 ] == ';')
								token[ strlen(token)-1 ] = '\0';
							WinSetDlgItemText(hDlg,ID_DIRECTORY,token);
							break;
					}
					i++;
					token = lprtok(NULL,"#");
				}
			}
			break;

		case WM_COMMAND:
			pLprData = (PLPRDATA)WinQueryWindowULong (hDlg, QWL_USER);
			switch (SHORT1FROMMP(mp1))
			{
				case DID_OK:
					sprintf(szDesc," ");
					/* Program */
					WinQueryDlgItemText (hDlg, ID_PROGRAM, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,szTemp);
					strcat(pLprData->szSaveLprSetting,"#");

					if (strlen(szTemp) > 0) {
						strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
						strncat(szDesc, szTemp, STR_LEN_PORTDESC - 1);
					}
					/* strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
					strncat(szDesc, szShareName, STR_LEN_PORTDESC - 1); */
					
					/* Parameters */
					WinQueryDlgItemText (hDlg, ID_PARAMETERS, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,szTemp);
					strcat(pLprData->szSaveLprSetting,"#");

					if (strlen(szTemp) > 0) {
						strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
						strncat(szDesc, szTemp, STR_LEN_PORTDESC - 1);
					} 
					/* strncat(szDesc, " ", STR_LEN_PORTDESC - 1);
					strncat(szDesc, szShareName, STR_LEN_PORTDESC - 1); */
					
					/* Working directory */
					WinQueryDlgItemText (hDlg, ID_DIRECTORY, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,szTemp);
					/* strcat(pLprData->szSaveLprSetting,"#"); */
					
					/* if (strlen(szTemp) > 0) {
						strncat(szShareName, "\\", STR_LEN_PORTDESC - 1);
						strncat(szShareName, szTemp, STR_LEN_PORTDESC - 1);
					} */
					/* Username */
/*					WinQueryDlgItemText (hDlg, ID_USER, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,"#");
					strcat(pLprData->szSaveLprSetting,szTemp); */
					/* Number of copies */
/*					WinQueryDlgItemText (hDlg, ID_COPIES, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,"#");
					strcat(pLprData->szSaveLprSetting,szTemp); */
					/* Password - must be the last item! */
/*					WinQueryDlgItemText (hDlg, ID_PASSWORD, sizeof(szTemp), szTemp );
					strcat(pLprData->szSaveLprSetting,"#");
					strcpy(pwBuffer,szTemp);
					encryptPassword(pwBuffer,szTemp);
					strcat(pLprData->szSaveLprSetting,szTemp);
 */
					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												pLprData->pszAppName,
												KEY_INITIALIZATION,
												pLprData->szSaveLprSetting))
					WinDismissDlg(hDlg, MBID_CANCEL);

					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												APPNAME_PM_SPOOLER_PORT,
												pLprData->pszPortName,
												pLprData->szSaveLprSetting))
						WinDismissDlg(hDlg, MBID_CANCEL);

					if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
												pLprData->pszAppName,
												KEY_DESCRIPTION,
												szDesc ))
						WinDismissDlg(hDlg, MBID_CANCEL);

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
ULONG CalcStructLength ( HAB hab,
						HMODULE hModule,
						USHORT usID )
{
	ULONG cbRequired;
	CHAR  chString[STR_LEN_PORTDESC];

	cbRequired = 0;

	WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, chString);
	cbRequired += strlen (chString) + 1;
	WinLoadString(hab, hModule, (USHORT)(usID + 1), STR_LEN_PORTDESC, chString);
	cbRequired += strlen (chString) + 1;
	cbRequired += sizeof (PORTNAMES);
	return(cbRequired);
}

ULONG CalcBufLength ( HAB hab,
						HMODULE hModule )
{
	ULONG  cbRequired;
	USHORT usID;

	cbRequired = 0;

		/*
		** calculate length required to fit all the port info.
		*/
	for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
	{
		cbRequired += CalcStructLength (hab, hModule, usID);
	}

	return(cbRequired);
}

ULONG NumPortsCanFit ( HAB hab,
						HMODULE hModule,
						ULONG cbBuf )
{
	ULONG  cbRequired;
	USHORT usID;
	ULONG  ulNumPort;

	cbRequired = 0;
	ulNumPort = 0;

		/*
		** calculate how many ports we can fit in buf.
		*/
	for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
	{
		cbRequired += CalcStructLength (hab, hModule, usID);
		if (cbRequired > cbBuf)
		{
		return(ulNumPort);
		}
		ulNumPort++;
	}

	return(ulNumPort);
}
VOID CopyStruct ( HAB hab,
					HMODULE hModule,
					USHORT usID,
					PCH pBuf,
					PULONG pulBeginStruct,
					PULONG pulBeginText )
{
	PPORTNAMES pPortNames;

	pPortNames = (PPORTNAMES)(pBuf + *pulBeginStruct);
	*pulBeginStruct += sizeof (PORTNAMES);

		/*
		** copy port name in the structure
		*/
	pPortNames->pszPortName = pBuf + *pulBeginText;
	WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, pPortNames->pszPortName);
	*pulBeginText += strlen (pPortNames->pszPortName) + 1;

		/*
		** copy port description to the structure
		*/
	pPortNames->pszPortDesc = pBuf + *pulBeginText;
	WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, pPortNames->pszPortDesc);
	*pulBeginText += strlen (pPortNames->pszPortDesc) + 1;
}
VOID  CopyNPorts ( HAB hab,
					HMODULE hModule,
					PCH pBuf,
					ULONG ulReturned )
{
	USHORT usID;
	ULONG  ulBeginText;
	ULONG  ulBeginStruct;

	ulBeginText = ulReturned * sizeof (PORTNAMES);
	ulBeginStruct = 0;

	for (usID = PORT_ID_FIRST;
		usID <= PORT_ID_LAST && ulReturned;
		usID += 2, --ulReturned)
	{
		CopyStruct (hab, hModule, usID, pBuf, &ulBeginStruct, &ulBeginText);
	}
}

ULONG OpenLprPortDlg ( HAB hab,
						HMODULE hModule,
						PSZ pszPortName,
						PSZ pszAppName )
{
	LPRDATA LprData;

	memset (&LprData, 0, sizeof (LPRDATA));
	LprData.hAB = hab;
	LprData.hModule = hModule;
	LprData.pszPortName = pszPortName;
	LprData.pszAppName = pszAppName;

	WinDlgBox  (HWND_DESKTOP,
				HWND_DESKTOP,
				(PFNWP)CommDlg,
				(HMODULE)hModule,
				IDD_UNI,
				&LprData);

	return LprData.lfModified;
}
BOOL GetPortDescription ( HAB hab,
							HMODULE hModule,
							PSZ pszPortName,
							PSZ pszPortDesc )
{
	USHORT usID;
	CHAR   chBuf[STR_LEN_PORTDESC] = {0};

	for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
	{
		WinLoadString(hab, hModule, usID, STR_LEN_PORTNAME, chBuf);
		if (!strcmp (pszPortName, chBuf))
		{
		if ( WinLoadString(hab, hModule, usID+1, STR_LEN_PORTDESC, chBuf) ) {
			strcpy (pszPortDesc, chBuf);
			return(TRUE);
		}
		break;
		}
	}
	return(FALSE);
}
BOOL  GenerateUniquePortName( PSZ pszPortName )
{
	BOOL   fPortExists;
	PSZ    pszEndPortName;
	USHORT i;
	CHAR   chPortData[STR_LEN_PORTNAME];

	/*
	** Generate a unique port name by adding numbers to the
	**   end of pszPortName
	*/
	pszEndPortName = pszPortName + strlen( pszPortName );
	i = 1;
	fPortExists = TRUE;
	while ( (i < MAX_PORTS) && fPortExists )
	{
		_itoa( i, pszEndPortName, 4);
		fPortExists = PrfQueryProfileString (HINI_SYSTEMPROFILE,
											APPNAME_PM_SPOOLER_PORT,
											pszPortName,
											NULL,
											chPortData,
											sizeof(chPortData) - 1);
		i++;
	}
	return(!fPortExists);
}
APIRET APIENTRY SplPdEnumPort ( HAB hab,
								PVOID pBuf,
								ULONG cbBuf,
								PULONG pulReturned,
								PULONG pulTotal,
								PULONG pcbRequired )

{
	HMODULE hModule;
	ULONG   ulBootDrive;
	ULONG   rcLoadMod;
	CHAR     szPathName[260];

	if (!pulReturned ||
		!pulTotal ||
		!pcbRequired)
	{
		return(ERROR_INVALID_PARAMETER);
	}

	if (!pBuf && cbBuf)
	{
		return(ERROR_INVALID_PARAMETER);
	}

	DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
					sizeof (ULONG));
	szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

	PrfQueryProfileString (HINI_SYSTEMPROFILE,
							"PM_PORT_DRIVER",
							"UNI",
							szDefaultPortDrvPath,
							szPathName,
							256 );

	rcLoadMod = DosLoadModule (NULL, 0, szPathName, &hModule);

	if (cbBuf == 0L)
	{
		*pulReturned = 0;
		*pcbRequired = CalcBufLength (hab, hModule);
		*pulTotal = MAX_PORTS;
		if (!rcLoadMod)
		DosFreeModule (hModule);
		return(ERROR_MORE_DATA);
	}

		/*
		** check number of ports info we can fit in supplied buffer
		*/
	*pulTotal    = MAX_PORTS;
	*pcbRequired = CalcBufLength (hab, hModule);
	*pulReturned = NumPortsCanFit (hab, hModule, cbBuf);

		/*
		** return error if we can not fit one port.
		*/
	if (!(*pulReturned))
	{
		if (!rcLoadMod)
		DosFreeModule (hModule);
		return(ERROR_INSUFFICIENT_BUFFER);
	}

		/*
		** copy all the ports which we can store in the pBuf
		*/
	CopyNPorts (hab, hModule, (PCH)pBuf, *pulReturned);

		/*
		** Free the module - we do not need it any more.
		*/
	if (!rcLoadMod)
		DosFreeModule (hModule);

		/*
		** copy all the ports which we can store in the pBuf
		*/
	if (*pulReturned < *pulTotal)
	{
		return(ERROR_MORE_DATA);
	}

	return(NO_ERROR);
}
APIRET APIENTRY SplPdInstallPort ( HAB hab,
									PSZ pszPortName )
{
	CHAR    chBuf[STR_LEN_PORTNAME];
	CHAR    chPortDesc[STR_LEN_PORTDESC];
	ULONG   ulBootDrive;
	HMODULE hModule;
	CHAR    szPathName[260];    /* will contain full path to this port driver */

	if (!pszPortName)
	{
		return(ERROR_INVALID_PARAMETER);
	}
	strcpy(szDefaultPortDrvPath,PATH_UNI_PDR);
	DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
					sizeof (ULONG));
	szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);
	strcpy( szPathName, szDefaultPortDrvPath );

	/* Make sure the port driver itself is installed */
	PrfWriteProfileString (HINI_SYSTEMPROFILE,
							"PM_PORT_DRIVER",
							"UNI",
							szDefaultPortDrvPath);

	/* Generate appname for "PM_UNIx" */
	strcpy (chBuf, APPNAME_LEAD_STR);
	strcat (chBuf, pszPortName);

	/*
	** Check for this being our default Port Name to install.
	** If so (pszPortName == "UNI") then generate a unique
	**   port name so that we can install multiple UNI printers.
	*/
	if (!strcmp(pszPortName, DEF_PORTNAME))
	{
		/*
		** Use chBuf to store the new portname to install
		** Must first increment past "PM_" in chBuf
		*/
		pszPortName = chBuf + 3;
		GenerateUniquePortName( pszPortName );
	}

	/* Get initial port description (fall back to portname if unavailable) */
	hModule = 0L ;                            /* Init module handle to null */
	DosLoadModule (NULL, 0, szPathName, &hModule);
	if (!GetPortDescription (hab, hModule, pszPortName, chPortDesc))
	{
		strcpy( chPortDesc, pszPortName );
	}
	DosFreeModule (hModule);

	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								chBuf,
								KEY_DESCRIPTION,
								chPortDesc))
	{
		return (WinGetLastError (hab));
	}

	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								chBuf,
								KEY_INITIALIZATION,
								DEF_INITIALIZATION))
	{
		return (WinGetLastError (hab));
	}

	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								chBuf,
								KEY_TERMINATION,
								DEF_TERMINATION))
	{
		return (WinGetLastError (hab));
	}

	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								chBuf,
								KEY_PORTDRIVER,
								DEF_PORTDRIVER))
	{
		return (WinGetLastError (hab));
	}

	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								chBuf,
								KEY_TIMEOUT,
								DEF_TIMEOUT))
	{
		return (WinGetLastError (hab));
	}
	if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
								APPNAME_PM_SPOOLER_PORT,
								pszPortName,
								DEF_INITIALIZATION))
	{
		return (WinGetLastError (hab));
	}
	return(NO_ERROR);
}
BOOL   APIENTRY SplPdGetPortIcon ( HAB hab,
									PULONG idIcon )
{
	if (idIcon)
	{
		*idIcon = UNI_ICON;
	}
	return(TRUE);
}
APIRET APIENTRY SplPdQueryPort ( HAB hab,
								PSZ pszPortName,
								PVOID pBufIn,
								ULONG cbBuf,
								PULONG cItems )
{
	HMODULE hModule;
	CHAR    chString[STR_LEN_DESC];
	USHORT  usNumLines;
	USHORT  usLineID;
	USHORT  usStrLength;
	ULONG   ulBootDrive;
	PCH     pBuf = pBufIn;
	CHAR    szPathName[260];    /* will contain full path to this port driver */

	if (!cItems)
	{
		return(ERROR_INVALID_PARAMETER);
	}

	if (!pBuf || !cbBuf)
	{
		return(ERROR_INVALID_PARAMETER);
	}


	DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
					sizeof (ULONG));
	szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

	PrfQueryProfileString (HINI_SYSTEMPROFILE,
							"PM_PORT_DRIVER",
							"UNI",
							szDefaultPortDrvPath,
							szPathName,
							256 );

	hModule = 0L ;

	DosLoadModule (NULL, 0, szPathName, &hModule);

	chString[0] = '\0' ;

	WinLoadString(hab, hModule, (USHORT)ID_NUMBER_OF_DESC_LINES, STR_LEN_DESC,
				chString);
	usNumLines = (USHORT)atoi (chString);
	usLineID = ID_FIRST_DESC_LINES;
	for (*cItems = 0; *cItems < usNumLines; *cItems++)
	{
		WinLoadString(hab, hModule, usLineID, STR_LEN_DESC, chString);
		if (cbBuf >= (usStrLength = (USHORT)(strlen (chString) + 1)))
		{
		strcpy (pBuf, chString);
		pBuf += usStrLength;
		cbBuf -= usStrLength;
		}
		else
		{
		DosFreeModule (hModule);
		return(ERROR_INSUFFICIENT_BUFFER);
		}
	}
	DosFreeModule (hModule);
	return(NO_ERROR);
}
APIRET APIENTRY SplPdSetPort ( HAB hab,
								PSZ pszPortName,
								PULONG flModified )
{
	CHAR    chBuf[STR_LEN_PORTNAME];
	CHAR    chPortDriver[STR_LEN_PORTNAME];
	ULONG   ulBootDrive;
	HMODULE hModule;
	CHAR    szPathName[260];   /* will contain full path to this port driver */

	if (!pszPortName || !flModified)
	{
		return(ERROR_INVALID_PARAMETER);
	}

	strcpy (chBuf, APPNAME_LEAD_STR);
	strcat (chBuf, pszPortName);

	if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, chBuf,
								KEY_PORTDRIVER, NULL, chPortDriver,
								STR_LEN_PORTNAME)))
	{
		return(ERROR_INVALID_PARAMETER);
	}

	if (strcmp (chPortDriver, DEF_PORTDRIVER))
	{
		return(ERROR_INVALID_PARAMETER);
	}

	DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
					sizeof (ULONG));
	szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

	PrfQueryProfileString (HINI_SYSTEMPROFILE,
							"PM_PORT_DRIVER",
							"UNI",
							szDefaultPortDrvPath,
							szPathName,
							256 );

	hModule = 0L ;                               /* Init module handle to null */

	DosLoadModule (NULL, 0, szPathName, &hModule);

	*flModified = OpenLprPortDlg (hab, hModule, pszPortName, chBuf);

	DosFreeModule (hModule);
	return(NO_ERROR);
}


APIRET APIENTRY SplPdRemovePort ( HAB hab,
									PSZ pszPortName )
{
	CHAR chBuf[STR_LEN_PORTNAME];
	CHAR chPortDriver[STR_LEN_PORTNAME];

	if (!pszPortName)
	{
		return(ERROR_INVALID_PARAMETER);
	}

	strcpy (chBuf, APPNAME_LEAD_STR);
	strcat (chBuf, pszPortName);

	if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, chBuf,
								KEY_PORTDRIVER, NULL, chPortDriver,
								STR_LEN_PORTNAME)))
	{
		return(ERROR_INVALID_PARAMETER);
	}

	if (strcmp (chPortDriver, DEF_PORTDRIVER))
	{
		return(ERROR_INVALID_PARAMETER);
	}

	PrfWriteProfileString (HINI_SYSTEMPROFILE, chBuf, NULL, NULL);

	PrfWriteProfileString (HINI_SYSTEMPROFILE,
							APPNAME_PM_SPOOLER_PORT,
							pszPortName,
							NULL);
	return(NO_ERROR);

}
ULONG APIENTRY SplPdOpen( PSZ     pszPortName,
							PHFILE  phFile,
							PULONG  pDeviceFlags,
							PVOID   pPrtOpenStruct)
{
	APIRET rc;
	ULONG  ulAction       = 0;      /* Action taken by DosOpen */
	ULONG  ulBytesRead    = 0;      /* Number of bytes read by DosRead */
	ULONG  ulWrote       = 0;      /* Number of bytes written by DosWrite */
	ULONG  ulLocal       = 0;      /* File pointer position after DosSetFilePtr */
	char    szTemp[ 256];
	char    tmp[256];
	ULONG  pcbWritten ;
	USHORT  i;
	HFILE   control;
	char  filename[256];
	DATETIME dt;
	char  spool_dir[256];
	PEAOP2 pEABuf = NULL;


	if (!phFile || !pDeviceFlags )
	{
		return(ERROR_INVALID_PARAMETER);
	}
	DosGetDateTime(&dt);
	rc = PrfQueryProfileString (HINI_SYSTEMPROFILE,
								"PM_SPOOLER",
								"DIR",
								NULL,
								(PSZ)spool_dir,
								sizeof(spool_dir));
	spool_dir[ strlen(spool_dir) - 1] = '\0';
	sprintf(tmp,"%s\\UNI",spool_dir);
	DosCreateDir( tmp,pEABuf );
	sprintf(filename,"%s\\UNI\\%02d_%02d_%02d_%02d_%s",spool_dir,dt.hours,dt.minutes,dt.seconds,dt.hundredths,pszPortName);
	rc = DosOpen(filename,
				phFile,                       /* File handle */
				&ulAction,                     /* Action taken */
				100L,                           /* File primary allocation */
				FILE_ARCHIVED | FILE_NORMAL,    /* File attribute */
				OPEN_ACTION_CREATE_IF_NEW |
				OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
				OPEN_FLAGS_NOINHERIT |
				OPEN_SHARE_DENYNONE  |
				OPEN_ACCESS_READWRITE,         /* Open mode of the file */
				0L);                         /* No extended attribute */
/*  DosWrite(*phFile,pszPSHeader,strlen(pszPSHeader),&cbHeader);  */
	sprintf(szTemp,"PM_%s",pszPortName);
	if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
							szTemp,
							KEY_INITIALIZATION,
							NULL,
							szTemp,
							sizeof(szTemp)))
	{
		sprintf(tmp   ,"%s\\UNI\\%d.control",spool_dir,*phFile);
		rc = DosOpen( tmp    ,
					&control,                         /* File handle */
					&ulAction,                     /* Action taken */
					strlen(szTemp),              /* File primary allocation */
					FILE_ARCHIVED | FILE_NORMAL,    /* File attribute */
					OPEN_ACTION_CREATE_IF_NEW |
					OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
					OPEN_FLAGS_NOINHERIT |
					OPEN_SHARE_DENYNONE  |
					OPEN_ACCESS_READWRITE,         /* Open mode of the file */
					0L);                         /* No extended attribute */
	rc = DosWrite( control,szTemp,strlen(szTemp),&pcbWritten);
	rc = DosWrite( control,"#",1,&pcbWritten);
	rc = DosWrite( control,filename,strlen(filename),&pcbWritten);
	rc = DosWrite( control,"@",1,&pcbWritten);
	rc = DosClose(control);
	}
return rc;

}
ULONG  APIENTRY SplPdQuery ( PSZ    pszDeviceName,
							ULONG  ulFlags,
							ULONG  ulCommand,
							PVOID  pInData,
							ULONG  cbInData,
							PVOID  pOutData,
							PULONG pcbOutData )
{
/*  return ERROR_NOT_SUPPORTED; */
	return NO_ERROR;
}
ULONG APIENTRY SplPdSet ( PSZ    pszDeviceName,
							ULONG  ulFlags,
							ULONG  ulCommand,
							PVOID  pInData,
							ULONG  cbInData )
{
/* return ERROR_NOT_SUPPORTED; */
	return NO_ERROR;
}
ULONG  APIENTRY SplPdNewPage ( HFILE  hFile, ULONG ulPageNumber )
{
	return NO_ERROR;
}
ULONG APIENTRY SplPdAbortDoc( HFILE   hFile,
								PVOID   pchData,
								ULONG   cbData,
								ULONG   ulFlags )
{
	return NO_ERROR;
}
ULONG  APIENTRY SplPdClose( HFILE  hFile )
{
	APIRET      rc;
	APIRET      resp;
	USHORT      i;
	USHORT      j;
	RESULTCODES rc_child;
	ULONG       nbr_lu;
	ULONG       ulAction;
	char       szTemp[256];
	HFILE       control;
	char       binfile[256];
	char       arg[256];
	char       j_parms[256] ;
	char         *f_parms;
	char       j_id[3];
	char       parameters[256];
	char       workingdir[256] ;
	char       j_title[256];
	char       j_copies[3];
	char       j_options[8];
	char       filename[256];
	char       ip_add[256];
	char       queue_name[256];
	char       workgroup[256];
	char       username[256];
	char       password_enc[256];
	char       password_dec[256];
	char       errorstr[256];
	USHORT      pos;
	char       spool_dir[256];
	ULONG       ulBootDrive;

	rc = PrfQueryProfileString (HINI_SYSTEMPROFILE,
								"PM_SPOOLER",
								"DIR",
								NULL,
								(PSZ)spool_dir,
								sizeof(spool_dir));
	spool_dir[ strlen(spool_dir) - 1] = '\0';
	sprintf(szTemp,"%s\\UNI\\%d.control",spool_dir,hFile);
	rc = DosOpen(szTemp,
				&control,
				&ulAction,
				0L,
				FILE_ARCHIVED | FILE_NORMAL,
				OPEN_ACTION_CREATE_IF_NEW |
				OPEN_ACTION_OPEN_IF_EXISTS,
				OPEN_FLAGS_NOINHERIT |
				OPEN_SHARE_DENYNONE  |
				OPEN_ACCESS_READWRITE,
				0L);
	rc = DosRead( control,szTemp,sizeof(szTemp),&nbr_lu);
	rc = DosClose( control );
	sprintf(filename,"%s\\UNI\\%d.control",spool_dir,hFile);
	DosDelete(filename);

	i = 0;
	j = 0;
	pos = 0;
	while (szTemp[i] != '@')
	{
		if (szTemp[i] == '#')
		{
			szTemp[i] = '\0';
			switch(j)
			{
				case 0:strcpy(binfile,&szTemp[pos]);
						break;
				case 1:strcpy(parameters,&szTemp[pos]);
						break;
				case 2:strcpy(workingdir,&szTemp[pos]);
						break;
/*				case 3:strcpy(username,&szTemp[pos]);
						break;
				case 4:strcpy(j_copies,&szTemp[pos]);
						break;
				case 5:strcpy(password_enc,&szTemp[pos]);
						break; */
			}
			pos = i+1;
			j++;
		}
		i++;
	}
	szTemp[i] = '\0';
	strcpy(filename,&szTemp[pos]);

	rc = DosClose( hFile );
	DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
					sizeof (ULONG));

	decryptPassword(password_enc,password_dec);

// This is the soon to be obsolete description of smbspool
// Usage: smbspool [DEVICE_URI] job-id user title copies options [file]


	f_parms = malloc((strlen(parameters) - strlen("%file%") + strlen(filename)) * sizeof(char));

	searchReplace("%file%", filename, parameters, f_parms);

	sprintf(j_parms,f_parms);
	free(f_parms);

	chdir(workingdir);
	rc = spawnlp(P_WAIT,binfile,binfile,j_parms,NULL);

	while (rc != 0)
	{
		sprintf(errorstr,"Error during spooling %s to %s %s",queue_name,binfile,j_parms);
		resp = WinMessageBox (HWND_DESKTOP,
							HWND_DESKTOP,
							errorstr,
							"Universal Port driver error",
							0L, MB_RETRYCANCEL | MB_WARNING | MB_MOVEABLE);
		if (resp != MBID_CANCEL )
		{
			rc = spawnlp(P_WAIT,binfile,binfile,j_parms,NULL);
		}
		else rc = 0;
	};

	strcpy(filename,&szTemp[pos]);
	DosDelete(filename);

	/* We always have to return success to the spooler subsystem */
	rc = NO_ERROR;
	return rc;
}
ULONG APIENTRY SplPdWrite( HFILE    hFile,
							PVOID   pchData,
							ULONG   cbData,
							PULONG  pcbWritten )
{  APIRET rc;

	rc = DosWrite( hFile,pchData,cbData,pcbWritten);
	rc = DosSleep(0);
	return rc;
}

