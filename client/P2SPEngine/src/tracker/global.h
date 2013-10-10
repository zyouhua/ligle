///////////////////////////////////////////////////////////////////////////////
// P2SP Client Daemon (P2SP_CD)
//
// 文件名称: global.h
// 功能描述: 全局常量、类型定义
// 最后修改: 2005-12-22
///////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"

#include "icl_types.h"
#include "icl_sysutils.h"
#include "icl_socket.h"

using namespace icl;

///////////////////////////////////////////////////////////////////////////////

// disable warning: "cannot create pre-compiled header"
#ifdef __BORLANDC__
#pragma warn -pch
#endif

///////////////////////////////////////////////////////////////////////////////
// 项目名相关常量

// 软件名称 (用于商务)
const char* const SOFTWARE_NAME         = "p2spcd";
// 项目名称 (用于内部开发)
const char* const PROJECT_NAME          = "p2spcd";

///////////////////////////////////////////////////////////////////////////////
// 文件名相关常量

// 日志文件名 (不含路径)
const char* const LOG_FILENAME          = "log";
// 持久信息文件名 (不含路径)
const char* const PERSISTENT_FILENAME   = "config";
// 元文件目录名
const char* const METAFILES_DIR_NAME    = "metafiles";

///////////////////////////////////////////////////////////////////////////////
// 网络相关常量

// Tracker 服务器的地址和端口
const char* const TRACKER_SERVER_ADDR   = "192.168.0.108";
const int TRACKER_SERVER_UDP_PORT       = 9100;

// 客户端的端口
const int CLIENT_UDP_PORT_START         = 7752;       // 客户端UDP起始端口
const int CLIENT_TCP_PORT_START         = 7752;       // 客户端TCP起始端口
const int CHECK_PORT_TIMES              = 1000;       // 客户端绑定端口的重试次数

///////////////////////////////////////////////////////////////////////////////
// BT 相关常量

// 块的大小(字节数)
const int BT_PIECE_SIZE                 = 1024*256;
// 片的大小(字节数)
const int BT_SLICE_SIZE                 = 1024*16;
// 一个块中的片数
const int BT_SLICES_PER_PIECE           = BT_PIECE_SIZE / BT_SLICE_SIZE;

// BT连接中，BT数据包的最大字节数
const int BT_MAX_PACKET_SIZE            = 1024*1024*1;
// BT文件的最大字节数(8G)
const int64 BT_MAX_FILE_SIZE            = (int64)1024*1024*1024*8;
// 下载一个文件时，最多同时下载几个块
const int BT_MAX_RECVING_PIECES         = 4;
// 流水线请求的最大数量
const int BT_MAX_REQUESTS               = 8;
// 系统最大总连接数 (针对普通用户)
const int BT_MAX_CONNS_FOR_PEER         = 80;
// 系统最大总连接数 (针对CDN节点)
const int BT_MAX_CONNS_FOR_CDN          = 10000;

// 下载一个文件时，启动多少个连接
const int BT_CONNS_PER_RECVING_FILE     = 10;
// 对于普通用户，下载一个文件的连接中最多可有几个CDN节点连接
const int BT_MAX_CDN_CONN_FOR_PEER      = 1;
// 每次获取多少 Peers
const int BT_GET_PEERS_PER_TIME         = 30;
// 下载一个文件时，如果(总下载速度 - CDN下载速度) > 此值，则断开CDN连接。(Bytes/S)
const int BT_DISCARD_CDN_COND_RATE      = 1024*100;
// 下载一个文件时，如果下载速度 < 此值，则断掉低速连接。(Bytes/S)
const int BT_DISCARD_WEAK_COND_RATE     = 1024*100;
// 速率小于此值的连接，被认为是低速连接。(Bytes/S)
const int BT_WEAK_CONN_RATE             = 1024*5;
// 系统总连接数超过 此值% 时，将清理低速连接
const int BT_CONNS_ALERT_PERCENT        = 90;

// 文件ID的长度
const int BT_FILE_ID_SIZE               = 28;
// BT连接握手的超时时间(毫秒)
const int BT_HANDSHAKE_TIMEOUT          = 1000*60*1;
// 下载一个块时，超过此限制则认为超时(毫秒)
const int BT_PIECE_RECV_TIMEOUT         = 1000*30;
// 下载一个片时，超过此限制则认为超时(毫秒)
const int BT_SLICE_RECV_TIMEOUT         = 1000*5;
// BT连接哈希表中桶的数量(素数: 1543,3079,6151,12289,24593,49157,98317,196613,...)
const int BT_CONN_HASH_TABLE_BUCKETS    = 12289;     // (占用48K)

///////////////////////////////////////////////////////////////////////////////
// 项目可调参数

// DTP连接在接受处理器中的最长停留时间(毫秒)
const int DTP_ACCEPT_TIMEOUT            = 1000*60;
// DTP连接接受处理器的最大容量
const int DTP_ACCEPT_HANDLER_CAPACITY   = 10000;

// TCP正向连接超时时间(毫秒)
const int TCP_NORMAL_CONN_TIMEOUT       = 1000*15;
// TCP反向连接超时时间(毫秒)
const int TCP_INVERT_CONN_TIMEOUT       = 1000*30;
// TCP连接发送握手包的超时时间(毫秒)
const int TCP_SEND_GREET_PKT_TIMEOUT    = 1000*30;

// 发送UDP包的默认重试次数
const int DEF_UDP_SEND_TRY_TIMES        = 5;
// 等待接收UDP包的默认时限(毫秒)
const int DEF_UDP_SEND_RECV_TIMEOUT     = 1000*3;

// 系统垃圾收集的时间间隔(毫秒)
const int COLLECT_GARBAGE_INTERVAL      = 1000*10;

///////////////////////////////////////////////////////////////////////////////
// 公用结果代码

const int RET_SUCCESS                   = 0;        // 成功
const int RET_FAILURE                   = 1;        // 失败

const int RET_BASE                      = 100;      // 非公用结果代码必须 >= 此值!

///////////////////////////////////////////////////////////////////////////////
// 类型定义

#pragma pack(1)     // 1字节对齐

// 协议类型(包首部的第一个字节)
enum PacketProto {
    PP_UDP         = 1,         // UDP业务包
    PP_UTP         = 2,         // UTP数据包
};
enum { MAX_PACKET_PROTO = 2 };  // 协议类型最大数量

// 传输方向
enum TransferDir {
    TD_SEND,                    // 发送
    TD_RECV                     // 接收
};

// 用户连接方式
enum ConnectType {
    CT_UNKNOWN      = 0,        // 不明类型
    CT_DIRECT       = 1,        // 直连
    CT_SOCKS5       = 2,        // SOCKS5
    CT_LAN          = 3         // 局域网
};

// Peer 类型
enum PeerType {
    PT_UNKNOWN      = 0,        // 未知用户
    PT_PEER         = 1,        // 普通用户
    PT_CDN          = 2         // CDN 节点
};

// 下载状态
enum DownloadState {
    DS_UNKNOWN      = 0,        // 未知状态
    DS_DOWNLOAD     = 1,        // 正在下载
    DS_PAUSE        = 2,        // 暂停下载
    DS_COMPLETE     = 3,        // 下载完成
    DS_FAILURE      = 4         // 下载失败
};

// 任务类型
enum TaskKind {
    TK_INCOMPLETE   = 0,        // 未完成
    TK_COMPLETE     = 1,        // 已完成
    TK_ALL          = 2         // 全部
};

// 服务器地址
struct CServerAddress
{
    uint nIp;
    word nUdpPort;
    word nTcpPort;

    CServerAddress() { Clear(); }
    void Clear() { ZeroBuffer(this, sizeof(*this)); }
    CPeerAddress GetUdpAddr() const { return CPeerAddress(nIp, nUdpPort); }
    CPeerAddress GetTcpAddr() const { return CPeerAddress(nIp, nTcpPort); }
};

// 用户信息 (用于GetPeerList)
struct CPeerInfo
{
    uint nPeerId;               // PeerId
    uint nIp;                   // IP (对我方而言可用的IP，下同)
    word nUdpPort;              // UDP Port
    word nTcpPort;              // TCP Port
    byte nPeerType;             // Peer 类型

public:    
    CPeerInfo() { Clear(); }
    void Clear() { ZeroBuffer(this, sizeof(*this)); }
    CPeerAddress GetUdpAddr() const { return CPeerAddress(nIp, nUdpPort); }
    CPeerAddress GetTcpAddr() const { return CPeerAddress(nIp, nTcpPort); }
};

typedef vector<CPeerInfo> PeerInfoList;

// 文件信息 (用于UploadFileList)
struct CFileInfo
{
    string strFileId;

public:
    CFileInfo() {}
    CFileInfo(const string& strFileId) { this->strFileId = strFileId; }
};

typedef vector<CFileInfo> FileInfoList;

#pragma pack()

///////////////////////////////////////////////////////////////////////////////

#endif // _GLOBAL_H_
