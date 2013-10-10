///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: utilities.cpp
// ��������: ��ҵ���йص������
// ����޸�: 2005-04-27
///////////////////////////////////////////////////////////////////////////////

#include "utilities.h"
#include "hash_sha1.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// ����: �����ݽ���Base64����
// ����:
//   pData  - �����б��������
//   nSize  - pData�е��ֽ���
// ����:
//   �������ַ���(����'\0'�ַ�)
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
// ����: ��������� HASH ����
// ����: SHA1 + Base64 ��������Ľ��������#0�ַ�����28�ֽڡ�
//-----------------------------------------------------------------------------
string HashPassword(const string& strPassword)
{
    const int DIGEST_SIZE = 20;
    char chDigest[DIGEST_SIZE];

    HashSha1(strPassword.c_str(), strPassword.length(), chDigest);
    return StrToBase64(chDigest, DIGEST_SIZE);
}

//-----------------------------------------------------------------------------
// ����: ����ѹ�� (ZLIB)
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
// ����: ���ݽ�ѹ (ZLIB)
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

                // �����ѹ�ɹ�
                if (r == Z_OK)
                {
                    bResult = true;
                    break;
                }
                // �������������ռ䲻��
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
// ����: MBCS -> Unicode
// ��ע: ת������������ǰ�ַ��� ( ���� setlocale(LC_CTYPE, ...) ����)
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
// ����: ��������������ָ���ָ��������������������ַ���
// ����:
//   IntList    - ��������
//   chSplitter - �ָ���������","
// ����:
//   ����֮����ַ������� "123,456,789"
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
// ����: ��һ������nNumber���뵽�ַ���strList�У��Զ��ŷָ�
// ����:
//   strList    - �����ּ��뵽����ַ�����
//   nNumber    - �����������
//   nMaxCount  - �ַ���strList���������Ŷ��ٸ�����
//   bExists    - �����ַ������Ƿ��Ѿ������������
// ����:
//   true   - �ɹ�
//   false  - ʧ�� (��������)
//-----------------------------------------------------------------------------
bool AddNumberToList(string& strList, int nNumber, int nMaxCount, bool& bExists)
{
    bool bResult = false;
    IntegerArray IntList;
    int nCount;
    
    bExists = false;
    // �ָ��ַ���
    SplitStringToInt(strList, STRING_SPLITTER, IntList);
    nCount = IntList.size();
    // ��������㹻
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
// ����: ��һ������nNumber���ַ���strList��ɾ����strList�е����Զ��ŷָ�
// ����:
//   strList    - ��������ַ���
//   nNumber    - ����ɾ���ĵ�����
// ����:
//   true   - �ɹ�
//   false  - ʧ�� (û�ҵ�)
//-----------------------------------------------------------------------------
bool DeleteNumberFromList(string& strList, int nNumber)
{
    bool bResult;
    IntegerArray IntList;
    int nCount, nIndex;

    // �ָ��ַ���
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
// ����: ȡ�� CAppDispatcher ����ָ��
//-----------------------------------------------------------------------------
CAppDispatcher* GetAppDispatcher()
{
    return (CAppDispatcher*)(&Application.GetMainServer().GetDispatcher());
}

//-----------------------------------------------------------------------------
// ����: ȡ�� CAppGlobalData ����ָ��
//-----------------------------------------------------------------------------
CAppGlobalData* GetAppGlobalData()
{
    return &GetAppDispatcher()->GetGlobalData();
}

///////////////////////////////////////////////////////////////////////////////
