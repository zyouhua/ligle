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

//提前声明
class CTracker;
///////////////////////////////////////////////////////////////////////////////
// 数据包标志常量

// 数据包是否由服务器转发
const int PF_BYFORWARD                  = 0x0001;

///////////////////////////////////////////////////////////////////////////////
// 数据包首部:

#pragma pack(1)     // 1字节对齐

struct CUdpPacketHeader
{

    byte nProtoType;  //协议类型
    uint nActionCode;  //动作代码
    uint nSeqNumber;       //序列号
    uint nOrder;
    uint nUserIp;     //用户IP地址(内网）
    word wUdpPort;        //UDP监听端口
    word wTcpPort;        //TCP监听端口
    char strUserName[10];     //用户名
    char strUserPassword[10];    //用户密码

    word nFlags;            // 标志域 (见 PF_XXX 常量)
    word nReserved;         // 0
    uint nCheckSum;         // 校验和

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
// CUdpBizPacket - UDP业务包基类

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
// CNoopPacket - 空数据包

class CNoopPacket : public CUdpBizPacket
{
};

///////////////////////////////////////////////////////////////////////////////
// CRePacket - 应答包基类

class CRePacket : public CUdpBizPacket
{
public:
    int nResultCode;        // 结果代码 (RET_XXX)
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(int nResultCode);
};

///////////////////////////////////////////////////////////////////////////////
// 转发数据包

/*class CFwPktPacket : public CUdpBizPacket
{
public:
    uint nToPeerId;                     // 目标用户的PeerId
    CBuffer PacketBuffer;               // 待转发的包(原文)
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
