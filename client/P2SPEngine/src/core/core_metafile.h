////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine (P2SP CE)
//   metafile.h
//   ����޸�ʱ��29-11-2006
////////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_METAFILE_H_
#define _CORE_METAFILE_H_

#include "icl_classes.h"
#include "icl_sysutils.h"

//#include <string>

using namespace icl;



const int METAFILE_URL    =   200;
const int METAFILE_NAME   =   100;
const int METAFILE_CODE   =   34;


////////////////////////////////////////////////////////////////////////////////
//Ԫ�ļ�ͷ
#pragma pack(1)

struct CMetaData
{
	int  State;                     //�ļ�״̬��0��δ��ɣ�1������� 2����ɾ��
	char Url[METAFILE_URL];         //�ļ�ԴURL
  char Ref[METAFILE_URL];
  char FileName[METAFILE_NAME];   //�����ļ�������·����
	char FileCode[METAFILE_CODE];   //�ļ�hashֵ
	int64 FileSize;                  //�ļ���С
  int  Index;                     //�ļ����
public:
  CMetaData() { Clear(); }
  void Clear() { ZeroBuffer(this, sizeof(*this)); }
};

#pragma pack()

typedef list<CMetaData*> CMetaList;//Ԫ�ļ��б�

////////////////////////////////////////////////////////////////////////////////
// Ԫ�ļ�������

class CMetaFile
{
private:
  string m_FileName;
  int m_Records;
  int m_DataLen;

public:
  CMetaFile();
  ~CMetaFile(){};

  void UpDate(CMetaData* pMetaData);
  void Insert(CMetaData* pMetaData);
  void Delete(int nIndex);
  bool GetFile(char* pFileCode,char* pFileName);
  bool GetFile(char* pUrl);
  int  Load(CMetaList& List,int type );
  bool Load(int TaskID,CMetaData* pMetaData);
  void Pack();
};

extern CMetaFile MetaFile;

#endif   //End  MetaFile.h
