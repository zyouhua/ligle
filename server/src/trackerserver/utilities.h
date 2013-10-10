///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// utilities.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <zlib.h>

#include "ise_system.h"
#include "dispatcher.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

// 字符串中的通用分隔符
const char STRING_SPLITTER = ',';

///////////////////////////////////////////////////////////////////////////////

// 对密码进行 HASH 加密
string HashPassword(const string& strPassword);

// 数据压缩/解压 (ZLIB)
bool ZlibCompress(CBuffer& Buffer);
bool ZlibUncompress(CBuffer& Buffer);

// MBCS -> Unicode
wstring MultiByteToWideChar(const string& str);

// 号码字符串支持
string MakeIntListString(const IntegerArray& IntList, char chSplitter);
bool AddNumberToList(string& strList, int nNumber, int nMaxCount, bool& bExists);
bool DeleteNumberFromList(string& strList, int nNumber);

// 取得常用对象
CAppDispatcher* GetAppDispatcher();
CAppGlobalData* GetAppGlobalData();

///////////////////////////////////////////////////////////////////////////////

#endif // _UTILITIES_H_ 
