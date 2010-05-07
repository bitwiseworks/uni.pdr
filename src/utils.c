#include    "uni.h"

#include    <stdlib.h>
#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "utils.h"

/*  replace one string by another */
BOOL searchReplace(const char *search, const char *replace, const char *string, char *replaced, int size)
{
// create/init some variables
        char *searchStart;
        char *p;
        int len = 0;

// some sanity check
        if (replaced == NULL || replace == NULL)
        {
                return FALSE;
        }

// do we find the searched string at all
        searchStart = strstr(string, search);
        if (searchStart == NULL)
        {
                strncpy(replaced, string, size);
                p = replaced+size;
                *p = '\0';
                return FALSE;
        }

// copy first part
        len = searchStart - string;
        strncpy(replaced, string, len);
        p = replaced+len;
        *p = '\0';

// add the replaced string
        strcat(replaced, replace);

// add the last part
        len += strlen(search);
        strcat(replaced, string+len);

        return TRUE;
}

/* convert a string to uppercase */
void uppercase( char *sPtr )
{
	while( *sPtr != '\0' ) {
		*sPtr = toupper( ( unsigned char ) *sPtr );
	}
}

/* Password encryption/decryption routines from ndpsmb.c */

unsigned char fromhex(char c)
{
	if ('0' <= c && c <= '9')
	{
		return c - '0';
	}
	
	if ('A' <= c && c <= 'F')
	{
		return c - 'A' + 0xA;
	}
	
	if ('a' <= c && c <= 'f')
	{
		return c - 'a' + 0xA;
	}
	
	return 0;
}

char tohex(unsigned char b)
{
	b &= 0xF;
	
	if (b <= 9)
	{
		return b + '0';
	}
	
	return 'A' + (b - 0xA);
}

void decryptPassword(const char *pszCrypt, char *pszPlain)
{
	/* A simple "decryption", character from the hex value. */
	const char *s = pszCrypt;
	char *d = pszPlain;
	
	while (*s)
	{
		*d++ = (char)((fromhex (*s++) << 4) + fromhex (*s++));
	}
	
	*d++ = 0;
}

void encryptPassword(const char *pszPlain, char *pszCrypt)
{
	/* A simple "encryption" encode each character as hex value. */
	const char *s = pszPlain;
	char *d = pszCrypt;
	
	while (*s)
	{
		*d++ = tohex ((*s) >> 4);
		*d++ = tohex (*s);
		s++;
	}
	
	*d++ = 0;
}

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

ULONG CalcStructLength ( HAB hab, HMODULE hModule, USHORT usID )
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

ULONG CalcBufLength ( HAB hab, HMODULE hModule )
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

ULONG NumPortsCanFit ( HAB hab, HMODULE hModule, ULONG cbBuf )
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
VOID CopyStruct ( HAB hab, HMODULE hModule, USHORT usID, PCH pBuf, PULONG pulBeginStruct, PULONG pulBeginText )
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
VOID  CopyNPorts ( HAB hab, HMODULE hModule, PCH pBuf, ULONG ulReturned )
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

BOOL GetPortDescription ( HAB hab, HMODULE hModule, PSZ pszPortName, PSZ pszPortDesc )
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

