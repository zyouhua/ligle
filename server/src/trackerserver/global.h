///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// 文件名称: global.h
// 功能描述: 全局常量、类型定义
// 最后修改: 2005-12-1
///////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_types.h"

///////////////////////////////////////////////////////////////////////////////
// 常量定义

//-----------------------------------------------------------------------------
//-- 文件常量:

// 日志文件名 (不含路径)
#define SERVER_LOG_FILE         "log"

// 配置文件名 (不含路径)
#define SERVER_CONFIG_FILE      "config.xml"

//-----------------------------------------------------------------------------
//-- 网络常量:

// 网络数据包协议版本代号
const int PROTOCOL_VER_1                = 1;

//----------------------------------------------------------------------------
//-- 数组尺寸常量:

const int MAX_UDP_FILE_LIST_COUNT = 19;     // 一个包中最大能放置File的个数
const int MAX_UDP_PEER_LIST_COUNT = 66;     // 一个包中最大能放置Peer的个数
//const int MAX_HASHCODE_LENGTH = 20;
const int MAX_MD5_LENGTH = 30;              // MD5字串的最大长度
const int MAX_FILE_ID_LENGTH = 28;          // FileId字串的最大长度
const int MAX_URL_LENGTH = 260;             // URL字串的最大长度
const int MAX_IP_STRING_LENGTH = 15;        // 字串类型的IP最大长度
const int MAX_IP_RANGE_DESC_LENGTH = 260;   // IP段描述的最大长度

#pragma pack(1)     // 1字节对齐

///////////////////////////////////////////////////////////////////////////////
// 类型定义

typedef string HashCode;
//typedef string Md5Code;
//typedef int PeerId;
//typedef string FileId;

///////////////////////////////////////////////////////////////////////////////
// 结果代码定义

// 公用结果代码
const int RET_SUCCESS                   = 0;        // 成功
const int RET_FAILURE                   = 1;        // 失败

const int RET_BASE                      = 100;      // 非公用结果代码必须 >= 此值!


///////////////////////////////////////////////////////////////////////////////
// 杂项定义

// 协议类型(包首部的第一个字节)
enum PacketProto {
    PP_UDP      = 1,        // TRACKER协议数据包
    PP_UTP          = 2,        // UTP数据包
};
enum { MAX_PACKET_PROTO = 2 };  // 协议类型最大数量

// 用户的连接方式
enum ConnectType {
    CT_UNKNOWN      = 0,        // 不明类型
    CT_DIRECT       = 1,        // 直连
    CT_SOCKS5       = 2,        // SOCKS5
    CT_LAN          = 3         // 局域网
};

// 用户连上互联网的网络类型
enum NetType {
    NT_UNKNOWN      = 0         // 在数据库中无法找到网络的类型
    // 其他运营商分配规则见iprangetype.h
};

// Peer 类型
enum PeerType {
    PT_UNKNOWN      = 0,        // 未知用户
    PT_PEER         = 1,        // 普通用户
    PT_SUPER_PEER   = 2         // CDN 节点
};    

// 用户信息
/*struct CPeerInfo
{
    uint nPeerId;               // PeerId
    uint nIp;                   // 对外IP，如Proxy的IP
    word nUdpPort;              // 对外UDP Port，如Proxy分配的Port
    word nTcpPort;              // 对外TCP Port
    uint nInternalIp;           // 本机IP
    word nInternalUdpPort;      // 本机UDP Port
    word nInternalTcpPort;      // 本机TCP Port
    byte nConnectType;          // 上网方式
    byte nPeerType;             // Peer 类型(enum PeerType)
    
    CPeerInfo() { Clear(); }
    void Clear() { bzero(this, sizeof(*this)); }
};*/

// 文件通知的类型
enum NotifyFileState
{
    NFS_START_DOWNLOAD = 1,     // 启动一个新下载
    NFS_PAUSE_DOWNLOAD = 2,     // 暂停下载
    NFS_RESUME_DOWNLOAD = 3,    // 继续下载
    NFS_COMPLETE_DOWNLOAD = 4   // 完成下载
};

#pragma pack()


///////////////////////////////////////////////////////////////////////////////

#endif // _GLOBAL_H_ 
