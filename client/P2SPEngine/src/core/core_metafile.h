////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine (P2SP CE)
//   metafile.h
//   最后修改时间29-11-2006
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
//元文件头
#pragma pack(1)

struct CMetaData
{
	int  State;                     //文件状态，0：未完成，1：已完成 2：已删除
	char Url[METAFILE_URL];         //文件源URL
  char Ref[METAFILE_URL];
  char FileName[METAFILE_NAME];   //本地文件名（含路径）
	char FileCode[METAFILE_CODE];   //文件hash值
	int64 FileSize;                  //文件大小
  int  Index;                     //文件序号
public:
  CMetaData() { Clear(); }
  void Clear() { ZeroBuffer(this, sizeof(*this)); }
};

#pragma pack()

typedef list<CMetaData*> CMetaList;//元文件列表

////////////////////////////////////////////////////////////////////////////////
// 元文件管理类

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
