/////////////////////////////////////////////////////////////////////////////////
//trackerPacket.cpp

#include "cfinderpacket.h"

/////////////////////////////////////////////////////////////////////////////////
//CLogoutPacket - ���߰�

void CLogoutPacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CLogoutPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CLogoutPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CKeepAlivePacket - �����

void CKeepAlivePacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CKeepAlivePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CKeepAlivePacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CCKeepAlivePacket - ���ְ�

void CIntegralPacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CIntegralPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CIntegralPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CUpLoadFileListPacket - �ϴ���Դ�б��

void CUploadFileListPacket::DoPack()
{
    CFinderPacket::DoPack();
    WriteBuffer(&wFileCount, sizeof(word));
    for(int i=0; i<wFileCount; i++)
    {
        WriteBuffer(&FileList.at(i).nFileLength, sizeof(int64));
        WriteString(FileList.at(i).strHashValue);
    }
};

void CUploadFileListPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
    ReadBuffer(&wFileCount, sizeof(word));
    ////������һ���ṹ�����,����ȡ��������ṹ���Ӧ,�ٷ���������
    CUploadFile UploadFile;
    for(int i=0; i<wFileCount; i++)
    {
        ReadBuffer(&UploadFile.nFileLength, sizeof(int64));
        ReadString(UploadFile.strHashValue);
        FileList.push_back(UploadFile);
    }
};

void CUploadFileListPacket::InitData(word wFileCount, UploadFileList& FileList)
{
    this->wFileCount = wFileCount;
    this->FileList.assign(FileList.begin(), FileList.end());
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - ������ɰ�

void CDownloadFileCompletePacket::DoPack()
{
    CFinderPacket::DoPack();
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteString(strHashValue);
};

void CDownloadFileCompletePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
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
    CFinderPacket::DoPack();
    WriteString(strRef);
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
    ReadString(strRef);
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::InitData(string strUrl, string strRef, int64 nFileLength,int nTaskId)
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
    CReFinderPacket::DoPack();
};

void CRequestFilePacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
};

void CRequestFilePacket::InitData(word nResultCode)
{
    CReFinderPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
//CReOnPacket - ͨ��Ӧ���

void CReOnPacket::DoPack()
{
    CReFinderPacket::DoPack();
};

void CReOnPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
};

void CReOnPacket::InitData(word nResultCode)
{
    CReFinderPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
// CReGetResoursesPacket - �ظ��ͻ���һ��URL

void CReGetUrlPacket::DoPack()
{
    CReFinderPacket::DoPack();
    //��һ��URL
    WriteString(strReference);
    WriteString(strUrl);
    WriteString(strHashValue);
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    //��ȡһ��URL
    ReadString(strReference);
    ReadString(strUrl);
    ReadString(strHashValue);
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::InitData(word nResultCode, string strReference, string strUrl,
                               string strHashValue,int nTaskId)
{
    this->strReference = strReference;
    this->strUrl = strUrl;
    this->strHashValue = strHashValue;
    this->nTaskId = nTaskId;
    CReFinderPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReGetUserIpPacket - �ظ��ͻ���һ��IP
void CReGetUserIpPacket::DoPack()
{
    CReFinderPacket::DoPack();
    //��һ���û�IP
    int Number = UserList.size();
    WriteBuffer(&Number, sizeof(int));
    for(int i=0; i<Number; i++)
    {
        WriteBuffer(&UserList.at(i).nUserIp, sizeof(uint));
        WriteBuffer(&UserList.at(i).wTcpPort, sizeof(word));

    }
    WriteString(strHashValue);
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
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
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::InitData(word nResultCode, UserDataList& UserList, 
                                 string strHashValue, int nTaskId)
{
    this->UserList.assign(UserList.begin(), UserList.end());
    this->strHashValue = strHashValue;
    this->nTaskId = nTaskId;
    CReFinderPacket::InitData(nResultCode);
};


/////////////////////////////////////////////////////////////////////////////////
// CReNoopPacket - ����Դ��

void CReNoopPacket::DoPack()
{
    CReFinderPacket::DoPack();
    WriteBuffer(&nCount, sizeof(int));
};

void CReNoopPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    ReadBuffer(&nCount, sizeof(int));
};

void CReNoopPacket::InitData(word nResultCode, int nCount)
{
    this->nCount = nCount;
    CReFinderPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReIntegralPacket - ���ְ�

void CReIntegralPacket::DoPack()
{
    CReFinderPacket::DoPack();
    WriteBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    ReadBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::InitData(word nResultCode, int nIntegral)
{
    this->nIntegral = nIntegral;
    CReFinderPacket::InitData(nResultCode);
};
