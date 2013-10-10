/////////////////////////////////////////////////////////////////////////////////
//CFile.cpp

//#include "cfinderPacket"
#include "user.h"
#include "file.h"


////////////////////////////////////////////////////////////////////////////////
//全局变量
CFileManger FileManger;

////////////////////////////////////////////////////////////////////////////////
//CFile - 资源类

bool CFile::RemoveUser(CUser* User, bool bIsUser)
{
    CAutoSynchronizer Syncher(m_UserLock);
    for(UserContainerIterator iter = m_UserContainer.begin();
        iter!=m_UserContainer.end(); ++iter)
    {
        if(*iter == User)
        {
            //if (bIsUser) User->RemoveFile(this, false);
            m_UserContainer.erase(iter);
            return true;
        }
    }
    return false;
};

void CFile::AddUser(CUser* User, bool bIsUser)
{
    CAutoSynchronizer Syncher(m_UserLock);
    for(UserContainerIterator iter = m_UserContainer.begin();
        iter!=m_UserContainer.end(); ++iter)
    {
        if(*iter == User) return;
    }
    m_UserContainer.push_back(User);
    if(bIsUser) User->AddFile(this, false);
};

void  CFile::GetIp(uint nInIp, uint nOutIp, UserDataList& UserList)
{
    CUserData UserData;
    for(UserContainerIterator iter = m_UserContainer.begin();
        iter!=m_UserContainer.end(); ++iter)
    {
        uint nOuterIp = (*iter)->GetOuterIp();
        uint nInnerIp = (*iter)->GetInnerIp();
        word wTcpPort = (*iter)->GetTcpPort();
        bool bState = (*iter)->GetState();
        //用户和所取用户为同一用户
        if(nOuterIp == nOutIp && nInnerIp == nInIp) continue;

        //用户处在外网
        if(bState)
        {
            UserData.nUserIp = nOuterIp;
            UserData.wTcpPort = wTcpPort;
            UserList.push_back(UserData);
            continue;
        }
        //用户和所取用户处在同一局域网
        if(nOutIp == nOuterIp)
        {
            UserData.nUserIp = nInnerIp;
            UserData.wTcpPort = wTcpPort;
            UserList.push_back(UserData);
            continue;
        }
    }
     
};

/////////////////////////////////////////////////////////////////////////////////
//CFileManger - 资源管理类

CFile* CFileManger::FindFile(uint nFileId)
{
    CAutoSynchronizer Syncher(m_FileLock);
    FileMapIterator iter = m_FileContainer.find(nFileId);
    return ((iter == m_FileContainer.end()) ? NULL: iter->second);
};

CFile* CFileManger::AddFile(uint nFileId)
{
    CFile* File = new CFile();
    File->SetFileId(nFileId);
    m_FileContainer[nFileId] = File;
    return m_FileContainer[nFileId];
};


