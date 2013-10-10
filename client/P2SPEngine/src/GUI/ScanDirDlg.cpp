//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ScanDirDlg.h"
//---------------------------------------------------------------------
#pragma link "RzShellCtrls"
#pragma link "RzTreeVw"
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"
TScanDir *ScanDir;
//--------------------------------------------------------------------- 
__fastcall TScanDir::TScanDir(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TScanDir::FormPaint(TObject *Sender)
{
  this->Color=RGB(240,241,243);  
}
//---------------------------------------------------------------------------






