////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_block.h
//最后修改时间：3-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_BLOCK_H
#define _CORE_BLOCK_H


#include <list>
#include <iostream>
#include "icl_classes.h"
#include "icl_sysutils.h"
using namespace icl;

//-------------------------------------------------------------------------
//下载块类，存储了下载所须的信息，
//例如：启始位置，结束位置
//m_nState:下载块的状态(既对应该块的连接数)：0：没有连接既空闲
//-------------------------------------------------------------------------
#pragma pack(1)

struct CBlock
{
	int64 m_nStart;
	int64 m_nEnd;
	int64 m_nSize;
	int m_nState;
public:
	CBlock(int64 Start,int64 End,int State=0);
	CBlock(){ZeroBuffer(this, sizeof(*this));}
	~CBlock();
	void SetState(int state);
	void UpDate(int DownLoaded);
	bool IsComplete();
	void ReSetBlock(int64 Start,int64 End);
	void Detach(){m_nState--;}
};

#pragma pack()

//-----------------------------------------------------------------
//下载块容器类，用来存储下载信息块，并分配，和管理
//-----------------------------------------------------------------
class CBlockPool
{
private:
  list<CBlock*> m_BlockList;
	int m_Blocks;
  string m_FileName;
private:
	CBlock* AssignBlock(CBlock *pBlock);
	void DeleteBlock(CBlock* &pBlock);
public:
	CBlockPool();
	~CBlockPool(){Clear();}
	void Clear();
  void SetFileName(string& FileName);
  void DeleteBlcFile(){DeleteFile(m_FileName);}
  int64 Load(int64 FileLength);
  void Save();
	bool GetBlock(CBlock* &Block);
	void AddBlock(CBlock* Block);
};

#endif