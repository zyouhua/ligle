/////////////////////////////////////////////////////////////
// trackerpacket.h
////////////////////////////////////////////////////////////

#ifndef _CTRACKER_PACKET_H_
#define _CTRACKER_PACKET_H_

#include "config.h"
#include <vector>
#include "icl_classes.h"
#include "actcode_base.h"
#include "packet_udp.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// 宏定义

#define MAKE_FINDER_CS_ACTION_CODE(opr)  \
    MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_FINDER, opr);
#define MAKE_FINDER_SC_ACTION_CODE(opr)  \
    MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_FINDER, opr);

///////////////////////////////////////////////////////////////////////////////
// 动作代码定义

const uint SC_FINDER_REQUEST_FILE          = MAKE_FINDER_SC_ACTION_CODE(100);   //向客户端请求资源
const uint SC_FINDER_GET_IP		   = MAKE_FINDER_SC_ACTION_CODE(101);   //收到服务器发送来的用户IP
const uint SC_FINDER_GET_URL		   = MAKE_FINDER_SC_ACTION_CODE(102);   //收到服务器发送来的URL
const uint SC_FINDER_ACK		   = MAKE_FINDER_SC_ACTION_CODE(103);   //通用应答
const uint SC_FINDER_NONE_Resource	   = MAKE_FINDER_SC_ACTION_CODE(104);   //无资源包
const uint SC_FINDER_INTEGRAL_Resource	   = MAKE_FINDER_SC_ACTION_CODE(105);   //积分包

const uint CS_FINDER_LOGOUT                = MAKE_FINDER_CS_ACTION_CODE(101);   // 登出
const uint CS_FINDER_GET_USER_LIST         = MAKE_FINDER_CS_ACTION_CODE(102);   // 获取USER列表
const uint CS_FINDER_UPLOAD_FILE_LIST      = MAKE_FINDER_CS_ACTION_CODE(103);   // 上传共享文件列表
const uint CS_FINDER_KEEP_ALIVE            = MAKE_FINDER_CS_ACTION_CODE(104);   // 保活
const uint CS_FINDER_COMPLETE_FILE         = MAKE_FINDER_CS_ACTION_CODE(105);   // 文件下载完成
const uint CS_FINDER_REQUEST_INTEGRAL      = MAKE_FINDER_CS_ACTION_CODE(106);   // 请求积分包

///////////////////////////////////////////////////////////////////////////////
// 协议的结果值定义

const uint RET_FINDER_NOT_FILE = RET_BASE + 1;                // 不存在的File

/////////////////////////////////////////////////////////////
//CTrackerPacket - 业务包其类
class CTrackerPacket:public CUdpBizPacket
{
//nothing 
};

/////////////////////////////////////////////////////////////
//CCLogoutPacket - 下线包
class CLogoutPacket:public CTrackerPacket
{
public:
    //nothing,包头部分是每个包共有的属性，这个部分是每个包各有的属性
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_LOGOUT, 0,0); }
    void InitData();

};

//////////////////////////////////////////////////////////////
//CCKeepAlivePacket - 保活包
class CKeepAlivePacket:public CTrackerPacket
{
public:
    //nothing
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_KEEP_ALIVE, 0,0); }
    void InitData();

};

//////////////////////////////////////////////////////////////
//CIntegralPacket-(Integral)积分包
class CIntegralPacket:public CTrackerPacket
{
public:
    //nothing
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_REQUEST_INTEGRAL, 0,0); }
    void InitData();

};

/////////////////////////////////////////////////////////////
//CUploadFileListPacket - 上传资源列表包

struct CUploadData
{
    int64 nFileLength;
    string strHashValue;
};

typedef vector<CUploadData> CUploadList;

class CUploadFileListPacket:public CTrackerPacket
{
public:
    word wFileCount;
    CUploadList FileList;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_UPLOAD_FILE_LIST, 0,0); }
    void InitData(word wFileCount, CUploadList& FileList);

};

////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - 下载完成包

class CDownloadFileCompletePacket:public CTrackerPacket
{
public:
    string strUrl;
    int64 nFileLength;
    string strHashValue;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_COMPLETE_FILE, 0,0); }
    void InitData(string strUrl, int64 nFileLength, string strHashValue);
};

/////////////////////////////////////////////////////////////////
//CRequestResource - 请求下载资源包

class CRequestResourcePacket:public CTrackerPacket
{
public:
    string strRef;
    string strUrl;
    int64 nFileLength;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CTrackerPacket::InitHeader(CS_FINDER_GET_USER_LIST, 0,0); }
    void InitData(string strRef, string strUrl, int64 nFileLength, int nTaskId);
};


//----------------------------------------------------------------
//以上是客户端发来的包,下面是服务器回给客户端的包
//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
//CReTrackerPacket - 回包
class CReTrackerPacket:public CRePacket
{
//nothing
};
////////////////////////////////////////////////////////////////////
//CReOnPacket - 通用应答包
class CReOnPacket:public CReTrackerPacket
{
public:
    //nothing;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_ACK, 0,0); }
    void InitData(word nResultCode);
};
/////////////////////////////////////////////////////////////////
//CReGetResoursesPacket - 回给客户端URL资源

/*struct CUrlData
{
    string strReference;
    string strUrl;
};
typedef vector<CUrlData> UrlDataList;*/

class CReGetUrlPacket:public CReTrackerPacket
{
public:
    string strReference;
    string strUrl;
    string strHashValue;
    //uint nFileLength;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_GET_URL, 0,0); }
    void InitData(word nResultCode, string strReference, string strUrl,
                  string strHashValue,  int nTaskId);
};

//////////////////////////////////////////////////////////////////////
//CReGetUserIpPacket - 回给客户端一批用户IP
struct CUserData
{
    uint nUserIp;
    word wTcpPort;
};
typedef vector<CUserData> UserDataList;

class CReGetUserIpPacket:public CReTrackerPacket
{
public:
    UserDataList UserList;
    string strHashValue;
    //uint nFileLength;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_GET_IP, 0,0); }
    void InitData(word nResultCode, UserDataList& UserList, string strHashValue,
                  int nTaskId);
};

////////////////////////////////////////////////////////////////////
//CRequestFilePacket - 服务器要求客户端上传其曾经下载,现在拥有的资源
class CRequestFilePacket:public CReTrackerPacket
{
public:
    //nothing;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_REQUEST_FILE, 0,0); }
    void InitData(word nResultCode);
};

/////////////////////////////////////////////////////////////////////
//CReNoopPacket - 无资源包
class CReNoopPacket:public CReTrackerPacket
{
public:
    int nCount;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_NONE_Resource, 0,0); }
    void InitData(word nResultCode, int nCount);
};


/////////////////////////////////////////////////////////////////////
//CReIntegralPacket-(Integral)积分包

class CReIntegralPacket:public CReTrackerPacket
{
public:
    int nIntegral;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CReTrackerPacket::InitHeader(SC_FINDER_INTEGRAL_Resource, 0,0); }
    void InitData(word nResultCode, int nIntegral);
};
        

//////////////////////////////////////////////////////////////////
#endif













