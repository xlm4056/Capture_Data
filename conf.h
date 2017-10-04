/*
 * conf.h
 *
 *  Created on: Apr 11, 2017
 *      Author: xieliming
 */

#ifndef CONF_H_
#define CONF_H_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
typedef unsigned int DWORD;
typedef unsigned short WORD;

int  ReplaceStr(char * sSrc,const char * sMatchStr,const char * sReplaceStr);
int  GetPrivateProfileInt(const char *key,const char * cField,int cDefault,char *filename);
int  GetPrivateProfileString(const char* key,const char *cField,char * cDefault,char * cRet,int dRetLen,char * filename );
bool WritePrivateProfileInt(const char * key,const char *cField,int nValue,char * filename);
bool WritePrivateProfileString(const char * key,const char *cField,char *cValue,char * filename);

#endif /* CONF_H_ */
