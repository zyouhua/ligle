///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: packet_base.cpp
// 功能描述: 数据包定义基类
// 最后修改: 2005-09-13
///////////////////////////////////////////////////////////////////////////////

#include "packet_base.h"

///////////////////////////////////////////////////////////////////////////////
// class CPacket

CPacket::CPacket()
{
    Init();
}

CPacket::~CPacket()
{
    Clear();
}

void CPacket::Init()
{
    m_pStream = NULL;
    m_bAvailable = false;
    m_bIsPacked = false;
}

void CPacket::RaiseUnpackError()
{
    throw CException("packet unpack error", __FILE__, __LINE__);
}

void CPacket::RaisePackError()
{
    throw CException("packet pack error", __FILE__, __LINE__);
}

void CPacket::CheckUnsafeSize(int nValue)
{
    if (nValue < 0 || nValue > word(-1))
        throw CException("unsafe value in packet", __FILE__, __LINE__);
}

void CPacket::ReadBuffer(void *pBuffer, int nBytes)
{
    if (m_pStream->Read(pBuffer, nBytes) != nBytes)
        RaiseUnpackError();
}

void CPacket::WriteBuffer(void *pBuffer, int nBytes)
{
    m_pStream->Write(pBuffer, nBytes);
}

void CPacket::ReadString(string& str)
{
    word nSize;

    str.clear();
    if (m_pStream->Read(&nSize, sizeof(word)) == sizeof(word))
    {    
        CheckUnsafeSize(nSize);
        if (nSize > 0)
        {  
            str.resize(nSize);
            if (m_pStream->Read((void*)str.data(), nSize) != nSize)
                RaiseUnpackError();
               
        }
    }
    else
    {
        RaiseUnpackError();
    }
}

void CPacket::WriteString(const string& str)
{
    word nSize;

    nSize = str.length();
    m_pStream->Write(&nSize, sizeof(word));
    if (nSize > 0)
        m_pStream->Write((void*)str.c_str(), nSize);
}

void CPacket::ReadBlob(string& str)
{
    ReadString(str);
}

void CPacket::ReadBlob(CStream& Stream)
{
    string str;

    ReadBlob(str);
    Stream.SetSize(0);
    if (!str.empty())
        Stream.Write((void*)str.c_str(), str.length());
}

void CPacket::ReadBlob(CBuffer& Buffer)
{
    string str;

    ReadBlob(str);
    Buffer.SetSize(0);
    if (!str.empty())
        Buffer.Assign((void*)str.c_str(), str.length());
}

void CPacket::WriteBlob(void *pBuffer, int nBytes)
{
    if (nBytes < 0) return;

    string str;
    str.assign((char*)pBuffer, nBytes);
    WriteString(str);
}

//-----------------------------------------------------------------------------
// 描述: 固定字符串的长度
//-----------------------------------------------------------------------------
void CPacket::FixStrLength(string& str, int nLength)
{
    if ((int)str.length() != nLength)
        str.resize(nLength, 0);
}

//-----------------------------------------------------------------------------
// 描述: 限制字符串的最大长度
//-----------------------------------------------------------------------------
void CPacket::TruncString(string& str, int nMaxLength)
{
    if ((int)str.length() > nMaxLength)
        str.resize(nMaxLength);
}

//-----------------------------------------------------------------------------
// 描述: 数据打包
// 返回:
//   true  - 成功
//   false - 失败
// 备注: 
//   打包后的数据可由 CPacket.GetBuffer 和 CPacket.GetSize 取出。
//-----------------------------------------------------------------------------
bool CPacket::Pack()
{
    bool bResult;

    try
    {
        delete m_pStream;
        m_pStream = new CMemoryStream(DEFAULT_MEMORY_DELTA);
        DoPack();
        DoAfterPack();
        DoEncrypt();
        m_bAvailable = true;
        m_bIsPacked = true;
        bResult = true;
    }
    catch (CException& e)
    { 
        bResult = false;
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 数据解包
// 返回:
//   true  - 成功
//   false - 失败 (数据不足、格式错误等)
//-----------------------------------------------------------------------------
bool CPacket::Unpack(void *pBuffer, int nBytes)
{
    bool bResult;
    try
    {
        delete m_pStream;
        m_pStream = new CMemoryStream(DEFAULT_MEMORY_DELTA);
        m_pStream->SetSize(nBytes);
        MoveBuffer(pBuffer, m_pStream->GetMemory(), nBytes);
        DoDecrypt();
        DoUnpack(); 
        m_bAvailable = true;
        m_bIsPacked = false;
        bResult = true;
    }
    catch (CException& e)
    { 
        bResult = false;
        Clear();
    }

    delete m_pStream;
    m_pStream = NULL;

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 清空数据包内容
//-----------------------------------------------------------------------------
void CPacket::Clear()
{
    delete m_pStream;
    m_pStream = NULL;
    m_bAvailable = false;
    m_bIsPacked = false;
}

//-----------------------------------------------------------------------------
// 描述: 确保数据已打包
//-----------------------------------------------------------------------------
void CPacket::EnsurePacked()
{
    if (!IsPacked()) Pack();
}

///////////////////////////////////////////////////////////////////////////////
