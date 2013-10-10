////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_block.cpp
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////



#include "core_block.h"
#include "core_channel.h"

/////////////////////////////////////////////////////////////////////////
// class CBlock

//-----------------------------------------------------------------
//  构造函数
//-----------------------------------------------------------------
CBlock::CBlock(int64 Start,int64 End,int State)
{
  m_nStart=Start;
  m_nEnd=End;
  m_nSize=m_nEnd-m_nStart;
  m_nState=State;
}

CBlock::~CBlock()
{
//        cout<<"block!~~~~"<<endl;
}
//-----------------------------------------------------------------
//  重置下载块信息
//-----------------------------------------------------------------
void CBlock::ReSetBlock(int64 Start,int64 End)
{
  m_nStart=Start;
  m_nEnd=End;
  m_nSize=m_nEnd-m_nStart;
}
//-----------------------------------------------------------------
//更新下载块的信息
//-----------------------------------------------------------------
void CBlock::UpDate(int DownLoaded)
{
	if(!IsComplete())
	{
		m_nStart+=DownLoaded;
		m_nSize-=DownLoaded;
	}
}
//--------------------------------------------------------------------
//    设置下载块的状态
//     参数:state当分配出去的时候，则置1，当一个channel不能连接时置-1
//--------------------------------------------------------------------
void CBlock::SetState(int state)
{
  m_nState+=state;
}
//--------------------------------------------------------------------
//    检查该块是否已经下载完
//--------------------------------------------------------------------
bool CBlock::IsComplete()
{
  if(m_nStart>=m_nEnd) return true;
  return false;
}

////////////////////////////////////////////////////////////////////////
// class CBlockPool

//-----------------------------------------------------------------
//    构造函数
//-----------------------------------------------------------------
CBlockPool::CBlockPool()
{
	m_Blocks=0;
}

//-----------------------------------------------------------------
//删除下载块
//-----------------------------------------------------------------
void CBlockPool::DeleteBlock(CBlock* &pBlock)
{
  if(pBlock)
  {
    delete pBlock;
    m_BlockList.remove(pBlock);
    m_Blocks--;
  }
}

//-----------------------------------------------------------------
//   析构函数
//-----------------------------------------------------------------
void CBlockPool::Clear()
{
  m_Blocks=0;
  m_FileName="";

  list<CBlock*>::iterator iter;
  for(iter=m_BlockList.begin();iter!=m_BlockList.end();iter++)
    delete (*iter);
  m_BlockList.clear();
}

//-----------------------------------------------------------------
//分配下载块
//-----------------------------------------------------------------
CBlock* CBlockPool::AssignBlock(CBlock *Block)
{
  int64 start=Block->m_nStart+(Block->m_nSize+PIECELEN)/PIECELEN/2*PIECELEN;
  CBlock* NewBlock=new CBlock(start,Block->m_nEnd);
  Block->ReSetBlock(Block->m_nStart,NewBlock->m_nStart);
  AddBlock(NewBlock);
  NewBlock->SetState(1);
  return NewBlock;
}

//-----------------------------------------------------------------
//返回下载块
//-----------------------------------------------------------------
bool CBlockPool::GetBlock(CBlock* &pBlock)
{
  if(m_Blocks==0)  // 未得到长度
  {
    pBlock=NULL;
    return true;
  }

  list<CBlock*>::iterator iter;
  CBlock* block=NULL;
  for(iter=m_BlockList.begin();iter!=m_BlockList.end();iter++)
  {
    if((*iter)->IsComplete()) continue;
    if(0==(*iter)->m_nState)            //如果有空闲的块，则返回此块
    {
      (*iter)->SetState(1);
      pBlock= (*iter);
      return true;
    }
    if(block==NULL) { block=(*iter); continue;}
    if((*iter)->m_nSize > block->m_nSize) block=(*iter);
  }

  if(block==NULL)      //下载完成
  {
    pBlock=NULL;
    return false;
  }

  if(block->m_nSize > PIECELEN)           //如果找到的块大于16K则分
  {
    pBlock=AssignBlock(block);
    return true;
  }

  pBlock=block;
  block->SetState(1);
  return true;

}

//-----------------------------------------------------------------
//添加下载块
//-----------------------------------------------------------------
void CBlockPool::AddBlock(CBlock* pBlock)
{
	m_BlockList.push_back(pBlock);
	m_Blocks+=1;
}

void CBlockPool::SetFileName(string& FileName)
{
  m_FileName=FileName+".blc";
}

int64 CBlockPool::Load(int64 FileLength)
{
  if(!FileLength) return 0;

  int64 Remain=0;
  CBlock* pBlock=new CBlock;
  try
  {
    CFileStream FileStream(m_FileName,FM_OPEN_READ);
    while(FileStream.Read(pBlock,sizeof(int64)*3))
    {
      Remain+=pBlock->m_nSize;
      AddBlock(pBlock);
      pBlock=new CBlock;
    }
  }
  catch (CException& e)
  {
    CBlock* Block=new CBlock(0,FileLength,0);
    AddBlock(Block);
    Remain=FileLength;
  }
  delete pBlock;
  return (FileLength-Remain);
}

void CBlockPool::Save()
{
  CFileStream FileStream(m_FileName, FM_CREATE | FM_SHARE_DENY_WRITE);
  list<CBlock*>::iterator iter;
  for(iter=m_BlockList.begin();iter!=m_BlockList.end();iter++)
	  if(!((*iter)->IsComplete())) FileStream.Write((*iter),sizeof(int64)*3);
}
