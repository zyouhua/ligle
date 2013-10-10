///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: packet_base.cpp
// ��������: ���ݰ��������
// ����޸�: 2005-09-13
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
// ����: �̶��ַ����ĳ���
//-----------------------------------------------------------------------------
void CPacket::FixStrLength(string& str, int nLength)
{
    if ((int)str.length() != nLength)
        str.resize(nLength, 0);
}

//-----------------------------------------------------------------------------
// ����: �����ַ�������󳤶�
//-----------------------------------------------------------------------------
void CPacket::TruncString(string& str, int nMaxLength)
{
    if ((int)str.length() > nMaxLength)
        str.resize(nMaxLength);
}

//-----------------------------------------------------------------------------
// ����: ���ݴ��
// ����:
//   true  - �ɹ�
//   false - ʧ��
// ��ע: 
//   ���������ݿ��� CPacket.GetBuffer �� CPacket.GetSize ȡ����
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
// ����: ���ݽ��
// ����:
//   true  - �ɹ�
//   false - ʧ�� (���ݲ��㡢��ʽ�����)
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
// ����: ������ݰ�����
//-----------------------------------------------------------------------------
void CPacket::Clear()
{
    delete m_pStream;
    m_pStream = NULL;
    m_bAvailable = false;
    m_bIsPacked = false;
}

//-----------------------------------------------------------------------------
// ����: ȷ�������Ѵ��
//-----------------------------------------------------------------------------
void CPacket::EnsurePacked()
{
    if (!IsPacked()) Pack();
}

///////////////////////////////////////////////////////////////////////////////
