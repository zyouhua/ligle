///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_sysutils.cpp
// 功能描述: 系统杂项函数
// 最后修改: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

//-----------------------------------------------------------------------------
// 描述: 判断一个字符串是不是一个整数
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
// 描述: 判断一个字符串是不是一个浮点数
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
// 描述: 字符串转换成整型(若转换失败，则返回 nDefault)
//-----------------------------------------------------------------------------
int StrToInt(const string& str, int nDefault)
{
    if (IsInt(str))
        return strtol(str.c_str(), NULL, 10);
    else
        return nDefault;
}

//-----------------------------------------------------------------------------
// 描述: 整型转换成字符串
//-----------------------------------------------------------------------------
string IntToStr(int nValue)
{
    char sTemp[64];
    sprintf(sTemp, "%d", nValue);
    return sTemp;
}

//-----------------------------------------------------------------------------
// 描述: 字符串转换成浮点型(若转换失败，则返回 fDefault)
//-----------------------------------------------------------------------------
double StrToFloat(const string& str, double fDefault)
{
    if (IsFloat(str))
        return strtod(str.c_str(), NULL);
    else
        return fDefault;
}

//-----------------------------------------------------------------------------
// 描述: 浮点型转换成字符串
//-----------------------------------------------------------------------------
string FloatToStr(double fValue, const char *sFormat)
{
    char sTemp[256];
    sprintf(sTemp, sFormat, fValue);
    return sTemp;
}

//-----------------------------------------------------------------------------
// 描述: 格式化字符串 (供FormatString函数调用)
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
// 描述: 格式化字符串
// 参数:
//   strResult      - 存放结果串
//   sFormatString  - 格式化字符串
//   ...            - 格式化参数
// 示例: 
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
// 描述: 返回格式化后的字符串
// 参数:
//   sFormatString  - 格式化字符串
//   ...            - 格式化参数
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
// 描述: 判断两个字符串是否相同 (不区分大小写)
//-----------------------------------------------------------------------------
bool SameText(const string& str1, const string& str2)
{
    return strcasecmp(str1.c_str(), str2.c_str()) == 0;
}

//-----------------------------------------------------------------------------
// 描述: 去掉字符串头尾的空白字符 (ASCII <= 32)
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
// 描述: 字符串变大写
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
// 描述: 字符串变小写
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
// 描述: 字符串替换
// 参数:
//   strSource        - 源串
//   strOldPattern    - 源串中将被替换的字符串
//   strNewPattern    - 取代 strOldPattern 的字符串
//   bReplaceAll      - 是否替换源串中所有匹配的字符串(若为false，则只替换第一处)
//   bCaseSensitive   - 是否区分大小写
// 返回:
//   进行替换动作之后的字符串
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
        if (nOffset == string::npos) break;  // 若没找到
     
        strSearch.replace(nOffset, nOldPattLen, strNewPattern);
        strResult.replace(nOffset, nOldPattLen, strNewPattern);
        nIndex = (nOffset + nNewPattLen);

        if (!bReplaceAll) break;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// 描述: 分割字符串
// 参数:
//   strSource  - 源串
//   chSplitter - 分隔符
//   StrList    - 存放分割之后的字符串列表
// 示例:
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
        if (nOffset == string::npos)   // 若没找到
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
// 描述: 分割字符串并转换成整型数列表
// 参数:
//   strSource  - 源串
//   chSplitter - 分隔符
//   IntList    - 存放分割之后的整型数列表
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
// 描述: 复制串 pSource 到 pDest 中
// 备注: 
//   1. 最多只复制 nMaxBytes 个字节到 pDest 中，包括结束符'\0'。
//   2. 如果 pSource 的实际长度(strlen)小于 nMaxBytes，则复制会提前结束，
//      pDest 的剩余部分以 '\0' 填充。
//   3. 如果 pSource 的实际长度(strlen)大于 nMaxBytes，则复制之后的 pDest 没有结束符。
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
// 描述: 复制串 pSource 到 pDest 中
// 备注: 最多只复制 nDestSize 个字节到 pDest 中。并将 pDest 的最后字节设为'\0'。
// 参数: 
//   nDestSize - pDest的大小
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
// 描述: 检查文件是否存在
//-----------------------------------------------------------------------------
bool FileExists(const string& strFileName)
{
    return (euidaccess(strFileName.c_str(), F_OK) == 0);
}

//-----------------------------------------------------------------------------
// 描述: 检查目录是否存在
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
// 描述: 创建目录
// 示例: CreateDir("/home/test");
//-----------------------------------------------------------------------------
bool CreateDir(const string& strDir)
{
    return mkdir(strDir.c_str(), (mode_t)(-1)) == 0;
}

//-----------------------------------------------------------------------------
// 描述: 从文件名中取出文件路径
// 参数:
//   strFileName - 包含路径的文件名
// 返回:
//   文件的路径
// 示例:
//   ExtractFilePath("/home/user1/data/test.c");
//   将返回: "/home/user1/data/"
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
// 描述: 强制创建目录
// 参数: 
//   strDir - 待创建的目录 (可以是多级目录)
// 返回:
//   true   - 成功
//   false  - 失败
// 示例:
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
// 描述: 补全路径字符串后面的 "/"
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
// 描述: 去掉路径字符串后面的 "/"
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
// 描述: 返回操作系统错误代码对应的错误信息
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
// 描述: 睡眠 fSeconds 秒，可精确到纳秒。
// 参数:
//   fSeconds       - 睡眠的秒数，可为小数，可精确到纳秒 (实际精确度取决于操作系统)
//   AllowInterrupt - 是否允许信号中断
//-----------------------------------------------------------------------------
void NanoSleep(double fSeconds, bool AllowInterrupt)
{
    const uint NANO_PER_SEC = 1000000000;  // 一秒等于多少纳秒
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
// 描述: 取得当前 Ticks，单位:毫秒
//-----------------------------------------------------------------------------
uint GetCurTicks()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return int64(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

//-----------------------------------------------------------------------------
// 描述: 取得两个 Ticks 之差
//-----------------------------------------------------------------------------
uint GetTickDiff(uint nOldTicks, uint nNewTicks)
{
    if (nNewTicks >= nOldTicks)
        return (nNewTicks - nOldTicks);
    else
        return (uint(-1) - nOldTicks + nNewTicks);
}

//-----------------------------------------------------------------------------
// 描述: 随机化 "随机数种子"
//-----------------------------------------------------------------------------
void Randomize()
{
    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------
// 描述: 返回 [nMin..nMax] 之间的一个随机数，包含边界
//-----------------------------------------------------------------------------
int GetRandom(int nMin, int nMax)
{
    return nMin + (int)((double)(nMax - nMin + 1) * rand() / (RAND_MAX + 1.0));
}

//-----------------------------------------------------------------------------
// 描述: 移动内存块
//-----------------------------------------------------------------------------
void MoveBuffer(const void *pSource, void *pDest, int nCount)
{
    memmove(pDest, pSource, nCount);
}

//-----------------------------------------------------------------------------
// 描述: 填充内存块内容
//-----------------------------------------------------------------------------
void FillBuffer(void *pBuffer, int nCount, byte nValue)
{
    memset(pBuffer, nValue, nCount);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
