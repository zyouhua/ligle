///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: actcode_base.h
// 功能描述: 基本动作代码定义
// 最后修改: 2006-01-05
///////////////////////////////////////////////////////////////////////////////

#ifndef _ACTCODE_BASE_H_
#define _ACTCODE_BASE_H_

///////////////////////////////////////////////////////////////////////////////
// 宏定义

#define MAKE_ACTION_CODE(dir, pro, biz, opr)    ((dir << 24) | (pro << 22) | (biz << 16) | opr)

///////////////////////////////////////////////////////////////////////////////
// 动作代码相关常量
//
//   |<-- bit31                       bit0 -->|
//   0000 0000  00  000000  0000 0000 0000 0000
//   ---------  --  ------  -------------------
//      通      协    业            操
//      讯      议    务            作
//      方      分    分            分
//      向      类    类            类
//      码      码    码            码
//

// 掩码 -----------------------------------------------------------------------
const uint AC_DIR_MASK          = 0xFF000000;
const uint AC_PRO_MASK          = 0x00C00000;
const uint AC_BIZ_MASK          = 0x003F0000;
const uint AC_OPR_MASK          = 0x0000FFFF;

// 通讯方向码 -----------------------------------------------------------------
const byte AC_DIR_CS            = 1;        // 客户端 -> 服务端
const byte AC_DIR_SC            = 2;        // 服务端 -> 客户端
const byte AC_DIR_CC            = 3;        // 客户端 -> 客户端

// 协议分类码 -----------------------------------------------------------------
const byte AC_PRO_NONE          = 0;        // 未指定(用于忽略协议类型)
const byte AC_PRO_UDP           = 1;        // UDP 协议
const byte AC_PRO_DTP           = 2;        // DTP 协议

///////////////////////////////////////////////////////////////////////////////
// 动作代码 (UDP业务)

// UDP业务分类码
const byte AC_BIZ_UDP_NONE      = 0;        // 未指定(用于忽略业务类型)
const byte AC_BIZ_UDP_FINDER    = 1;        // 系统业务
const byte AC_BIZ_UDP_TRANSFER  = 2;        // 传输相关

// 客户端 -> 服务端
const uint CS_NOOP              = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 101);   // 无操作
const uint CS_DEBUG             = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 102);   // 调试
const uint CS_ACK               = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 103);   // 应答
const uint CS_FW_PACKET         = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 104);   // 客户端请求服务器转发数据包

// 服务端 -> 客户端
//const uint SC_REQUEST_FILE      = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 203);                             //向客户端请求资源
const uint SC_ACK               = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 201);   // 应答
const uint SC_FW_PACKET         = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 202);   // 客户端请求服务器转发数据包

// 客户端 -> 客户端
const uint CC_NOOP              = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 301);   // 无操作
const uint CC_ACK               = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 302);   // 应答
const uint CC_INV_CONN_REQ      = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 303);   // 反向连接请求

///////////////////////////////////////////////////////////////////////////////
// 动作代码 (DTP业务)

// DTP业务分类码
const byte AC_BIZ_DTP_NONE              = 0;        // 未指定(用于忽略业务类型)
const byte AC_BIZ_DTP_GREET             = 1;        // DTP握手
const byte AC_BIZ_DTP_NETMGR            = 2;        // 网络管理

// 客户端 -> 客户端
const uint CC_INV_CONNECT       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_GREET, 101);   // 反向连接
const uint CC_BIT_TORRENT       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_GREET, 102);   // BT下载
const uint CC_APP_SERVICE       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_NETMGR, 103);  // 节点的网络管理协议

// 客户端 -> 服务端
const uint CS_TRACKER_SERVICE   = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_DTP, AC_BIZ_DTP_NETMGR, 104);  // Tracker的管理协议

///////////////////////////////////////////////////////////////////////////////

#endif // _ACTCODE_BASE_H_
