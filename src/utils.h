/* 
   universal port driver
   general utility functions
   Copyright (C) netlabs.org 2010

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#ifndef __UTILS_H__
#define __UTILS_H__

BOOL searchReplace(const char *search, const char *replace, const char *org_string, char *replaced_string);
void decryptPassword(const char *pszCrypt, char *pszPlain);
void encryptPassword(const char *pszPlain, char *pszCrypt);

#endif // __UTILS__H
