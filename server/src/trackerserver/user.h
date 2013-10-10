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
// 提前声明

class CFile;
//class CIPRangeType;
class CUserManger;
/////////////////////////////////////////////////////////////////////
//全局变量
extern CUserManger UserManger;

/////////////////////////////////////////////////////////////////////
//class Cuser - 用户列表，对应一个用户

typedef list <CFile*> FileContainer;
typedef FileContainer::iterator FileContainerIterator;

class CUser
{
private:
    uint m_nUserId;		//用户ID
    uint m_nOuterIp;		//外网IP
    uint m_nInnerIp;		//内网IP
    uint m_nFirstTime;		//第一次登陆时间
    uint m_nLastTime;		//最后一次在线时间
    word m_wUdpPort;		//UDP端口
    word m_wTcpPort;  		//TCP端口
    char strUserName[10];          //客户名;
    char strUserPassword[10];      //客户密码;
    bool m_bState;		
    FileContainer m_FileList;	//保存该用户拥有资源的引用
    CCriticalSection m_FileLock;
public:
  //  CUser();
    ~CUser();

//----------------------------------------------------------------------
//Set函数用于存储,Get函数用于获得(在CFinder类中打包用)
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
    
    //添加用户对资源的引用
    void AddFile(CFile* File, bool bIsFile);
    //删除用户对资源的引用
    //bool RemoveFile(CFile* File, bool bIsFile);
    void ClearFile();
    //更新时间
    void UpdateFirstTime(){ m_nFirstTime = GetCurTicks(); };
    void UpdateLastTime(){ m_nLastTime = GetCurTicks(); };

};

/////////////////////////////////////////////////////////////////////
//class UserManger-管理所有登陆的用户

//使用指针，防止复制User对象
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































