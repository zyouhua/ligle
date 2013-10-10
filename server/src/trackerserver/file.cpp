/////////////////////////////////////////////////////////////////////////////////
//CFile.cpp

//#include "cfinderPacket"
#include "user.h"
#include "file.h"


////////////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
CFileManger FileManger;

////////////////////////////////////////////////////////////////////////////////
//CFile - ��Դ��

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
        //�û�����ȡ�û�Ϊͬһ�û�
        if(nOuterIp == nOutIp && nInnerIp == nInIp) continue;

        //�û���������
        if(bState)
        {
            UserData.nUserIp = nOuterIp;
            UserData.wTcpPort = wTcpPort;
            UserList.push_back(UserData);
            continue;
        }
        //�û�����ȡ�û�����ͬһ������
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
//CFileManger - ��Դ������

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


