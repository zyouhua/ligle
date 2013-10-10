///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: packet_dtp.cpp
// 功能描述: 业务层数据包基础协议
// 最后修改: 2006-01-16
///////////////////////////////////////////////////////////////////////////////

#include "packet_dtp.h"
#include "utilities.h"

///////////////////////////////////////////////////////////////////////////////
// struct CDtpPacketHeader

void CDtpPacketHeader::Init(uint nActionCode, uint nDataSize)
{
    this->nActionCode = nActionCode;
    this->nDataSize = nDataSize;
}

///////////////////////////////////////////////////////////////////////////////
// class CDtpPacket

void CDtpPacket::DoPack()
{
    WriteBuffer(&Header, sizeof(CDtpPacketHeader));
}

void CDtpPacket::DoUnpack()
{
    ReadBuffer(&Header, sizeof(CDtpPacketHeader));
}

void CDtpPacket::DoAfterPack()
{
    uint nDataSize = GetSize() - sizeof(Header);
    
    Header.nDataSize = nDataSize;
    ((CDtpPacketHeader*)GetBuffer())->nDataSize = nDataSize;
}

//-----------------------------------------------------------------------------
// 描述: 初始化DTP包的首部
//-----------------------------------------------------------------------------
void CDtpPacket::InitHeader(uint nActionCode)
{
    Header.Init(nActionCode, 0);
}

///////////////////////////////////////////////////////////////////////////////
// CDtpRePacket

void CDtpRePacket::DoPack()
{
    CDtpPacket::DoPack();

    WriteBuffer(&nResultCode, sizeof(uint));
}

void CDtpRePacket::DoUnpack()
{
    CDtpPacket::DoUnpack();

    ReadBuffer(&nResultCode, sizeof(uint));
}

void CDtpRePacket::InitData(uint nResultCode)
{
    this->nResultCode = nResultCode;

    Pack();
}

///////////////////////////////////////////////////////////////////////////////
// class CInvConnGreetPacket

void CInvConnGreetPacket::DoPack()
{
    CDtpPacket::DoPack();

    WriteBuffer(&nConnectorId, sizeof(uint));
}

void CInvConnGreetPacket::DoUnpack()
{
    CDtpPacket::DoUnpack();

    ReadBuffer(&nConnectorId, sizeof(uint));
}

void CInvConnGreetPacket::InitData(uint nConnectorId)
{
    this->nConnectorId = nConnectorId;
    
    Pack();
}

///////////////////////////////////////////////////////////////////////////////
