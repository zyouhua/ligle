////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_block.cpp
//����޸�ʱ�䣺9-11-2006
////////////////////////////////////////////////////////////////////////////////



#include "core_block.h"
#include "core_channel.h"

/////////////////////////////////////////////////////////////////////////
// class CBlock

//-----------------------------------------------------------------
//  ���캯��
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
//  �������ؿ���Ϣ
//-----------------------------------------------------------------
void CBlock::ReSetBlock(int64 Start,int64 End)
{
  m_nStart=Start;
  m_nEnd=End;
  m_nSize=m_nEnd-m_nStart;
}
//-----------------------------------------------------------------
//�������ؿ����Ϣ
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
//    �������ؿ��״̬
//     ����:state�������ȥ��ʱ������1����һ��channel��������ʱ��-1
//--------------------------------------------------------------------
void CBlock::SetState(int state)
{
  m_nState+=state;
}
//--------------------------------------------------------------------
//    ���ÿ��Ƿ��Ѿ�������
//--------------------------------------------------------------------
bool CBlock::IsComplete()
{
  if(m_nStart>=m_nEnd) return true;
  return false;
}

////////////////////////////////////////////////////////////////////////
// class CBlockPool

//-----------------------------------------------------------------
//    ���캯��
//-----------------------------------------------------------------
CBlockPool::CBlockPool()
{
	m_Blocks=0;
}

//-----------------------------------------------------------------
//ɾ�����ؿ�
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
//   ��������
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
//�������ؿ�
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
//�������ؿ�
//-----------------------------------------------------------------
bool CBlockPool::GetBlock(CBlock* &pBlock)
{
  if(m_Blocks==0)  // δ�õ�����
  {
    pBlock=NULL;
    return true;
  }

  list<CBlock*>::iterator iter;
  CBlock* block=NULL;
  for(iter=m_BlockList.begin();iter!=m_BlockList.end();iter++)
  {
    if((*iter)->IsComplete()) continue;
    if(0==(*iter)->m_nState)            //����п��еĿ飬�򷵻ش˿�
    {
      (*iter)->SetState(1);
      pBlock= (*iter);
      return true;
    }
    if(block==NULL) { block=(*iter); continue;}
    if((*iter)->m_nSize > block->m_nSize) block=(*iter);
  }

  if(block==NULL)      //�������
  {
    pBlock=NULL;
    return false;
  }

  if(block->m_nSize > PIECELEN)           //����ҵ��Ŀ����16K���
  {
    pBlock=AssignBlock(block);
    return true;
  }

  pBlock=block;
  block->SetState(1);
  return true;

}

//-----------------------------------------------------------------
//������ؿ�
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
