///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// packet_dtp.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _PACKET_DTP_H_
#define _PACKET_DTP_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"
#include "global.h"
#include "actcode_base.h"
#include "packet_base.h"

///////////////////////////////////////////////////////////////////////////////
// struct DTP - 首部:

#pragma pack(1)     // 1字节对齐

// DTP包通用首部
struct CDtpPacketHeader
{
    uint nReserved;         // 保留
    uint nActionCode;       // 动作代码
    uint nDataSize;         // 数据部分的字节数
public:
    CDtpPacketHeader() { Clear(); }

    void Clear() { bzero(this, sizeof(*this)); }
    void Init(uint nActionCode, uint nDataSize);
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// class CDtpPacket - DTP包基类

class CDtpPacket : public CPacket
{
public:
    CDtpPacketHeader Header;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
    virtual void DoAfterPack();
public:
    void InitHeader(uint nActionCode);
};

///////////////////////////////////////////////////////////////////////////////
// class CDtpGreetPacket - DTP 握手包基类

class CDtpGreetPacket : public CDtpPacket {};

///////////////////////////////////////////////////////////////////////////////
// class CDtpRePacket

class CDtpRePacket : public CDtpPacket
{
public:
    uint nResultCode;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(uint nResultCode);
};

///////////////////////////////////////////////////////////////////////////////
// class CInvConnGreetPacket - 反向DTP连接 握手包

class CInvConnGreetPacket : public CDtpGreetPacket
{
public:
    uint nConnectorId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(uint nConnectorId);
};

///////////////////////////////////////////////////////////////////////////////

#endif // _PACKET_DTP_H_
