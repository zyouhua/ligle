////////////////////////////////////////////////////////////////////////////////////////////
//User.cpp

#include "user.h"
//#include "file.h"
//#include "iprangetype.h"
//#include "systemstate.h"

////////////////////////////////////////////////////////////////////////////////////////////
//全局变量

CUserManger UserManger;
///////////////////////////////////////////////////////////////////////////////////////////
//CUser -用户类

CUser::~CUser()
{
    ClearFile();

};

//添加用户对应的资源
void CUser::AddFile(CFile* File, bool bIsFile)
{
    CAutoSynchronizer Syncher(m_FileLock);
    for(FileContainerIterator iter = m_FileList.begin();
        iter != m_FileList.end(); ++iter)
    {
        if(*iter = File) return;
    }
    m_FileList.push_back(File);

    if(bIsFile) File->AddUser(this, false);

};

/*//删除用户对应的资源
bool CUser::RemoveFile(CFile* File, bool bIsFile)
{
    CAutoSynchronizer Syncher(m_FileLock);
    for(FileContainerIterator iter = m_FileList.begin();
        iter != m_FileList.end(); ++iter)
    {
        if(*iter == File)
        {
            m_FileList.erase(iter);
            if(bIsFile) File->RemoveUser(this, false);
        }
        return true;
    }
    return false;

}; */

//删除资源对用户的引用，以及删除用户对资源的引用
void CUser::ClearFile()
{
    CAutoSynchronizer Syncher(m_FileLock);

    //使用迭带把用户对应的资源一个一个处理掉
    for(FileContainerIterator iter_File = m_FileList.begin();
        iter_File != m_FileList.end(); ++iter_File)
    {
        (*iter_File)->RemoveUser(this, false);
    }
    //上面的删除只把iter释放了
    m_FileList.clear();

}; 


///////////////////////////////////////////////////////////////////////////////////////
//CUserManger - 用户管理类

//添加用户
CUser* CUserManger::AddUser(uint nUserId, uint nInnerIp, uint nOuterIp,
                                word wUdpPort, word wTcpPort, char strUserName[10], char strUserPassword[10])
{
    CAutoSynchronizer Syncher(m_MapLock);
    CUser* User = new CUser();
    User->SetUserId(nUserId);
    User->SetInnerIp(nInnerIp);
    User->SetOuterIp(nOuterIp);
    User->SetUdpPort(wUdpPort);
    User->SetTcpPort(wTcpPort);
    User->SetstrUserName(strUserName);
    User->SetstrUserPassword(strUserPassword);
    //用户处在外网，标识位为真
    if(nInnerIp == nOuterIp) User->SetState(true);

    m_UserMap[nUserId] = User;

    return User;

};

//查询用户Map,返回指向该用户所存储信息的指针
CUser* CUserManger::FindUser(uint nUserId)
{
    CAutoSynchronizer Syncher(m_MapLock);

     UserMapIterator iter = m_UserMap.find(nUserId);
     return ((iter == m_UserMap.end()) ? NULL : iter->second);
   //CUser* User = m_UserMap[nUserId];
   //return User;
};

//从用户Map中删除用户,释放用户对象
bool CUserManger::RemoveUser(uint nUserId)
{
    CAutoSynchronizer Syncher(m_MapLock);

    CUser* User = m_UserMap[nUserId];
    if(User != NULL)
    {  
        uint Time = GetCurTicks();
        uint OnlineTime =  Time - User->GetFirstTime();
        int nIntegral = 6*(OnlineTime/1000/3600);
        CResource m_Resource;
        char * strUserName = User->GetstrUserName();
        char * strUserPassword = User->GetstrUserPassword();
        bool bIntegral = m_Resource.UpdateIntegral(strUserName, strUserPassword, nIntegral);      
        m_UserMap[nUserId] = NULL;
        delete User;
        return true;
    }else
    {
        return false;
    }


};

void CUserManger::ViewUser()
{
    CAutoSynchronizer Syncher(m_MapLock);
    const uint TIMEOUT_LIMIT = 70 * 1000;    // Peer超过70秒没有操作就remove掉
    CUser* User = NULL;                      //在删除时应该去更新数据库。
    uint nDiffTime = 0;

    for(UserMapIterator iter = m_UserMap.begin(); iter != m_UserMap.end(); )
    {

        User = iter->second;
        uint Time = GetCurTicks();
        nDiffTime = Time - User->GetLastTime();
        //uint OnlineTime =  Time - User->GetFirstTime();
        //int nIntegral = 6*(OnlineTime/1000/3600);
        if(nDiffTime >= TIMEOUT_LIMIT)
        {   //CResource m_Resource;
            //char * strUserName = User->GetstrUserName();
            //char * strUserPassword = User->GetstrUserPassword();
            //bool bIntegral = m_Resource.UpdateIntegral(strUserName, strUserPassword, nIntegral);
            m_UserMap.erase(iter++);

            delete User;

        }else
	{
	    ++iter;
	}
    }

};




