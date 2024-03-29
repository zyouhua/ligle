///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_sysutils.cpp
// 功能描述: 系统杂项功能
// 最后修改: 2005-12-06
///////////////////////////////////////////////////////////////////////////////

#include "icl_sysutils.h"
#include "icl_classes.h"
#include "zlib.h"

namespace icl
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
        strtol(pStr, &endp, 10);
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
    return &sTemp[0];
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
    return &sTemp[0];
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
#ifdef ICL_WIN32
    return stricmp(str1.c_str(), str2.c_str()) == 0;
#endif
#ifdef ICL_LINUX
    return strcasecmp(str1.c_str(), str2.c_str()) == 0;
#endif
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

    while (nIndex < (int)strSearch.size())
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
    for (int i = 0; i < (int)StrList.size(); i++)
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
#ifdef ICL_WIN32
    HANDLE nHandle;
    WIN32_FIND_DATA FindData;
    bool bResult;

    nHandle = FindFirstFile(strFileName.c_str(), &FindData);
    bResult = (nHandle != INVALID_HANDLE_VALUE);
    if (bResult) FindClose(nHandle);
    return bResult;
#endif
#ifdef ICL_LINUX
    return (euidaccess(strFileName.c_str(), F_OK) == 0);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 检查目录是否存在
//-----------------------------------------------------------------------------
bool DirectoryExists(const string& strDir)
{
#ifdef ICL_WIN32
    int nCode;
    nCode = GetFileAttributes(strDir.c_str());
    return (nCode != -1) && ((FILE_ATTRIBUTE_DIRECTORY & nCode) != 0);
#endif
#ifdef ICL_LINUX
    struct stat st;
    bool bResult;

    if (stat(strDir.c_str(), &st) == 0)
        bResult = ((st.st_mode & S_IFDIR) == S_IFDIR);
    else
        bResult = false;

    return bResult;
#endif
}

//-----------------------------------------------------------------------------
// 描述: 创建目录
// 示例:
//   CreateDir("C:\\test");
//   CreateDir("/home/test");
//-----------------------------------------------------------------------------
bool CreateDir(const string& strDir)
{
#ifdef ICL_WIN32
    return CreateDirectory(strDir.c_str(), NULL);
#endif
#ifdef ICL_LINUX
    return mkdir(strDir.c_str(), (mode_t)(-1)) == 0;
#endif
}

//-----------------------------------------------------------------------------
// 描述: 取得文件名中最后一个分隔符的位置(0-based)。若没有，则返回-1
//-----------------------------------------------------------------------------
int GetLastDelimPos(const string& strFileName)
{
    int nResult = strFileName.size() - 1;
    string strDelims;

    strDelims += PATH_DELIM;
#ifdef ICL_WIN32
    strDelims += DRIVER_DELIM;
#endif

    for (; nResult >= 0; nResult--)
        if (strDelims.find(strFileName[nResult], 0) != string::npos)
            break;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 从文件名字符串中取出文件路径
// 参数:
//   strFileName - 包含路径的文件名
// 返回:
//   文件的路径
// 示例:
//   ExtractFilePath("C:\\MyDir\\test.c");         返回: "C:\\MyDir\\"
//   ExtractFilePath("C:");                        返回: "C:\\"
//   ExtractFilePath("/home/user1/data/test.c");   返回: "/home/user1/data/"
//-----------------------------------------------------------------------------
string ExtractFilePath(const string& strFileName)
{
    int nPos = GetLastDelimPos(strFileName);
    return PathWithSlash(strFileName.substr(0, nPos + 1));
}

//-----------------------------------------------------------------------------
// 描述: 从文件名字符串中取出单独的文件名
// 参数:
//   strFileName - 包含路径的文件名
// 返回:
//   文件名
// 示例:
//   ExtractFileName("C:\\MyDir\\test.c");         返回: "test.c"
//   ExtractFilePath("/home/user1/data/test.c");   返回: "test.c"
//-----------------------------------------------------------------------------
string ExtractFileName(const string& strFileName)
{
    int nPos = GetLastDelimPos(strFileName);
    return strFileName.substr(nPos + 1, strFileName.size() - nPos - 1);
}

//-----------------------------------------------------------------------------
// 描述: 强制创建目录
// 参数: 
//   strDir - 待创建的目录 (可以是多级目录)
// 返回:
//   true   - 成功
//   false  - 失败
// 示例:
//   ForceDirectories("C:\\MyDir\\Test");
//   ForceDirectories("/home/user1/data");
//-----------------------------------------------------------------------------
bool ForceDirectories(string strDir)
{
    int nLen = strDir.length();

    if (strDir.empty()) return false;
    if (strDir[nLen-1] == PATH_DELIM)
        strDir.resize(nLen - 1);

#ifdef ICL_WIN32
    if (strDir.length() < 3 || DirectoryExists(strDir) ||
        ExtractFilePath(strDir) == strDir) return true;    // avoid 'xyz:\' problem.
#endif
#ifdef ICL_LINUX
    if (strDir.empty() || DirectoryExists(strDir)) return true;
#endif
    return ForceDirectories(ExtractFilePath(strDir)) && CreateDir(strDir);
}

//-----------------------------------------------------------------------------
// 描述: 删除文件
//-----------------------------------------------------------------------------
bool DeleteFile(const string& strFileName)
{
#ifdef ICL_WIN32
    return ::DeleteFile(strFileName.c_str());
#endif
#ifdef ICL_LINUX
    return (unlink(strFileName.c_str()) != -1);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 取得文件的大小。若失败则返回-1
//-----------------------------------------------------------------------------
int64 GetFileSize(const string& strFileName)
{
    int64 nResult;

    try
    {
        CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
        nResult = FileStream.GetSize();
    }
    catch (CException& e)
    {
        nResult = -1;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 在指定路径下查找符合条件的文件
// 参数:
//   strPath    - 指定在哪个路径下进行查找，并必须指定通配符
//   nAttr      - 只查找符合此属性的文件
//   FindResult - 传回查找结果
// 示例:
//   FindFiles("C:\\test\\*.*", FA_ANY_FILE & ~FA_HIDDEN, fr);
//   FindFiles("/home/*.log", FA_ANY_FILE & ~FA_SYM_LINK, fr); 
//-----------------------------------------------------------------------------
void FindFiles(const string& strPath, uint nAttr, FileFindResult& FindResult)
{
    const uint FA_SPECIAL = FA_HIDDEN | FA_SYS_FILE | FA_VOLUME_ID | FA_DIRECTORY;
    uint nExcludeAttr = ~nAttr & FA_SPECIAL;
    FindResult.clear();
    
#ifdef ICL_WIN32
    HANDLE nFindHandle;
    WIN32_FIND_DATA FindData;

    nFindHandle = FindFirstFile(strPath.c_str(), &FindData);
    if (nFindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((FindData.dwFileAttributes & nExcludeAttr) == 0)
            {
                CFileFindRec Item;
                Item.nFileSize = FindData.nFileSizeHigh;
                Item.nFileSize = (Item.nFileSize << 32) | FindData.nFileSizeLow;
                Item.strFileName = FindData.cFileName;
                Item.nAttr = FindData.dwFileAttributes;

                FindResult.push_back(Item);
            }
        }
        while (FindNextFile(nFindHandle, &FindData));

        FindClose(nFindHandle);
    }
#endif

#ifdef ICL_LINUX
    string strPathOnly = ExtractFilePath(strPath);
    string strPattern = ExtractFileName(strPath);
    string strFileName;
    DIR *pDir;
    struct dirent DirEnt, *pDirEnt = NULL;
    struct stat StatBuf, LinkStatBuf;
    uint nFileAttr, nFileMode;

    if (strPathOnly.empty()) strPathOnly = "/";

    pDir = opendir(strPathOnly.c_str());
    if (pDir)
    {
        while ((readdir_r(pDir, &DirEnt, &pDirEnt) == 0) && pDirEnt)
        {
            if (!fnmatch(strPattern.c_str(), pDirEnt->d_name, 0) == 0) continue;

            strFileName = strPathOnly + pDirEnt->d_name;

            if (lstat(strFileName.c_str(), &StatBuf) == 0)
            {
                nFileAttr = 0;
                nFileMode = StatBuf.st_mode;

                if (S_ISDIR(nFileMode))
                    nFileAttr |= FA_DIRECTORY;
                else if (!S_ISREG(nFileMode))
                {
                    if (S_ISLNK(nFileMode))
                    {
                        nFileAttr |= FA_SYM_LINK;
                        if ((stat(strFileName.c_str(), &LinkStatBuf) == 0) &&
                            (S_ISDIR(LinkStatBuf.st_mode)))
                            nFileAttr |= FA_DIRECTORY;
                    }
                    nFileAttr |= FA_SYS_FILE;
                }

                if (pDirEnt->d_name[0] == '.' && pDirEnt->d_name[1])
                    if (!(pDirEnt->d_name[1] == '.' && !pDirEnt->d_name[2]))
                        nFileAttr |= FA_HIDDEN;

                if (euidaccess(strFileName.c_str(), W_OK) != 0)
                    nFileAttr |= FA_READ_ONLY;

                if ((nFileAttr & nExcludeAttr) == 0)
                {
                    CFileFindRec Item;
                    Item.nFileSize = StatBuf.st_size;
                    Item.strFileName = pDirEnt->d_name;
                    Item.nAttr = nFileAttr;

                    FindResult.push_back(Item);
                }
            }
        } // while

        closedir(pDir);
    }
#endif
}

//-----------------------------------------------------------------------------
// 描述: 补全路径字符串后面的 "\" 或 "/"
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
// 描述: 去掉路径字符串后面的 "\" 或 "/"
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
#ifdef ICL_WIN32
    char *pErrorMsg;
    
    pErrorMsg = strerror(nErrorCode);
    return pErrorMsg;
#endif
#ifdef ICL_LINUX
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
#endif
}

//-----------------------------------------------------------------------------
// 描述: 睡眠 fSeconds 秒，可精确到纳秒。
// 参数:
//   fSeconds       - 睡眠的秒数，可为小数，可精确到纳秒 (实际精确度取决于操作系统)
//   AllowInterrupt - 是否允许信号中断
//-----------------------------------------------------------------------------
void NanoSleep(double fSeconds, bool AllowInterrupt)
{
#ifdef ICL_WIN32
    Sleep((uint)(fSeconds * 1000));
#endif
#ifdef ICL_LINUX
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
#endif
}

//-----------------------------------------------------------------------------
// 描述: 取得当前 Ticks，单位:毫秒
//-----------------------------------------------------------------------------
uint GetCurTicks()
{
#ifdef ICL_WIN32
    return GetTickCount();
#endif
#ifdef ICL_LINUX
    timeval tv;
    gettimeofday(&tv, NULL);
    return int64(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
#endif
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
// 描述: 数据压缩 (ZLIB)
//-----------------------------------------------------------------------------
bool ZlibCompress(CBuffer& Buffer)
{
    bool bResult = false;
    uint nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 120 / 100 + 12);
            uint DestLen = DestBuffer.GetSize();

            bResult = (compress(
                (Bytef*)DestBuffer.Data(),  // dest buffer
                (uLongf*)&DestLen,          // dest size
                (Bytef*)Buffer.Data(),      // src buffer
                (uLong)nSrcSize)            // src size
                == Z_OK);

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 数据解压 (ZLIB)
//-----------------------------------------------------------------------------
bool ZlibUncompress(CBuffer& Buffer)
{
    bool bResult = false;
    uint nSrcSize = Buffer.GetSize();

    if (nSrcSize > 0)
    {
        try
        {
            CBuffer DestBuffer(nSrcSize * 5);
            uint DestLen = DestBuffer.GetSize();
            int r;

            while (true)
            {
                r = uncompress(
                    (Bytef*)DestBuffer.Data(),  // dest buffer
                    (uLongf*)&DestLen,          // dest size
                    (Bytef*)Buffer.Data(),      // src buffer
                    (uLong)nSrcSize);           // src size

                // 如果解压成功
                if (r == Z_OK)
                {
                    bResult = true;
                    break;
                }
                // 如果输出缓冲区空间不足
                else if (r == Z_BUF_ERROR)
                {
                    bResult = false;
                    DestBuffer.SetSize(DestBuffer.GetSize() * 2);
                    DestLen = DestBuffer.GetSize();
                }
                else
                {
                    bResult = false;
                    break;
                }
            }

            if (bResult)
                Buffer.Assign(DestBuffer.Data(), DestLen);
        }
        catch (CException& e)
        {
            bResult = false;
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 计算 CRC32 值
// 注意:
//   如果要利用此函数循环计算一段数据，先将参数 nCrc 置为 0xFFFFFFFF，下次调用
//   时，将上次的计算结果取反再传入。
//-----------------------------------------------------------------------------
uint Crc32(uint nCrc, void *pBuffer, int nSize)
{
    return crc32(nCrc, (unsigned char*)pBuffer, nSize);
}

//-----------------------------------------------------------------------------
// 描述: 计算一块缓存的 CRC32 值
// 备注:
//   若 pBuffer == NULL，返回 0
//   若 nSize == 0，返回 -1
//-----------------------------------------------------------------------------
uint CalcBufferCrc32(void *pBuffer, int nSize)
{
    return Crc32(0xFFFFFFFF, pBuffer, nSize);
}

//-----------------------------------------------------------------------------
// 描述: 将数据进行 Base64 编码
// 参数:
//   pData  - 待进行编码的数据
//   nSize  - pData中的字节数
// 返回:
//   编码后的字符串(不含'\0'字符)
//-----------------------------------------------------------------------------
string StrToBase64(const char *pData, int nSize)
{
    static char *pTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    uint b;
    int i;
    char *d, *t;
    string strResult;

    if (!pData || nSize <= 0) return "";
    strResult.resize(nSize * 4 / 3 + 4);
    d = (char*)strResult.c_str();
    t = pTable;

    while (nSize > 0)
    {
        b = 0;
        for (i = 0; i <= 2; i++)
        {
            b = b << 8;
            if (nSize > 0)
            {
                b = b | (unsigned char)(*pData);
                pData++;
            }
            nSize--;
        }
        for (i = 3; i >= 0; i--)
        {
            if (nSize < 0)
            {
                d[i] = t[64];
                nSize++;
            }
            else
            {
                d[i] = t[b & 0x3F];
            }
            b = b >> 6;
        }
        d += 4;
    }

    strResult.resize(d - strResult.c_str());
    return strResult;
}

//-----------------------------------------------------------------------------
// 描述: 将数据进行 Base16 编码
// 参数:
//   pData  - 待进行编码的数据
//   nSize  - pData中的字节数
// 返回:
//   编码后的字符串(不含'\0'字符)
//-----------------------------------------------------------------------------
string StrToBase16(const char *pData, int nSize)
{
    static char *pTable = "0123456789ABCDEF";
    char *d, *s;
    string strResult;

    if (!pData || nSize <= 0) return "";
    strResult.resize(nSize * 2);
    d = (char*)strResult.c_str();
    s = (char*)pData;

    while (nSize > 0)
    {
        *d++ = pTable[((*s) >> 4) & 0x0F];
        *d++ = pTable[(*s) & 0x0F];
        s++;
        nSize--;
    }

    return strResult;
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
// 描述: 内存块内容清零
//-----------------------------------------------------------------------------
void ZeroBuffer(void *pBuffer, int nCount)
{
#ifdef ICL_WIN32
    ZeroMemory(pBuffer, nCount);
#endif
#ifdef ICL_LINUX
    bzero(pBuffer, nCount);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 填充内存块内容
//-----------------------------------------------------------------------------
void FillBuffer(void *pBuffer, int nCount, byte nValue)
{
    memset(pBuffer, nValue, nCount);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
