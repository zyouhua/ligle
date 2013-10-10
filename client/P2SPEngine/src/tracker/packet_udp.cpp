///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: packet_udp.cpp
// ��������: ҵ������ݰ�����Э��
// ����޸�: 2006-01-16
///////////////////////////////////////////////////////////////////////////////

#include "packet_udp.h"
#include "icl_sysutils.h"
#include "tracker.h"
#include "server_peerserver.h"

//ȫ�ֱ���
extern CTracker Tracker;
extern CTrackerServer TrackerServer;

///////////////////////////////////////////////////////////////////////////////
// struct CUdpPacketHeader

//-----------------------------------------------------------------------------
// ����: ���ݰ��ײ���ʼ��
// ����:
//   nSeqNumberA   - Ϊ0ʱ��ʾ�Զ�����,����ֱ�Ӹ�ֵ
//-----------------------------------------------------------------------------
void CUdpPacketHeader::Init(uint nActionCodeA, uint nSeqNumberA,uint nOrderA)
{
    ZeroBuffer(this, sizeof(CUdpPacketHeader));

    nProtoType = PP_UDP;      //����Э��
    nActionCode = nActionCodeA;    //���Ķ�������
    //�������к�
    if (nSeqNumberA == 0)
        nSeqNumber = Tracker.GetAllocator().AllocId() + 1;
    else
        nSeqNumber = nSeqNumberA;
    nOrder =  nOrderA;
    nUserIp = StringToIp(GetLocalIp());    //�û�IP
    wUdpPort = TrackerServer.GetLocalPort();  //UDP�˿�
    wTcpPort = PeerTcpServer.GetLocalPort();    //TCP�˿�
    char * UserName = Tracker.GetstrUserName();
    char * UserPassword = Tracker.GetstrUserPassword();
    strcpy(strUserName,UserName);
    strcpy(strUserPassword,UserPassword);
    UpdateCheckSum();
}

//-----------------------------------------------------------------------------
// ����: ���У���ֵ�Ƿ���ȷ
//-----------------------------------------------------------------------------
bool CUdpPacketHeader::CheckSumIsValid() const
{
    return nCheckSum == CalcCheckSum();
}

//-----------------------------------------------------------------------------
// ����: ����ײ��Ƿ���ȷ
//-----------------------------------------------------------------------------
bool CUdpPacketHeader::CheckHeaderIsValid() const
{
    return CheckSumIsValid() && (nReserved == 0);
}

//-----------------------------------------------------------------------------
// ����: ����У���ֵ
//-----------------------------------------------------------------------------
uint CUdpPacketHeader::CalcCheckSum() const
{
    return nProtoType ^nUserIp ^ wUdpPort ^ wTcpPort ^ nActionCode ^ nSeqNumber ^ nFlags ^ nReserved;
}

//-----------------------------------------------------------------------------
// ����: ����У���ֵ�������� nCheckSum
//-----------------------------------------------------------------------------
void CUdpPacketHeader::UpdateCheckSum()
{
    nCheckSum = CalcCheckSum();
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpBizPacket

void CUdpBizPacket::ReadIntegerArray(IntegerArray& Array)
{
    int nCount;

    ReadBuffer(&nCount, sizeof(int));
    if (nCount > word(-1))
    {
        Array.clear();
        return;
    }

    Array.resize(nCount);
    if (nCount > 0)
    {
        // ������ϱ���: vector ���ڴ���ռ�����������ռ䡣
        ReadBuffer((void*)&Array[0], sizeof(int) * nCount);
    }
}

void CUdpBizPacket::WriteIntegerArray(const IntegerArray& Array)
{
    int nCount;

    nCount = Array.size();
    WriteBuffer(&nCount, sizeof(int));
    if (nCount > 0)
        WriteBuffer((void*)&Array[0], sizeof(int) * nCount);
}

void CUdpBizPacket::ReadStringWithDecompress(string& str)
{
    CBuffer Buffer;

    str.clear();
    ReadBlob(Buffer);
    if (Buffer.GetSize() > 0)
    {
        if (ZlibUncompress(Buffer))
            str.assign(Buffer.Data(), Buffer.GetSize());
    }
}

void CUdpBizPacket::WriteStringWithCompress(string& str)
{
    CBuffer Buffer(str.c_str(), str.length());

    if (Buffer.GetSize() > 0)
        ZlibCompress(Buffer);
    WriteBlob(Buffer.Data(), Buffer.GetSize());
}

//-----------------------------------------------------------------------------
// ����: ���ݴ�� (���������׳��쳣)
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoPack()
{
    WriteBuffer(&Header, sizeof(Header));
}

//-----------------------------------------------------------------------------
// ����: ���ݽ�� (���������׳��쳣)
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoUnpack()
{
    ReadBuffer(&Header, sizeof(Header));
}

//-----------------------------------------------------------------------------
// ����: ���ݰ�����
// ��ע: �˴������㷨����̫���ӣ�������ط�����CPU������
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoEncrypt()
{
    char *pBuffer = (char*)m_pStream->GetMemory();
    int nBytes = m_pStream->GetSize();
    char chKey;

    if (nBytes <= sizeof(CUdpPacketHeader)) return;
    chKey = ((CUdpPacketHeader*)pBuffer)->nActionCode ^ ((CUdpPacketHeader*)pBuffer)->nUserIp;
    pBuffer += sizeof(CUdpPacketHeader);
    nBytes -= sizeof(CUdpPacketHeader);

    pBuffer[0] = ~pBuffer[0] ^ chKey;
    for (register int i = 1; i < nBytes; i++)
        pBuffer[i] = ~pBuffer[i - 1] ^ chKey ^ pBuffer[i];
}

//-----------------------------------------------------------------------------
// ����: ���ݰ�����
// ��ע: �˴������㷨����̫���ӣ�������ط�����CPU������
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoDecrypt()
{
    char *pBuffer = (char*)m_pStream->GetMemory();
    int nBytes = m_pStream->GetSize();
    char chKey;

    if (nBytes <= sizeof(CUdpPacketHeader)) return;
    chKey = ((CUdpPacketHeader*)pBuffer)->nActionCode ^ ((CUdpPacketHeader*)pBuffer)->nUserIp;
    pBuffer += sizeof(CUdpPacketHeader);
    nBytes -= sizeof(CUdpPacketHeader);

    for (register int i = nBytes - 1; i >= 1; i--)
        pBuffer[i] = ~pBuffer[i - 1] ^ chKey ^ pBuffer[i];
    pBuffer[0] = ~pBuffer[0] ^ chKey;
}

///////////////////////////////////////////////////////////////////////////////
// class CRePacket

void CRePacket::DoPack()
{
    CUdpBizPacket::DoPack();
    WriteBuffer(&nResultCode, sizeof(int));
}

void CRePacket::DoUnpack()
{
    CUdpBizPacket::DoUnpack();
    ReadBuffer(&nResultCode, sizeof(int));
}

void CRePacket::InitData(int nResultCode)
{
    this->nResultCode = nResultCode;
    Pack();
}

///////////////////////////////////////////////////////////////////////////////
// class CFwPktPacket

/*void CFwPktPacket::DoPack()
{
    CUdpBizPacket::DoPack();

    WriteBuffer(&nToPeerId, sizeof(uint));
    WriteBlob(PacketBuffer.Data(), PacketBuffer.GetSize());
}

void CFwPktPacket::DoUnpack()
{
    CUdpBizPacket::DoUnpack();

    ReadBuffer(&nToPeerId, sizeof(uint));
    ReadBlob(PacketBuffer);
}

void CFwPktPacket::InitData(
    uint nToPeerId,
    void *pPacketBuffer,
    int nPacketSize)
{
    this->nToPeerId = nToPeerId;
    this->PacketBuffer.Assign(pPacketBuffer, nPacketSize);

    Pack();
}     */

///////////////////////////////////////////////////////////////////////////////
