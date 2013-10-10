//////////////////////////////////////////////////////////////////////
// User.h
/////////////////////////////////////////////////////////////////////

#ifndef _USER_H_
#define _USER_H_

#include <ext/hash_map>
#include "global.h"
#include "resource.h"
#include "ise_system.h"
//#include "systemstate.h"
#include "file.h"
using namespace std;
using namespace __gnu_cxx;

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CFile;
//class CIPRangeType;
class CUserManger;
/////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
extern CUserManger UserManger;

/////////////////////////////////////////////////////////////////////
//class Cuser - �û��б���Ӧһ���û�

typedef list <CFile*> FileContainer;
typedef FileContainer::iterator FileContainerIterator;

class CUser
{
private:
    uint m_nUserId;		//�û�ID
    uint m_nOuterIp;		//����IP
    uint m_nInnerIp;		//����IP
    uint m_nFirstTime;		//��һ�ε�½ʱ��
    uint m_nLastTime;		//���һ������ʱ��
    word m_wUdpPort;		//UDP�˿�
    word m_wTcpPort;  		//TCP�˿�
    char strUserName[10];          //�ͻ���;
    char strUserPassword[10];      //�ͻ�����;
    bool m_bState;		
    FileContainer m_FileList;	//������û�ӵ����Դ������
    CCriticalSection m_FileLock;
public:
  //  CUser();
    ~CUser();

//----------------------------------------------------------------------
//Set�������ڴ洢,Get�������ڻ��(��CFinder���д����)
    void SetUserId(const uint nUserId){ m_nUserId = nUserId; };
    uint GetUserId(){ return m_nUserId; };

    void SetOuterIp(const uint nOuterIp){ m_nOuterIp = nOuterIp; };
    uint GetOuterIp(){ return m_nOuterIp; };

    void SetInnerIp(const uint nInnerIp){ m_nInnerIp = nInnerIp; };
    uint GetInnerIp(){ return m_nInnerIp;};
    
    void SetFirstTime(const uint nFirstTime){ m_nFirstTime = nFirstTime; };
    uint GetFirstTime(){ return m_nFirstTime; };

    void SetLastTime(const uint nLastTime){ m_nLastTime = nLastTime; };
    uint GetLastTime(){ return m_nLastTime; };

    void SetUdpPort(const word wUdpPort){ m_wUdpPort = wUdpPort; };
    word GetUdpPort(){ return m_wUdpPort; };

    void SetTcpPort(const word wTcpPort){ m_wTcpPort = wTcpPort; };
    word GetTcpPort(){ return m_wTcpPort; };

    void SetstrUserName(const char strUserName[10]) {strUserName = strUserName;};
    char * GetstrUserName() {return  strUserName; };

    void SetstrUserPassword(const char strUserPassword[10]) {strUserPassword = strUserPassword;};
    char * GetstrUserPassword() {return strUserPassword; };

    void SetState(const bool bState){ m_bState = bState ;};
    bool GetState(){ return m_bState;};
    
    //����û�����Դ������
    void AddFile(CFile* File, bool bIsFile);
    //ɾ���û�����Դ������
    //bool RemoveFile(CFile* File, bool bIsFile);
    void ClearFile();
    //����ʱ��
    void UpdateFirstTime(){ m_nFirstTime = GetCurTicks(); };
    void UpdateLastTime(){ m_nLastTime = GetCurTicks(); };

};

/////////////////////////////////////////////////////////////////////
//class UserManger-�������е�½���û�

//ʹ��ָ�룬��ֹ����User����
typedef hash_map <uint, CUser*> UserMap;
typedef UserMap::iterator UserMapIterator;
//typedef vector<CUser> Userlist; 

class CUserManger
{
private:
    UserMap m_UserMap;
    CCriticalSection m_MapLock; 
   // CResource m_Resource;
public:
//    CUserManger();
//    ~CUserManger();

    CUser* AddUser(uint UserId, uint InnerIp, uint OuterIp, word UdpPort, word TcpPort, char strUserName[10], char strUserPassword[10]);
//    CUser* GetUser(CFinder& Finder, CUdpPacket& Packet, PeerAddress& Addr);
    CUser* FindUser(uint UserId);
    bool RemoveUser(uint UserId);
    void ViewUser();

};        

///////////////////////////////////////////////////////////////////////////////////////////
#endif































