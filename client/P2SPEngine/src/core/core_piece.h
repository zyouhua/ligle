////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_piece.h
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_PIECE_H_
#define _CORE_PIECE_H_


#include "icl_classes.h"


using namespace icl;

class CTask;

const int PIECELEN=1024*16;

//-------------------------------------------------------
//-------------------------------------------------------

struct CPiece
{
  char Buffer[PIECELEN];
  int64 Position;
  int Length;
};

//-----------------------------------------------------
//-----------------------------------------------
class CPiecePool
{
private:
  CCriticalSection m_Lock;
	list<CPiece*> m_PieceList;
	int m_nSize;
  long m_Speed;
  int m_Times;
public:
	CPiecePool();
	~CPiecePool(){Clear();}
	void AddPiece(CPiece *pPiece);
  bool Handle(CTask* pTask);
  void Save(string& FileName);
	void Clear();
};

#endif
