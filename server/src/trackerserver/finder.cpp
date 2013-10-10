////////////////////////////////////////////////////////////////////////////////
// CFinder.cpp
// ������ģ��
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
            //��ʱӦ��ȥ���ݲ�ѯ�û����������Ƿ���ȷ��1����ȷ����1����һ�����л��ֵİ�  2�����󷵻�0�� ��һ��˵������İ���
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
        //������ͻ���������Դ�İ�
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
    //����û���ID
    int64 nInIp = Packet.Header.nUserIp;
    int64 nOutIp = RemoteAddr.nIp;
    int64 nId = (nInIp<<32)^nOutIp;
    //���Ҹ��û�
    CUser* User = UserManger.FindUser(nId);
    //���û����ڣ���ɾ����
    if(User != NULL)
    {
        UserManger.RemoveUser(nId);
    }

};

void CFinder::DoKeepAlive(CKeepAlivePacket& Packet,
                            PeerAddress& RemoteAddr)
{
    //GetUser()����Ҫ�쳣�׳��������û�������������ʱ��
    CUser* User = Login(Packet, RemoteAddr);
    if(User == NULL) return;
    User->UpdateLastTime();

    //����ͨ��Ӧ���
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

    //����ͨ��Ӧ���
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

    CUploadFile Load;      //������һ���ϴ��ļ��Ľṹ�����

    for(int i=0; i<Packet.wFileCount; i++)
    {
        uint FileId = 0;
        //�ӽṹ���б���ȡ���ϴ������Ĺ�ϣֵ���ļ�����
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
        //����ͨ��Ӧ���
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

    //�ӽ⿪�İ���ȡ���ϴ�������ֵ
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

    //����ͨ��Ӧ���
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
// ����: ȡ�ø�ģ���Ͻ��UDP���ݰ���������
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
// ����: UDP���ݰ����� (���ݰ��ѽ���)
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
        case CS_FINDER_UPLOAD_FILE_LIST:            // �ͻ����ύ��Դ�б�
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

