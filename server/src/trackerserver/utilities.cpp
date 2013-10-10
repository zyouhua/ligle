///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: utilities.cpp
// 功能描述: 与业务有关的杂项函数
// 最后修改: 2005-04-27
///////////////////////////////////////////////////////////////////////////////

#include "utilities.h"
#include "hash_sha1.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// 描述: 将数据进行Base64编码
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
// 描述: 对密码进行 HASH 加密
// 返回: SHA1 + Base64 两次运算的结果，不含#0字符，共28字节。
//-----------------------------------------------------------------------------
string HashPassword(const string& strPassword)
{
    const int DIGEST_SIZE = 20;
    char chDigest[DIGEST_SIZE];

    HashSha1(strPassword.c_str(), strPassword.length(), chDigest);
    return StrToBase64(chDigest, DIGEST_SIZE);
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
// 描述: MBCS -> Unicode
// 备注: 转换过程依赖当前字符集 ( 可用 setlocale(LC_CTYPE, ...) 设置)
//-----------------------------------------------------------------------------
wstring MultiByteToWideChar(const string& str)
{
    wstring strResult;
    int nSrcLen = str.length();
    
    if (nSrcLen > 0)
    {
        vector<wchar_t> w(nSrcLen);
        int n = mbstowcs(&w[0], str.c_str(), nSrcLen);
        if (n != -1)
            strResult.assign(&w[0], n);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// 描述: 将若干整型数用指定分隔符串连起来，返回其字符串
// 参数:
//   IntList    - 整型数组
//   chSplitter - 分隔符，比如","
// 返回:
//   串连之后的字符串，如 "123,456,789"
//-----------------------------------------------------------------------------
string MakeIntListString(const IntegerArray& IntList, char chSplitter)
{
    int nCount = IntList.size();
    string strResult;

    for (int i = 0; i < nCount; i++)
    {
        if (!strResult.empty()) strResult += chSplitter;
        strResult += IntToStr(IntList[i]);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// 描述: 将一个数字nNumber加入到字符串strList中，以逗号分隔
// 参数:
//   strList    - 将数字加入到这个字符串中
//   nNumber    - 被加入的数字
//   nMaxCount  - 字符串strList中最多允许放多少个数字
//   bExists    - 返回字符串中是否已经存在这个数字
// 返回:
//   true   - 成功
//   false  - 失败 (容量不够)
//-----------------------------------------------------------------------------
bool AddNumberToList(string& strList, int nNumber, int nMaxCount, bool& bExists)
{
    bool bResult = false;
    IntegerArray IntList;
    int nCount;
    
    bExists = false;
    // 分割字符串
    SplitStringToInt(strList, STRING_SPLITTER, IntList);
    nCount = IntList.size();
    // 如果容量足够
    if (nCount < nMaxCount)
    {
        for (int i = 0; i < nCount; i++)
            if (IntList[i] == nNumber)
            {
                bExists = true;
                break;
            }
        if (!bExists)
        {
            if (!strList.empty()) strList += STRING_SPLITTER;
            strList += IntToStr(nNumber);
        }
        bResult = true;
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 将一个数字nNumber从字符串strList中删除，strList中的数以逗号分隔
// 参数:
//   strList    - 待处理的字符串
//   nNumber    - 将被删除的的数字
// 返回:
//   true   - 成功
//   false  - 失败 (没找到)
//-----------------------------------------------------------------------------
bool DeleteNumberFromList(string& strList, int nNumber)
{
    bool bResult;
    IntegerArray IntList;
    int nCount, nIndex;

    // 分割字符串
    SplitStringToInt(strList, STRING_SPLITTER, IntList);
    nCount = IntList.size();

    nIndex = -1;
    for (int i = 0; i < nCount; i++)
        if (IntList[i] == nNumber)
        {
            nIndex = i;
            break;
        }

    bResult = (nIndex != -1);
    
    if (bResult)
    {
        strList = "";
        for (int i = 0; i < nCount; i++)
        {
            if (i != nIndex)
            {
                if (!strList.empty()) strList += STRING_SPLITTER;
                strList += IntToStr(IntList[i]);
            }
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 取得 CAppDispatcher 对象指针
//-----------------------------------------------------------------------------
CAppDispatcher* GetAppDispatcher()
{
    return (CAppDispatcher*)(&Application.GetMainServer().GetDispatcher());
}

//-----------------------------------------------------------------------------
// 描述: 取得 CAppGlobalData 对象指针
//-----------------------------------------------------------------------------
CAppGlobalData* GetAppGlobalData()
{
    return &GetAppDispatcher()->GetGlobalData();
}

///////////////////////////////////////////////////////////////////////////////
