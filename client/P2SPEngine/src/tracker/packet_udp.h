///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// packet_udp.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _PACKET_UDP_H_
#define _PACKET_UDP_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "global.h"
#include "actcode_base.h"
#include "packet_base.h"

//��ǰ����
class CTracker;
///////////////////////////////////////////////////////////////////////////////
// ���ݰ���־����

// ���ݰ��Ƿ��ɷ�����ת��
const int PF_BYFORWARD                  = 0x0001;

///////////////////////////////////////////////////////////////////////////////
// ���ݰ��ײ�:

#pragma pack(1)     // 1�ֽڶ���

struct CUdpPacketHeader
{

    byte nProtoType;  //Э������
    uint nActionCode;  //��������
    uint nSeqNumber;       //���к�
    uint nOrder;
    uint nUserIp;     //�û�IP��ַ(������
    word wUdpPort;        //UDP�����˿�
    word wTcpPort;        //TCP�����˿�
    char strUserName[10];     //�û���
    char strUserPassword[10];    //�û�����

    word nFlags;            // ��־�� (�� PF_XXX ����)
    word nReserved;         // 0
    uint nCheckSum;         // У���

private:
    uint CalcCheckSum() const;
public:
    void Init(uint nActionCodeA, uint nSeqNumberA = 0, uint nOrderA = 0);
    bool CheckSumIsValid() const;
    bool CheckHeaderIsValid() const;
    void UpdateCheckSum();
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// CUdpBizPacket - UDPҵ�������

class CUdpBizPacket : public CPacket
{
public:
    CUdpPacketHeader Header;

protected:
    void ReadIntegerArray(IntegerArray& Array);
    void WriteIntegerArray(const IntegerArray& Array);
    void ReadStringWithDecompress(string& str);
    void WriteStringWithCompress(string& str);
protected:
    virtual void DoPack();
    virtual void DoUnpack();
    virtual void DoEncrypt();
    virtual void DoDecrypt();
public:
    inline void InitHeader(uint nActionCodeA, int nSeqNumberA = 0,uint nOrderA = 0)
        { Header.Init(nActionCodeA, nSeqNumberA); }
};

///////////////////////////////////////////////////////////////////////////////
// CNoopPacket - �����ݰ�

class CNoopPacket : public CUdpBizPacket
{
};

///////////////////////////////////////////////////////////////////////////////
// CRePacket - Ӧ�������

class CRePacket : public CUdpBizPacket
{
public:
    int nResultCode;        // ������� (RET_XXX)
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(int nResultCode);
};

///////////////////////////////////////////////////////////////////////////////
// ת�����ݰ�

/*class CFwPktPacket : public CUdpBizPacket
{
public:
    uint nToPeerId;                     // Ŀ���û���PeerId
    CBuffer PacketBuffer;               // ��ת���İ�(ԭ��)
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(
        uint nToPeerId,
        void *pPacketBuffer,
        int nPacketSize
        );
};                 */


///////////////////////////////////////////////////////////////////////////////

#endif // _PACKET_UDP_H_ 
