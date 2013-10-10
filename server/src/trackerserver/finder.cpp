////////////////////////////////////////////////////////////////////////////////
// CFinder.cpp
// 服务器模块
////////////////////////////////////////////////////////////////////////////////

#include "finder.h"
#include <time.h>
#include "file.h"
#include "ise_sysutils.h"
#include "utilities.h"

////////////////////////////////////////////////////////////////////////////////
//Class CFinder
CUser* CFinder::Login( CFinderPacket& Packet, PeerAddress& RemoteAddr)
 {  
    int64 nInIp = Packet.Header.nUserIp;
    int64 nOutIp = RemoteAddr.nIp;
    int64 nUserId = (nInIp<<32)^nOutIp;
    word wUdpPort = Packet.Header.wUdpPort;
    word wTcpPort = Packet.Header.wTcpPort;
    if(Packet.Header.strUserName != "" && Packet.Header.strUserPassword != "" )
       {   CUser * User = UserManger.FindUser(nUserId);
       	   char * UserName = User->GetstrUserName(); 
           //char tempUserName[10];
           //strcpy(tempUserName, UserName); 
           if((User != NULL) &&(UserName == Packet.Header.strUserName))   return User;
       	   char strUserName[10];
           char strUserPassword[10]; 
           strcpy(strUserName,Packet.Header.strUserName);
           strcpy(strUserPassword,Packet.Header.strUserPassword);
            //此时应该去数据查询用户名和密码是否正确。1、正确返回1，发一个带有积分的包  2、错误返回0， 发一个说明错误的包。
            int nIntegral = m_Resource.Find(strUserName, strUserPassword) ;
      
            if(nIntegral < 0)
                {  
                   CReIntegralPacket ReIntegralPacket;
                   ReIntegralPacket.InitHeader(SC_FINDER_INTEGRAL_Resource, Packet.Header.nSeqNumber, 0);
                   ReIntegralPacket.InitData(RET_FAILURE, nIntegral);
                   SendUdpPacket(ReIntegralPacket, CInetAddress(RemoteAddr.nIp),
                                 RemoteAddr.nPort, 3);
                   return 0;
                 }
            if(nIntegral >= 0)
               { 
                  CReIntegralPacket ReIntegralPacket;
                  ReIntegralPacket.InitHeader(SC_FINDER_INTEGRAL_Resource, Packet.Header.nSeqNumber, 0);
                  ReIntegralPacket.InitData(RET_SUCCESS, nIntegral);
                  SendUdpPacket(ReIntegralPacket, CInetAddress(RemoteAddr.nIp),
                                RemoteAddr.nPort, 3);
                  CUser* User = UserManger.AddUser(nUserId, nInIp, nOutIp, wUdpPort, wTcpPort, strUserName, strUserPassword);
                  User->UpdateFirstTime(); 
                  CRequestFilePacket RePacket;
                  RePacket.InitHeader(SC_FINDER_REQUEST_FILE, Packet.Header.nSeqNumber, 0);
                  RePacket.InitData(RET_SUCCESS);
                  SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                                 RemoteAddr.nPort, 3); 

              return User;
                 }
        }

    CUser* User = UserManger.FindUser(nUserId);
    if(User == NULL)
    { 
        char strUserName[10] = "q";
        char strUserPassword[10] = "q";
        User = UserManger.AddUser(nUserId, nInIp, nOutIp, wUdpPort, wTcpPort, strUserName, strUserPassword);
        User->UpdateFirstTime();
        //发送向客户端请求资源的包
        CRequestFilePacket RePacket;
        RePacket.InitHeader(SC_FINDER_REQUEST_FILE, Packet.Header.nSeqNumber, 0);
        RePacket.InitData(RET_SUCCESS);
        SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                        RemoteAddr.nPort, 3); 

        return User;
    }

    return User;


};

void CFinder::DoLogout( CLogoutPacket& Packet, PeerAddress& RemoteAddr)
{
    //算出用户的ID
    int64 nInIp = Packet.Header.nUserIp;
    int64 nOutIp = RemoteAddr.nIp;
    int64 nId = (nInIp<<32)^nOutIp;
    //查找该用户
    CUser* User = UserManger.FindUser(nId);
    //该用户存在，则删除它
    if(User != NULL)
    {
        UserManger.RemoveUser(nId);
    }

};

void CFinder::DoKeepAlive(CKeepAlivePacket& Packet,
                            PeerAddress& RemoteAddr)
{
    //GetUser()可能要异常抛出，查找用户，并更新在线时间
    CUser* User = Login(Packet, RemoteAddr);
    if(User == NULL) return;
    User->UpdateLastTime();

    //发送通用应答包
    CReOnPacket RePacket;
    RePacket.InitHeader(SC_FINDER_ACK, Packet.Header.nSeqNumber,0);
    RePacket.InitData(RET_SUCCESS);
    SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                        RemoteAddr.nPort, 3);

};

void CFinder::DoIntegral(CIntegralPacket& Packet,
                            PeerAddress& RemoteAddr)
{
    CUser* User = Login(Packet, RemoteAddr);
    if(User == NULL) return;
    //User->UpdateLastTime();

    //发送通用应答包
    CReOnPacket RePacket;
    RePacket.InitHeader(SC_FINDER_ACK, Packet.Header.nSeqNumber,0);
    RePacket.InitData(RET_SUCCESS);
    SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                        RemoteAddr.nPort, 3);

};

void CFinder::DoUploadFileList(CUploadFileListPacket& Packet,
                                PeerAddress& RemoteAddr)
{
    CUser* User = Login(Packet, RemoteAddr);
    if(User == NULL) return;

    CUploadFile Load;      //定义了一个上传文件的结构体变量

    for(int i=0; i<Packet.wFileCount; i++)
    {
        uint FileId = 0;
        //从结构体列表中取出上传过来的哈希值和文件长度
        Load.nFileLength = Packet.FileList.at(i).nFileLength;
        Load.strHashValue = Packet.FileList.at(i).strHashValue;

        FileId = m_Resource.GetFileId(Load.nFileLength, Load.strHashValue);
        if(FileId>0)
        {
            CFile* File = FileManger.FindFile(FileId);
            if(File == NULL)
            {
                File = FileManger.AddFile(FileId);
                File->AddUser(User, true);
            }else
            {
                File->AddUser(User, true);
            }
        }
        //发送通用应答包
        CReOnPacket RePacket;
        RePacket.InitHeader(SC_FINDER_ACK, Packet.Header.nSeqNumber,0);
        RePacket.InitData(RET_SUCCESS);
        SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                        RemoteAddr.nPort, 3);

    }

};

void CFinder::DoDownloadFileComplete(CDownloadFileCompletePacket& Packet,
                             PeerAddress& RemoteAddr)
{
    CUser* User = Login(Packet,RemoteAddr);
    if(User == NULL) return;

    //从解开的包中取出上传过来的值
    string strUrl= Packet.strUrl;
    int64 nFileLength = Packet.nFileLength;
    string strHashValue = Packet.strHashValue;

    uint FileId = 0;
    FileId = m_Resource.Downloaded(strUrl, nFileLength, strHashValue);
    if(FileId>0)
    {
        CFile* File = FileManger.FindFile(FileId);
        if(File == NULL)
        {
            File = FileManger.AddFile(FileId);
            File->AddUser(User, true);
        }else
        {
            File->AddUser(User, true);
        }

    }

    //发送通用应答包
    CReOnPacket RePacket;
    RePacket.InitHeader(SC_FINDER_ACK, Packet.Header.nSeqNumber,0);
    RePacket.InitData(RET_SUCCESS);
    SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                RemoteAddr.nPort, 3);

};

void CFinder::DoRequestResource(CRequestResourcePacket& Packet,
                              PeerAddress& RemoteAddr)
 {
    CUser* User = Login(Packet, RemoteAddr);
    if(User == NULL) return;
    UrlDataList UrlList;
    CUrlData UrlData;
    string strRef = Packet.strRef;
    string strUrl = Packet.strUrl;
    int64 nFileLength = Packet.nFileLength;
    int nTaskId = Packet.nTaskId;

    string strHashValue;
    uint nFileId = 0;
    nFileId = m_Resource.GetResource(strUrl, strRef, nFileLength, UrlList, strHashValue);
    int count = 0;
    count = UrlList.size();
    int nCount = count + 1;
    if(count>0)
    {
        for(int i=0; i<count; i++)
        {
            UrlData.strRef = UrlList.at(i).strRef;
            UrlData.strUrl = UrlList.at(i).strUrl;
            CReGetUrlPacket RePacket;
            RePacket.InitHeader(SC_FINDER_GET_URL, Packet.Header.nSeqNumber,i);
            RePacket.InitData(RET_SUCCESS, UrlData.strRef, UrlData.strUrl, strHashValue, nTaskId);
            SendUdpPacket(RePacket, CInetAddress(RemoteAddr.nIp),
                        RemoteAddr.nPort, 3);
        }
    }

    UserDataList UserList;
    CFile* File = FileManger.FindFile(nFileId);
    int Number = 0;
    if(File != NULL)
    {
        uint nInIp = User->GetInnerIp();
        uint nOutIp = User->GetOuterIp();
        File->GetIp(nInIp, nOutIp, UserList);

        Number = UserList.size();
      if( Number>0)
      {  
        CReGetUserIpPacket RePack;
        RePack.InitHeader(SC_FINDER_GET_IP, Packet.Header.nSeqNumber,nCount);
        RePack.InitData(RET_SUCCESS, UserList, strHashValue, nTaskId);
        SendUdpPacket(RePack, CInetAddress(RemoteAddr.nIp),
                            RemoteAddr.nPort, 3);
     }
     if(((File == NULL) || (Number == 0)) && (count == 0))
     {
        CReNoopPacket RePack;
        RePack.InitHeader(SC_FINDER_NONE_Resource, Packet.Header.nSeqNumber,0);
        RePack.InitData(RET_SUCCESS, 0);
        SendUdpPacket(RePack, CInetAddress(RemoteAddr.nIp),
                            RemoteAddr.nPort, 3);
     }
   }
};

//-----------------------------------------------------------------------------
// 描述: 取得该模块管辖的UDP数据包动作代码
//-----------------------------------------------------------------------------
void CFinder::GetUdpActionCodes(ActionCodeArray& List)
{
    List.push_back(CS_FINDER_LOGOUT);
    List.push_back(CS_FINDER_GET_USER_LIST);
    List.push_back(CS_FINDER_UPLOAD_FILE_LIST); 
    List.push_back(CS_FINDER_KEEP_ALIVE);
    List.push_back(CS_FINDER_COMPLETE_FILE);
};

//-----------------------------------------------------------------------------
// 描述: UDP数据包分派 (数据包已解密)
//-----------------------------------------------------------------------------
void CFinder::DispatchUdpPacket(CUdpWorkerThread& WorkerThread,
                                             CUdpPacket& Packet)
{
    try
    {
        switch (((CUdpPacketHeader*)Packet.GetPacketBuffer())->nActionCode)
        {
        case CS_FINDER_LOGOUT:
            {
                CLogoutPacket OpPacket;
                OpPacket.Unpack(Packet.GetPacketBuffer(), Packet.m_nPacketSize);
                DoLogout(OpPacket, Packet.m_RemoteAddr);
                break;
            }
        case CS_FINDER_UPLOAD_FILE_LIST:            // 客户端提交资源列表
            {
                CUploadFileListPacket OpPacket;
                OpPacket.Unpack(Packet.GetPacketBuffer(), Packet.m_nPacketSize);
                DoUploadFileList(OpPacket, Packet.m_RemoteAddr);
                break;
            }
        case CS_FINDER_GET_USER_LIST:
            {
                CRequestResourcePacket OpPacket;
                OpPacket.Unpack(Packet.GetPacketBuffer(), Packet.m_nPacketSize);
                DoRequestResource(OpPacket, Packet.m_RemoteAddr);
                break;
            }
        case CS_FINDER_COMPLETE_FILE:
           {
                CDownloadFileCompletePacket OpPacket;
                OpPacket.Unpack(Packet.GetPacketBuffer(), Packet.m_nPacketSize);
                DoDownloadFileComplete(OpPacket, Packet.m_RemoteAddr);
                break;
            }
        case CS_FINDER_KEEP_ALIVE:
            {
                CKeepAlivePacket OpPacket;
                OpPacket.Unpack(Packet.GetPacketBuffer(), Packet.m_nPacketSize);
                DoKeepAlive(OpPacket, Packet.m_RemoteAddr);
                break;
            }
        }
    }
    catch (CException& e)
    {
        Logger.Write(e);
    }
};

