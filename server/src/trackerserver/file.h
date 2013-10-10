//////////////////////////////////////////////////////////////////////////////
//CFile.h - ��Դ��

#ifndef _CFILE_H_
#define _CFILE_H_

//#include <vector>
#include <ext/hash_map>
#include <global.h>
#include "user.h"
#include "ise_system.h"
#include  "cfinderpacket.h"
////////////////////////////////////////////////////////////////////////////////
//��ǰ����
class CUser;
class CFileManger;

//////////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
extern CFileManger FileManger;

typedef list <CUser*> UserContainer;
typedef UserContainer::iterator UserContainerIterator;

class CFile
{
private:
    CCriticalSection m_UserLock;
    uint m_nFileId;
    UserContainer m_UserContainer;
public:

    void SetFileId(const uint nFileId){ m_nFileId = nFileId; };
    uint GetFileId(){ return m_nFileId; };
    //�û�����ʱҪɾ����Դ��Ӧ���û�
    bool RemoveUser(CUser* User, bool bIsUser);

    //�û��ϴ���Դʱ��ͬʱ�����Դ��Ӧ�õĸ��û�
    void AddUser(CUser* User, bool bIsUser);

    //ͨ��ʹ�õ�����������һ���û�IP
    void GetIp(uint nInIp, uint nOutIp, UserDataList& UserList);

};

typedef hash_map <uint, CFile*> FileMap;
typedef FileMap::iterator FileMapIterator;
//typedef vector<CFile> FileContainer;
//typedef FileContainer::iterator FileContainerIterator;

class CFileManger
{

private:
    FileMap m_FileContainer;
    CCriticalSection m_FileLock;
public:

    //�ϴ���Դ�����ݿⷵ����ԴID����ѯ��Դ
    CFile* FindFile(uint nFileId);

    //�ϴ���Դ��û����Դ�����
    CFile* AddFile(uint nFileId);



};

/////////////////////////////////////////////////////////////////////////////

#endif














