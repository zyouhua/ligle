#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include<stdio.h>
#include<stdlib.h>
#include "database.h"
#include <vector>
struct CUrlData
{
   string strUrl;
   string strRef;
   //string strHashValue;
} ;
typedef  vector<CUrlData> UrlDataList;

class CResource : public CDbOperations
{
public:
    CUrlData     sUrlData;
    UrlDataList  UrlData;
    
private:

    string strHashValue,strUrl,strurl;
    int nRet, nFileId,UrlCount,nUrlCount,downloadcount;

    //����URL��
    bool CreateUrl(string& strUrl, string& strRef, int64 nFileLength);

    //�Ƿ��Ѿ����ڡ�
   uint IsOn(string& strurl, int64 nFileLength);

   //��ѯ��URL��FILELENGTH��
    string GetHashValue(string& strurl, int64 nFileLength);


   //��ѯ��HASHVALUES��FILELENGTH��
   uint GetUrl(int64 nFileLength, string& strHashValue, UrlDataList&  UrlData);

  void UpdateDownloadCount(int64 nFileLength,string& strHashValue);
   //�����һ��FILEID
  uint GetNextFileId();

public:
    //�����û�������������û��Ļ��֡�
    int Find(char strUserName[10], char strUserPassword[10]);

    //Ϊ�û����»��֡�
    bool  UpdateIntegral(char strUserName[10], char strUserPassword[10], int nIntegral);

   // �ϴ��ļ��б�ʱ����ļ�FILEID
   uint GetFileId(int64 nFileLength ,string&  strHashValue);

   // ������ɺ����ļ�FILEID
   uint Downloaded(string& strUrl, int64 nFileLength,string& strHashValue);

   //������Դʱ����ļ�URL FILEID

   uint GetResource(string& strUrl, string& strRef, int64 nFileLength,UrlDataList&  UrlData, string& HashValue);


};


#endif
