////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_piece.cpp
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////
#include "core_piece.h"
#include "core_Task.h"
////////////////////////////////////////////////////////////////////////////////
//class CPiecePool


//-----------------------------------------------------------------
//添加片
//Piece:片对象指针的引用
//-----------------------------------------------------------------
void CPiecePool::AddPiece(CPiece* Piece)
{

	CAutoSynchronizer AutoSyncher(m_Lock);
  m_PieceList.push_back(Piece);
	m_nSize+=1;
  m_Speed+=Piece->Length;
}

CPiecePool::CPiecePool()
{
	m_nSize=0;
  m_Speed=0;
  m_Times=0;
}

void CPiecePool::Clear()
{
	list<CPiece*>::iterator iter;
  for(iter=m_PieceList.begin();iter!=m_PieceList.end();iter++)
	{
		if(*iter) delete (*iter);
  }
  m_PieceList.clear();
  m_nSize=0;
}
//-----------------------------------------------------------------
//处理片
//-----------------------------------------------------------------
bool CPiecePool::Handle(CTask* pTask)
{
	CAutoSynchronizer AutoSyncher(m_Lock);
  if(++m_Times==10)
  {
    long Speed=(m_Speed+(pTask->m_Speed))/2;
    pTask->m_Speed=Speed;
    m_Speed=0;
    m_Times=0;
    AppEventPool.Notify(SENDER_CORE,pTask->m_nFileID,EVENT_UPDATE);
  }

  if(m_nSize<20) return false;
  return true;
}

void CPiecePool::Save(string& FileName)
{
	CAutoSynchronizer AutoSyncher(m_Lock);

  list<CPiece*>::iterator iter;
  CFileStream File(FileName,FM_OPEN_WRITE);
  for(iter=m_PieceList.begin();iter!=m_PieceList.end();iter++)
  {
    File.Seek((*iter)->Position,SO_BEGINNING);
    if(!(File.Write((*iter)->Buffer,(*iter)->Length))) throw CException();
  }
  Clear();
}
