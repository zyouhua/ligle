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
// struct DTP - �ײ�:

#pragma pack(1)     // 1�ֽڶ���

// DTP��ͨ���ײ�
struct CDtpPacketHeader
{
    uint nReserved;         // ����
    uint nActionCode;       // ��������
    uint nDataSize;         // ���ݲ��ֵ��ֽ���
public:
    CDtpPacketHeader() { Clear(); }

    void Clear() { bzero(this, sizeof(*this)); }
    void Init(uint nActionCode, uint nDataSize);
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// class CDtpPacket - DTP������

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
// class CDtpGreetPacket - DTP ���ְ�����

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
// class CInvConnGreetPacket - ����DTP���� ���ְ�

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
