////////////////////////////////////////////////////////////////////////////////////////////
//User.cpp

#include "user.h"
//#include "file.h"
//#include "iprangetype.h"
//#include "systemstate.h"

////////////////////////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���

CUserManger UserManger;
///////////////////////////////////////////////////////////////////////////////////////////
//CUser -�û���

CUser::~CUser()
{
    ClearFile();

};

//����û���Ӧ����Դ
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

/*//ɾ���û���Ӧ����Դ
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

//ɾ����Դ���û������ã��Լ�ɾ���û�����Դ������
void CUser::ClearFile()
{
    CAutoSynchronizer Syncher(m_FileLock);

    //ʹ�õ������û���Ӧ����Դһ��һ�������
    for(FileContainerIterator iter_File = m_FileList.begin();
        iter_File != m_FileList.end(); ++iter_File)
    {
        (*iter_File)->RemoveUser(this, false);
    }
    //�����ɾ��ֻ��iter�ͷ���
    m_FileList.clear();

}; 


///////////////////////////////////////////////////////////////////////////////////////
//CUserManger - �û�������

//����û�
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
    //�û�������������ʶλΪ��
    if(nInnerIp == nOuterIp) User->SetState(true);

    m_UserMap[nUserId] = User;

    return User;

};

//��ѯ�û�Map,����ָ����û����洢��Ϣ��ָ��
CUser* CUserManger::FindUser(uint nUserId)
{
    CAutoSynchronizer Syncher(m_MapLock);

     UserMapIterator iter = m_UserMap.find(nUserId);
     return ((iter == m_UserMap.end()) ? NULL : iter->second);
   //CUser* User = m_UserMap[nUserId];
   //return User;
};

//���û�Map��ɾ���û�,�ͷ��û�����
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
    const uint TIMEOUT_LIMIT = 70 * 1000;    // Peer����70��û�в�����remove��
    CUser* User = NULL;                      //��ɾ��ʱӦ��ȥ�������ݿ⡣
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




