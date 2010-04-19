#include    <os2.h>

#include    <stdlib.h>
#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "utils.h"

/*  replace one string by another */
BOOL searchReplace(const char *search, const char *replace, const char *string, char *replaced)
{
	/* create init some variables */
	char *searchStart;
	int len = 0;

	/* do we find the searched string at all */
	searchStart = strstr(string, search);
	if (searchStart == NULL)
	{
		strncpy(replaced, string, strlen(replaced));
		return FALSE;
	}

	/* copy first part */
	len = searchStart - string;
	strncpy(replaced, string, len);

	/* add the replaced string */
	strcat(replaced, replace);

	/* add the last part */
	len += strlen(search);
	strcat(replaced, string+len);

	return TRUE;
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

