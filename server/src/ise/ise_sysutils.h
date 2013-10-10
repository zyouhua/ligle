///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_sysutils.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_SYSUTILS_H_
#define _ISE_SYSUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <iostream>
#include <string>

#include "ise_types.h"

using namespace std; 

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 常量定义

const char PATH_DELIM = '/';

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

//-----------------------------------------------------------------------------
//-- 字符串函数:

bool IsInt(const string& str);
bool IsFloat(const string& str);

int StrToInt(const string& str, int nDefault = 0);
string IntToStr(int nValue);
double StrToFloat(const string& str, double fDefault = 0);
string FloatToStr(double fValue, const char *sFormat = "%f");

void FormatStringV(string& strResult, const char *sFormatString, va_list argList);
void FormatString(string& strResult, const char *sFormatString, ...);
string FormatString(const char *sFormatString, ...);

bool SameText(const string& str1, const string& str2);
string TrimString(const string& str);
string UpperCase(const string& str);
string LowerCase(const string& str);
string RepalceString(const string& strSource, const string& strOldPattern, 
    const string& strNewPattern, bool bReplaceAll = false, bool bCaseSensitive = true);
void SplitString(const string& strSource, char chSplitter, StringArray& StrList);
void SplitStringToInt(const string& strSource, char chSplitter, IntegerArray& IntList);
char *StrNCopy(char *pDest, const char *pSource, int nMaxBytes);
char *StrNZCopy(char *pDest, const char *pSource, int nDestSize);

//-----------------------------------------------------------------------------
//-- 文件和目录:

bool FileExists(const string& strFileName);
bool DirectoryExists(const string& strDir);
bool CreateDir(const string& strDir);
string ExtractFilePath(const string& strFileName);
bool ForceDirectories(string strDir);
string PathWithSlash(const string& strPath);
string PathWithoutSlash(const string& strPath);

//-----------------------------------------------------------------------------
//-- 系统相关:

string SysErrorMessage(int nErrorCode);
void NanoSleep(double fSeconds, bool AllowInterrupt);
uint GetCurTicks();
uint GetTickDiff(uint nOldTicks, uint nNewTicks);

//-----------------------------------------------------------------------------
//-- 其它函数:

void Randomize();
int GetRandom(int nMin, int nMax);
void MoveBuffer(const void *pSource, void *pDest, int nCount);
void FillBuffer(void *pBuffer, int nCount, byte nValue);

template <typename T> 
inline const T& Min(const T& a, const T& b) { return ((a < b)? a : b); }

template <typename T> 
inline const T& Max(const T& a, const T& b) { return ((a < b)? b : a); }

template <typename T> 
inline const T& TrimValue(const T& Value, const T& Min, const T& Max)
    { return (Value > Max) ? Max : (Value < Min ? Min : Value); }

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SYSUTILS_H_ 
