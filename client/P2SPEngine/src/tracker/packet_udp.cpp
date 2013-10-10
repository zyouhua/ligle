///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: packet_udp.cpp
// 功能描述: 业务层数据包基础协议
// 最后修改: 2006-01-16
///////////////////////////////////////////////////////////////////////////////

#include "packet_udp.h"
#include "icl_sysutils.h"
#include "tracker.h"
#include "server_peerserver.h"

//全局变量
extern CTracker Tracker;
extern CTrackerServer TrackerServer;

///////////////////////////////////////////////////////////////////////////////
// struct CUdpPacketHeader

//-----------------------------------------------------------------------------
// 描述: 数据包首部初始化
// 参数:
//   nSeqNumberA   - 为0时表示自动增长,否则直接赋值
//-----------------------------------------------------------------------------
void CUdpPacketHeader::Init(uint nActionCodeA, uint nSeqNumberA,uint nOrderA)
{
    ZeroBuffer(this, sizeof(CUdpPacketHeader));

    nProtoType = PP_UDP;      //包的协议
    nActionCode = nActionCodeA;    //包的动作代码
    //包的序列号
    if (nSeqNumberA == 0)
        nSeqNumber = Tracker.GetAllocator().AllocId() + 1;
    else
        nSeqNumber = nSeqNumberA;
    nOrder =  nOrderA;
    nUserIp = StringToIp(GetLocalIp());    //用户IP
    wUdpPort = TrackerServer.GetLocalPort();  //UDP端口
    wTcpPort = PeerTcpServer.GetLocalPort();    //TCP端口
    char * UserName = Tracker.GetstrUserName();
    char * UserPassword = Tracker.GetstrUserPassword();
    strcpy(strUserName,UserName);
    strcpy(strUserPassword,UserPassword);
    UpdateCheckSum();
}

//-----------------------------------------------------------------------------
// 描述: 检查校验和值是否正确
//-----------------------------------------------------------------------------
bool CUdpPacketHeader::CheckSumIsValid() const
{
    return nCheckSum == CalcCheckSum();
}

//-----------------------------------------------------------------------------
// 描述: 检查首部是否正确
//-----------------------------------------------------------------------------
bool CUdpPacketHeader::CheckHeaderIsValid() const
{
    return CheckSumIsValid() && (nReserved == 0);
}

//-----------------------------------------------------------------------------
// 描述: 计算校验和值
//-----------------------------------------------------------------------------
uint CUdpPacketHeader::CalcCheckSum() const
{
    return nProtoType ^nUserIp ^ wUdpPort ^ wTcpPort ^ nActionCode ^ nSeqNumber ^ nFlags ^ nReserved;
}

//-----------------------------------------------------------------------------
// 描述: 计算校验和值，并赋给 nCheckSum
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
        // 多份资料表明: vector 在内存中占用线性连续空间。
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
// 描述: 数据打包 (若出错则抛出异常)
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoPack()
{
    WriteBuffer(&Header, sizeof(Header));
}

//-----------------------------------------------------------------------------
// 描述: 数据解包 (若出错则抛出异常)
//-----------------------------------------------------------------------------
void CUdpBizPacket::DoUnpack()
{
    ReadBuffer(&Header, sizeof(Header));
}

//-----------------------------------------------------------------------------
// 描述: 数据包加密
// 备注: 此处加密算法不宜太复杂，以免加重服务器CPU负担。
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
// 描述: 数据包解密
// 备注: 此处加密算法不宜太复杂，以免加重服务器CPU负担。
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
