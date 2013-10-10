///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_sysutils.cpp
// ��������: ϵͳ�����
// ����޸�: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// �����

//-----------------------------------------------------------------------------
// ����: �ж�һ���ַ����ǲ���һ������
//-----------------------------------------------------------------------------
bool IsInt(const string& str)
{
    bool bResult;
    int nLen = str.size();
    char *pStr = (char*)str.c_str();
    
    bResult = (nLen > 0) && !isspace(pStr[0]);

    if (bResult)
    {
        char *endp;
        strtoll(pStr, &endp, 10);
        bResult = (endp - pStr == nLen);
    } 

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: �ж�һ���ַ����ǲ���һ��������
//-----------------------------------------------------------------------------
bool IsFloat(const string& str)
{
    bool bResult;
    int nLen = str.size();
    char *pStr = (char*)str.c_str();
    char *endp;
    
    bResult = (nLen > 0) && !isspace(pStr[0]);

    if (bResult)
    {
        char *endp;
        strtod(pStr, &endp);
        bResult = (endp - pStr == nLen);
    } 

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ���ת��������(��ת��ʧ�ܣ��򷵻� nDefault)
//-----------------------------------------------------------------------------
int StrToInt(const string& str, int nDefault)
{
    if (IsInt(str))
        return strtol(str.c_str(), NULL, 10);
    else
        return nDefault;
}

//-----------------------------------------------------------------------------
// ����: ����ת�����ַ���
//-----------------------------------------------------------------------------
string IntToStr(int nValue)
{
    char sTemp[64];
    sprintf(sTemp, "%d", nValue);
    return sTemp;
}

//-----------------------------------------------------------------------------
// ����: �ַ���ת���ɸ�����(��ת��ʧ�ܣ��򷵻� fDefault)
//-----------------------------------------------------------------------------
double StrToFloat(const string& str, double fDefault)
{
    if (IsFloat(str))
        return strtod(str.c_str(), NULL);
    else
        return fDefault;
}

//-----------------------------------------------------------------------------
// ����: ������ת�����ַ���
//-----------------------------------------------------------------------------
string FloatToStr(double fValue, const char *sFormat)
{
    char sTemp[256];
    sprintf(sTemp, sFormat, fValue);
    return sTemp;
}

//-----------------------------------------------------------------------------
// ����: ��ʽ���ַ��� (��FormatString��������)
//-----------------------------------------------------------------------------
void FormatStringV(string& strResult, const char *sFormatString, va_list argList)
{
    int nSize = 100;
    char *pBuffer = (char *)malloc(nSize);

    while (pBuffer)
    {
        int nChars;
        nChars = vsnprintf(pBuffer, nSize, sFormatString, argList);
        if (nChars > -1 && nChars < nSize)
            break;
        if (nChars > -1)
            nSize = nChars + 1;
        else
            nSize *= 2;
        pBuffer = (char *)realloc(pBuffer, nSize);
    }

    if (pBuffer)
    {
        strResult = pBuffer;
        free(pBuffer);
    }
    else
        strResult = "";
}

//-----------------------------------------------------------------------------
// ����: ��ʽ���ַ���
// ����:
//   strResult      - ��Ž����
//   sFormatString  - ��ʽ���ַ���
//   ...            - ��ʽ������
// ʾ��: 
//   FormatString(strResult, "ABC%sDEF%d", strTemp, nTemp);
//-----------------------------------------------------------------------------
void FormatString(string& strResult, const char *sFormatString, ...)
{
    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strResult, sFormatString, argList);
    va_end(argList);
}

//-----------------------------------------------------------------------------
// ����: ���ظ�ʽ������ַ���
// ����:
//   sFormatString  - ��ʽ���ַ���
//   ...            - ��ʽ������
//-----------------------------------------------------------------------------
string FormatString(const char *sFormatString, ...)
{
    string strResult;

    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strResult, sFormatString, argList);
    va_end(argList);

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ж������ַ����Ƿ���ͬ (�����ִ�Сд)
//-----------------------------------------------------------------------------
bool SameText(const string& str1, const string& str2)
{
    return strcasecmp(str1.c_str(), str2.c_str()) == 0;
}

//-----------------------------------------------------------------------------
// ����: ȥ���ַ���ͷβ�Ŀհ��ַ� (ASCII <= 32)
//-----------------------------------------------------------------------------
string TrimString(const string& str)
{
    string strResult;
    int i, nLen;

    nLen = str.size();
    i = 0;
    while (i < nLen && (byte)str[i] <= 32) i++;
    if (i < nLen)
    {
        while ((byte)str[nLen-1] <= 32) nLen--;
        strResult = str.substr(i, nLen - i);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ������д
//-----------------------------------------------------------------------------
string UpperCase(const string& str)
{
    string strResult = str;
    int nLen = strResult.size();
    char c;

    for (int i = 0; i < nLen; i++)
    {
        c = strResult[i];
        if (c >= 'a' && c <= 'z')
            strResult[i] = c - 32;
    }
    
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ�����Сд
//-----------------------------------------------------------------------------
string LowerCase(const string& str)
{
    string strResult = str;
    int nLen = strResult.size();
    char c;

    for (int i = 0; i < nLen; i++)
    {
        c = strResult[i];
        if (c >= 'A' && c <= 'Z')
            strResult[i] = c + 32;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ַ����滻
// ����:
//   strSource        - Դ��
//   strOldPattern    - Դ���н����滻���ַ���
//   strNewPattern    - ȡ�� strOldPattern ���ַ���
//   bReplaceAll      - �Ƿ��滻Դ��������ƥ����ַ���(��Ϊfalse����ֻ�滻��һ��)
//   bCaseSensitive   - �Ƿ����ִ�Сд
// ����:
//   �����滻����֮����ַ���
//-----------------------------------------------------------------------------
string RepalceString(const string& strSource, const string& strOldPattern, 
    const string& strNewPattern, bool bReplaceAll, bool bCaseSensitive)
{
    string strResult = strSource;
    string strSearch, strPattern;
    int nOffset, nIndex, nOldPattLen, nNewPattLen;

    if (!bCaseSensitive)
    {
        strSearch = UpperCase(strSource);
        strPattern = UpperCase(strOldPattern);
    }
    else
    {
        strSearch = strSource;
        strPattern = strOldPattern;
    }

    nOldPattLen = strOldPattern.size();
    nNewPattLen = strNewPattern.size();
    nIndex = 0;

    while (nIndex < strSearch.size())
    {
        nOffset = strSearch.find(strPattern, nIndex);
        if (nOffset == string::npos) break;  // ��û�ҵ�
     
        strSearch.replace(nOffset, nOldPattLen, strNewPattern);
        strResult.replace(nOffset, nOldPattLen, strNewPattern);
        nIndex = (nOffset + nNewPattLen);

        if (!bReplaceAll) break;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �ָ��ַ���
// ����:
//   strSource  - Դ��
//   chSplitter - �ָ���
//   StrList    - ��ŷָ�֮����ַ����б�
// ʾ��:
//   ""          -> []
//   " "         -> [" "]
//   ","         -> ["", ""]
//   "a,b,c"     -> ["a", "b", "c"]
//   ",a,,b,c,"  -> ["", "a", "", "b", "c", ""]
//-----------------------------------------------------------------------------
void SplitString(const string& strSource, char chSplitter, StringArray& StrList)
{
    int nIndex = 0;
    int nOffset;

    StrList.clear();
    if (strSource.empty()) return;

    while (true)
    {
        nOffset = strSource.find(chSplitter, nIndex);
        if (nOffset == string::npos)   // ��û�ҵ�
        {
            StrList.push_back(strSource.substr(nIndex));
            break;
        }
        else
        {
            StrList.push_back(strSource.substr(nIndex, nOffset - nIndex));
            nIndex = nOffset + 1;
        }
    }
}

//-----------------------------------------------------------------------------
// ����: �ָ��ַ�����ת�����������б�
// ����:
//   strSource  - Դ��
//   chSplitter - �ָ���
//   IntList    - ��ŷָ�֮����������б�
//-----------------------------------------------------------------------------
void SplitStringToInt(const string& strSource, char chSplitter, IntegerArray& IntList)
{
    StringArray StrList;

    SplitString(strSource, chSplitter, StrList);
    IntList.clear();
    for (int i = 0; i < StrList.size(); i++)
        IntList.push_back(atoi(StrList[i].c_str()));
}

//-----------------------------------------------------------------------------
// ����: ���ƴ� pSource �� pDest ��
// ��ע: 
//   1. ���ֻ���� nMaxBytes ���ֽڵ� pDest �У�����������'\0'��
//   2. ��� pSource ��ʵ�ʳ���(strlen)С�� nMaxBytes�����ƻ���ǰ������
//      pDest ��ʣ�ಿ���� '\0' ��䡣
//   3. ��� pSource ��ʵ�ʳ���(strlen)���� nMaxBytes������֮��� pDest û�н�������
//-----------------------------------------------------------------------------
char *StrNCopy(char *pDest, const char *pSource, int nMaxBytes)
{
    if (nMaxBytes > 0)
    {
        if (pSource)
            return strncpy(pDest, pSource, nMaxBytes);
        else
            return strcpy(pDest, "");
    }

    return pDest;
}

//-----------------------------------------------------------------------------
// ����: ���ƴ� pSource �� pDest ��
// ��ע: ���ֻ���� nDestSize ���ֽڵ� pDest �С����� pDest ������ֽ���Ϊ'\0'��
// ����: 
//   nDestSize - pDest�Ĵ�С
//-----------------------------------------------------------------------------
char *StrNZCopy(char *pDest, const char *pSource, int nDestSize)
{
    if (nDestSize > 0)
    {
        if (pSource)
        {
            char *p;
            p = strncpy(pDest, pSource, nDestSize);
            pDest[nDestSize - 1] = '\0';
            return p;
        }
        else
            return strcpy(pDest, "");
    }
    else
        return pDest;
}

//-----------------------------------------------------------------------------
// ����: ����ļ��Ƿ����
//-----------------------------------------------------------------------------
bool FileExists(const string& strFileName)
{
    return (euidaccess(strFileName.c_str(), F_OK) == 0);
}

//-----------------------------------------------------------------------------
// ����: ���Ŀ¼�Ƿ����
//-----------------------------------------------------------------------------
bool DirectoryExists(const string& strDir)
{
    struct stat st;
    bool bResult;

    if (stat(strDir.c_str(), &st) == 0)
        bResult = ((st.st_mode & S_IFDIR) == S_IFDIR);
    else
        bResult = false;

    return bResult;
}

//-----------------------------------------------------------------------------
// ����: ����Ŀ¼
// ʾ��: CreateDir("/home/test");
//-----------------------------------------------------------------------------
bool CreateDir(const string& strDir)
{
    return mkdir(strDir.c_str(), (mode_t)(-1)) == 0;
}

//-----------------------------------------------------------------------------
// ����: ���ļ�����ȡ���ļ�·��
// ����:
//   strFileName - ����·�����ļ���
// ����:
//   �ļ���·��
// ʾ��:
//   ExtractFilePath("/home/user1/data/test.c");
//   ������: "/home/user1/data/"
//-----------------------------------------------------------------------------
string ExtractFilePath(const string& strFileName)
{
    int nLen, nPos;
    string strResult;

    nLen = strFileName.length();
    nPos = -1;
    for (int i = nLen - 1; i >= 0; i--)
    {
        if (strFileName[i] == PATH_DELIM)
        {
            nPos = i;
            break;
        }
    }

    if (nPos != -1)
        strResult = strFileName.substr(0, nPos + 1);
    else
        strResult = "";

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ǿ�ƴ���Ŀ¼
// ����: 
//   strDir - ��������Ŀ¼ (�����Ƕ༶Ŀ¼)
// ����:
//   true   - �ɹ�
//   false  - ʧ��
// ʾ��:
//   ForceDirectories("/home/user1/data");
//-----------------------------------------------------------------------------
bool ForceDirectories(string strDir)
{
    int nLen = strDir.length();

    if (strDir.empty()) return false;
    if (strDir[nLen-1] == PATH_DELIM)
        strDir.resize(nLen - 1);
    if (strDir.empty() || DirectoryExists(strDir)) return true;
    return ForceDirectories(ExtractFilePath(strDir)) && CreateDir(strDir);
}

//-----------------------------------------------------------------------------
// ����: ��ȫ·���ַ�������� "/"
//-----------------------------------------------------------------------------
string PathWithSlash(const string& strPath)
{
    string strResult = TrimString(strPath);
    int nLen = strResult.size();
    if (nLen > 0 && strResult[nLen-1] != PATH_DELIM)
        strResult += PATH_DELIM;
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ȥ��·���ַ�������� "/"
//-----------------------------------------------------------------------------
string PathWithoutSlash(const string& strPath)
{
    string strResult = TrimString(strPath);
    int nLen = strResult.size();
    if (nLen > 0 && strResult[nLen-1] == PATH_DELIM)
        strResult.resize(nLen - 1);
    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ���ز���ϵͳ��������Ӧ�Ĵ�����Ϣ
//-----------------------------------------------------------------------------
string SysErrorMessage(int nErrorCode)
{
    const int ERROR_MSG_SIZE = 256;
    char sErrorMsg[ERROR_MSG_SIZE];
    string strResult;

    sErrorMsg[0] = 0;
    strerror_r(nErrorCode, sErrorMsg, ERROR_MSG_SIZE);
    if (sErrorMsg[0] == 0)
        FormatString(strResult, "System error: %d", nErrorCode);
    else
        strResult = sErrorMsg;

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ˯�� fSeconds �룬�ɾ�ȷ�����롣
// ����:
//   fSeconds       - ˯�ߵ���������ΪС�����ɾ�ȷ������ (ʵ�ʾ�ȷ��ȡ���ڲ���ϵͳ)
//   AllowInterrupt - �Ƿ������ź��ж�
//-----------------------------------------------------------------------------
void NanoSleep(double fSeconds, bool AllowInterrupt)
{
    const uint NANO_PER_SEC = 1000000000;  // һ����ڶ�������
    struct timespec req, remain;
    int r;

    req.tv_sec = (uint)fSeconds;
    req.tv_nsec = (uint)((fSeconds - req.tv_sec) * NANO_PER_SEC);

    while (true)
    {
        r = nanosleep(&req, &remain);
        if (r == -1 && errno == EINTR && !AllowInterrupt)
            req = remain;
        else
            break;
    }
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ Ticks����λ:����
//-----------------------------------------------------------------------------
uint GetCurTicks()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return int64(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

//-----------------------------------------------------------------------------
// ����: ȡ������ Ticks ֮��
//-----------------------------------------------------------------------------
uint GetTickDiff(uint nOldTicks, uint nNewTicks)
{
    if (nNewTicks >= nOldTicks)
        return (nNewTicks - nOldTicks);
    else
        return (uint(-1) - nOldTicks + nNewTicks);
}

//-----------------------------------------------------------------------------
// ����: ����� "���������"
//-----------------------------------------------------------------------------
void Randomize()
{
    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------
// ����: ���� [nMin..nMax] ֮���һ��������������߽�
//-----------------------------------------------------------------------------
int GetRandom(int nMin, int nMax)
{
    return nMin + (int)((double)(nMax - nMin + 1) * rand() / (RAND_MAX + 1.0));
}

//-----------------------------------------------------------------------------
// ����: �ƶ��ڴ��
//-----------------------------------------------------------------------------
void MoveBuffer(const void *pSource, void *pDest, int nCount)
{
    memmove(pDest, pSource, nCount);
}

//-----------------------------------------------------------------------------
// ����: ����ڴ������
//-----------------------------------------------------------------------------
void FillBuffer(void *pBuffer, int nCount, byte nValue)
{
    memset(pBuffer, nValue, nCount);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
