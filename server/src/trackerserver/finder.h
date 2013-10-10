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
//  ���������û�����
//-----------------------------------------------------------
    void DoLogout(CLogoutPacket& Packet,  PeerAddress& RemoteAddr);

//-----------------------------------------------------------
//  �����û����߱���
//-----------------------------------------------------------
    void DoKeepAlive(CKeepAlivePacket& Packet, PeerAddress& RemoteAddr);

//-----------------------------------------------------------
//  �����û�������ְ�
//-----------------------------------------------------------
    void DoIntegral(CIntegralPacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  �ϴ�ӵ�е���Դ
//----------------------------------------------------------
    void DoUploadFileList(CUploadFileListPacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  ������ɵı���
//----------------------------------------------------------
    void DoDownloadFileComplete(CDownloadFileCompletePacket& Packet, PeerAddress& RemoteAddr);

//----------------------------------------------------------
//  ����������Դ
//----------------------------------------------------------
    void DoRequestResource(CRequestResourcePacket& Packet, PeerAddress& RemoteAddr);
//----------------------------------------------------------
//  �û���½
//----------------------------------------------------------
    CUser* Login(CFinderPacket& Packet, PeerAddress& RemoteAddr);

public:
//    CFinder();
//    ~CFinder();
    // ȡ�ø�ģ���Ͻ��UDP���ݰ���������
    virtual void GetUdpActionCodes(ActionCodeArray& List);

    // UDP���ݰ����� (���ݰ��ѽ���)
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, 
            CUdpPacket& Packet);    


};
////////////////////////////////////////////////////////////////
#endif
