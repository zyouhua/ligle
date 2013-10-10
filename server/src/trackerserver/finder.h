////////////////////////////////////////////////////////////////
// finder.h
///////////////////////////////////////////////////////////////

#ifndef _SVRMOD_FINDER_H_
#define _SVRMOD_FINDER_H_

#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "dispatcher.h"
#include "cfinderpacket.h"
//#include "resource.h"


/////////////////////////////////////////////////////////////
//class CFinder

class CFinder:public CServerModule
{
private:
    CResource m_Resource;

private:
//-----------------------------------------------------------
//  用来处理用户下线
//-----------------------------------------------------------
    void DoLogout(CLogoutPacket& Packet,  PeerAddress& RemoteAddr);

//-----------------------------------------------------------
//  处理用户在线报告
//-----------------------------------------------------------
    void DoKeepAlive(CKeepAlivePacket& Packet, PeerAddress& RemoteAddr);

//-----------------------------------------------------------
//  处理用户请求积分包
//-----------------------------------------------------------
    void DoIntegral(CIntegralPacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  上传拥有的资源
//----------------------------------------------------------
    void DoUploadFileList(CUploadFileListPacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  下载完成的报告
//----------------------------------------------------------
    void DoDownloadFileComplete(CDownloadFileCompletePacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  请求下载资源
//----------------------------------------------------------
    void DoRequestResource(CRequestResourcePacket& Packet, PeerAddress& RemoteAddr);
//----------------------------------------------------------
//  用户登陆
//----------------------------------------------------------
    CUser* Login(CFinderPacket& Packet, PeerAddress& RemoteAddr);

public:
//    CFinder();
//    ~CFinder();
    // 取得该模块管辖的UDP数据包动作代码
    virtual void GetUdpActionCodes(ActionCodeArray& List);

    // UDP数据包分派 (数据包已解密)
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, 
            CUdpPacket& Packet);    


};
////////////////////////////////////////////////////////////////
#endif
