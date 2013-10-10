/////////////////////////////////////////////////////////////
// cfinderpacket.h
////////////////////////////////////////////////////////////

#ifndef _CFINDER_PACKET_H_
#define _CFINDER_PACKET_H_

#include <vector>

#include "packet_udp.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// �궨��

#define MAKE_FINDER_CS_ACTION_CODE(opr)  \
    MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_FINDER, opr);
#define MAKE_FINDER_SC_ACTION_CODE(opr)  \
    MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_FINDER, opr);

///////////////////////////////////////////////////////////////////////////////
// �������붨��

const uint SC_FINDER_REQUEST_FILE          = MAKE_FINDER_SC_ACTION_CODE(100);   //��ͻ���������Դ
const uint SC_FINDER_GET_IP		   = MAKE_FINDER_SC_ACTION_CODE(101);   //�յ����������������û�IP
const uint SC_FINDER_GET_URL		   = MAKE_FINDER_SC_ACTION_CODE(102);   //�յ���������������URL
const uint SC_FINDER_ACK		   = MAKE_FINDER_SC_ACTION_CODE(103);   //ͨ��Ӧ��
const uint SC_FINDER_NONE_Resource	   = MAKE_FINDER_SC_ACTION_CODE(104);   //����Դ��
const uint SC_FINDER_INTEGRAL_Resource	   = MAKE_FINDER_SC_ACTION_CODE(105);   //���ְ�

const uint CS_FINDER_LOGOUT                = MAKE_FINDER_CS_ACTION_CODE(101);   // �ǳ�
const uint CS_FINDER_GET_USER_LIST         = MAKE_FINDER_CS_ACTION_CODE(102);   // ��ȡUSER�б�
const uint CS_FINDER_UPLOAD_FILE_LIST      = MAKE_FINDER_CS_ACTION_CODE(103);   // �ϴ������ļ��б�
const uint CS_FINDER_KEEP_ALIVE            = MAKE_FINDER_CS_ACTION_CODE(104);   // ����
const uint CS_FINDER_COMPLETE_FILE         = MAKE_FINDER_CS_ACTION_CODE(105);   // �ļ��������
const uint CS_FINDER_REQUEST_INTEGRAL      = MAKE_FINDER_CS_ACTION_CODE(106);   // ������ְ�

///////////////////////////////////////////////////////////////////////////////
// Э��Ľ��ֵ����

const uint RET_FINDER_NOT_FILE = RET_BASE + 1;                // �����ڵ�File

/////////////////////////////////////////////////////////////
//CTrackerPacket - ҵ�������
class CFinderPacket:public CUdpBizPacket
{
//nothing 
};

/////////////////////////////////////////////////////////////
//CLogoutPacket - ���߰�
class CLogoutPacket:public CFinderPacket
{
public:
    //nothing,��ͷ������ÿ�������е����ԣ����������ÿ�������е�����
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_LOGOUT, 0, 0); }
    void InitData();
    
};

//////////////////////////////////////////////////////////////
//CKeepAlivePacket - �����
class CKeepAlivePacket:public CFinderPacket
{
public:
    //nothing
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_KEEP_ALIVE, 0, 0); }
    void InitData();

};

//////////////////////////////////////////////////////////////
//CIntegralPacket-(Integral)���ְ�
class CIntegralPacket:public CFinderPacket
{
public:
    //nothing
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_REQUEST_INTEGRAL, 0,0); }
    void InitData();

};

/////////////////////////////////////////////////////////////
//CUploadFileListPacket - �ϴ���Դ�б��

struct CUploadFile
{
    int64 nFileLength;
    string strHashValue;
};

typedef vector<CUploadFile> UploadFileList;

class CUploadFileListPacket:public CFinderPacket
{
public:
    word wFileCount;
    UploadFileList FileList;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_UPLOAD_FILE_LIST, 0, 0); }
    void InitData(word wFileCount, UploadFileList& FileList);
    
};

////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - ������ɰ�

class CDownloadFileCompletePacket:public CFinderPacket
{
public:
    string strUrl;
    int64 nFileLength;
    string strHashValue;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_COMPLETE_FILE, 0, 0); }
    void InitData(string strUrl, int64 nFileLength, string strHashValue);
};

/////////////////////////////////////////////////////////////////
//CRequestResource - ����������Դ��

class CRequestResourcePacket:public CFinderPacket
{
public:
    string strRef;
    string strUrl;
    int64 nFileLength;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    inline void InitHeader()
        { CFinderPacket::InitHeader(CS_FINDER_GET_USER_LIST, 0, 0); }
    void InitData(string strRef, string strUrl, int64 nFileLength, int TaskId);
};


//----------------------------------------------------------------
//�����ǿͻ��˷����İ�,�����Ƿ������ظ��ͻ��˵İ�
//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
//CReTrackerPacket - �ذ�
class CReFinderPacket:public CRePacket
{
//nothing
};
////////////////////////////////////////////////////////////////////
//CReOnPacket - ͨ��Ӧ���
class CReOnPacket:public CReFinderPacket
{
public:
    //nothing;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode);
};
/////////////////////////////////////////////////////////////////
//CReGetResoursesPacket - �ظ��ͻ���URL��Դ

/*struct CUrlData
{
    string strReference;
    string strUrl;
};
typedef vector<CUrlData> UrlDataList;*/

class CReGetUrlPacket:public CReFinderPacket
{
public:
    string strReference;
    string strUrl;
    string strHashValue;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode, string strReference, string strUrl,
                  string strHashValue, int nTaskId);
};

//////////////////////////////////////////////////////////////////////
//CReGetUserIpPacket - �ظ��ͻ���һ���û�IP
struct CUserData
{
    uint nUserIp;
    word wTcpPort;
};
typedef vector<CUserData> UserDataList;

class CReGetUserIpPacket:public CReFinderPacket
{
public:
    UserDataList UserList;
    string strHashValue;
    int nTaskId;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode, UserDataList& UserList, string strHashValue, int nTaskId);
};

////////////////////////////////////////////////////////////////////
//CRequestFilePacket - ������Ҫ��ͻ����ϴ�����������,����ӵ�е���Դ
class CRequestFilePacket:public CReFinderPacket
{
public:
    //nothing;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode);
};

/////////////////////////////////////////////////////////////////////
//CReNoopPacket - ����Դ��
class CReNoopPacket:public CReFinderPacket
{
public:
    int nCount;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode, int nCount);
};


/////////////////////////////////////////////////////////////////////
//CReIntegralPacket - ���ְ�
class CReIntegralPacket:public CReFinderPacket
{
public:
    int nIntegral;
protected:
    virtual void DoPack();
    virtual void DoUnpack();
public:
    void InitData(word nResultCode, int nIntegral);
};


//////////////////////////////////////////////////////////////////
#endif













