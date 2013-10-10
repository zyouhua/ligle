//////////////////////////////////////////////////////////////////////////////
//CFile.h - 资源类

#ifndef _CFILE_H_
#define _CFILE_H_

//#include <vector>
#include <ext/hash_map>
#include <global.h>
#include "user.h"
#include "ise_system.h"
#include  "cfinderpacket.h"
////////////////////////////////////////////////////////////////////////////////
//提前声明
class CUser;
class CFileManger;

//////////////////////////////////////////////////////////////////////////////
//全局变量
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
    //用户下线时要删除资源对应的用户
    bool RemoveUser(CUser* User, bool bIsUser);

    //用户上传资源时，同时添加资源对应该的该用户
    void AddUser(CUser* User, bool bIsUser);

    //通过使用迭带器，返回一批用户IP
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

    //上传资源，数据库返回资源ID，查询资源
    CFile* FindFile(uint nFileId);

    //上传资源，没有资源，添加
    CFile* AddFile(uint nFileId);



};

/////////////////////////////////////////////////////////////////////////////

#endif














