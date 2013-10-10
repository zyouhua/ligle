/////////////////////////////////////////////////////////////////////////////////
//trackerPacket.cpp

#include "trackerpacket.h"

/////////////////////////////////////////////////////////////////////////////////
//CLogoutPacket - ���߰�

void CLogoutPacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CLogoutPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CLogoutPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CKeepAlivePacket - �����

void CKeepAlivePacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CKeepAlivePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CKeepAlivePacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CCIntegralPacket - ���ְ�

void CIntegralPacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CIntegralPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CIntegralPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CUpLoadFileListPacket - �ϴ���Դ�б��

void CUploadFileListPacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteBuffer(&wFileCount, sizeof(word));
    for(int i=0; i<wFileCount; i++)
    {
        WriteBuffer(&FileList.at(i).nFileLength, sizeof(int64));
        WriteString(FileList.at(i).strHashValue);
    }
};

void CUploadFileListPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadBuffer(&wFileCount, sizeof(word));
    ////������һ���ṹ�����,����ȡ��������ṹ���Ӧ,�ٷ���������
    CUploadData UploadFile;
    for(int i=0; i<wFileCount; i++)
    {
        ReadBuffer(&UploadFile.nFileLength, sizeof(int64));
        ReadString(UploadFile.strHashValue);
        FileList.push_back(UploadFile);
    }
};

void CUploadFileListPacket::InitData(word wFileCount, CUploadList& FileList)
{
    this->wFileCount = wFileCount;
    this->FileList.assign(FileList.begin(), FileList.end());
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - ������ɰ�

void CDownloadFileCompletePacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteString(strHashValue);
};

void CDownloadFileCompletePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadString(strHashValue);
};

void CDownloadFileCompletePacket::InitData(string strUrl, int64 nFileLength, string strHashValue)
{
    this->strUrl = strUrl;
    this->nFileLength = nFileLength;
    this->strHashValue = strHashValue;
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CRequestResource - ����������Դ��

void CRequestResourcePacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteString(strRef);
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadString(strRef);
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::InitData(string strUrl, string strRef, int64 nFileLength, int nTaskId)
{
    this->strRef = strRef;
    this->strUrl = strUrl;
    this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    Pack();
};


//------------------------------------------------------------------------------
// �����Ƿ�������Ҫ���͵İ�


////////////////////////////////////////////////////////////////////////////////
//CRequestFilePacket - ������Ҫ��ͻ����ϴ�����������,����ӵ�е���Դ

void CRequestFilePacket::DoPack()
{
    CReTrackerPacket::DoPack();
};

void CRequestFilePacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
};

void CRequestFilePacket::InitData(word nResultCode)
{
    CReTrackerPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
//CReOnPacket - ͨ��Ӧ���

void CReOnPacket::DoPack()
{
    CReTrackerPacket::DoPack();
};

void CReOnPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
};

void CReOnPacket::InitData(word nResultCode)
{
    CReTrackerPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
// CReGetResoursesPacket - �ظ��ͻ���һ��URL

void CReGetUrlPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    //��һ��URL
    WriteString(strReference);
    WriteString(strUrl);
    WriteString(strHashValue);
    //WriteBuffer(&nFileLength, sizeof(uint));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    //��ȡһ��URL
    ReadString(strReference);
    ReadString(strUrl);
    ReadString(strHashValue);
    //ReadBuffer(&nFileLength, sizeof(uint));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::InitData(word nResultCode, string strReference, string strUrl,
                                string strHashValue, int nTaskId)
{
    this->strReference = strReference;
    this->strUrl = strUrl;
    this->strHashValue = strHashValue;
    //this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    CReTrackerPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReGetUserIpPacket - �ظ��ͻ���һ��IP
void CReGetUserIpPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    //��һ���û�IP
    int Number = UserList.size();
    WriteBuffer(&Number, sizeof(int));
    for(int i=0; i<Number; i++)
    {
        WriteBuffer(&UserList.at(i).nUserIp, sizeof(uint));
        WriteBuffer(&UserList.at(i).wTcpPort, sizeof(word));

    }
    WriteString(strHashValue);
    //WriteBuffer(&nFileLength, sizeof(uint));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    //��ȡһ���û�IP��ַ
    int Number;
    ReadBuffer(&Number, sizeof(int));
    CUserData UserData;
    for(int i=0; i<Number; i++)
    {
        ReadBuffer(&UserData.nUserIp, sizeof(uint));
        ReadBuffer(&UserData.wTcpPort, sizeof(word));
        UserList.push_back(UserData);
    }
    ReadString(strHashValue);
    //ReadBuffer(&nFileLength, sizeof(uint));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::InitData(word nResultCode, UserDataList& UserList,
                        string strHashValue, int nTaskId)
{
    this->UserList.assign(UserList.begin(), UserList.end());
    this->strHashValue = strHashValue;
    //this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    CReTrackerPacket::InitData(nResultCode);
};


/////////////////////////////////////////////////////////////////////////////////
// CReNoopPacket - ����Դ��

void CReNoopPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    WriteBuffer(&nCount, sizeof(int));
    //WriteBuffer(&nTaskId, sizeof(int));
};

void CReNoopPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    ReadBuffer(&nCount, sizeof(int));
    //ReadBuffer(&nTaskId, sizeof(int));
};

void CReNoopPacket::InitData(word nResultCode, int nCount)
{
    this->nCount = nCount;
    CReTrackerPacket::InitData(nResultCode);
};



//////////////////////////////////////////////////////////////////////////////////
// CReIntegralPacket-(Integral)���ְ�

void CReIntegralPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    WriteBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    ReadBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::InitData(word nResultCode, int nIntegral)
{
    this->nIntegral = nIntegral;
    CReTrackerPacket::InitData(nResultCode);
};

