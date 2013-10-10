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
// ��������

// �ַ����е�ͨ�÷ָ���
const char STRING_SPLITTER = ',';

///////////////////////////////////////////////////////////////////////////////

// ��������� HASH ����
string HashPassword(const string& strPassword);

// ����ѹ��/��ѹ (ZLIB)
bool ZlibCompress(CBuffer& Buffer);
bool ZlibUncompress(CBuffer& Buffer);

// MBCS -> Unicode
wstring MultiByteToWideChar(const string& str);

// �����ַ���֧��
string MakeIntListString(const IntegerArray& IntList, char chSplitter);
bool AddNumberToList(string& strList, int nNumber, int nMaxCount, bool& bExists);
bool DeleteNumberFromList(string& strList, int nNumber);

// ȡ�ó��ö���
CAppDispatcher* GetAppDispatcher();
CAppGlobalData* GetAppGlobalData();

///////////////////////////////////////////////////////////////////////////////

#endif // _UTILITIES_H_ 
