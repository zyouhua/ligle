//----------------------------------------------------------------------------
#ifndef ScanDirDlgH
#define ScanDirDlgH
//----------------------------------------------------------------------------
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include "RzShellCtrls.hpp"
#include "RzTreeVw.hpp"
#include <ComCtrls.hpp>
#include "RzBmpBtn.hpp"
//----------------------------------------------------------------------------
class TScanDir : public TForm
{
__published:
  TLabel *Label1;
  TRzShellTree *DirTree;
  TRzBmpButton *OKBtn;
  TRzBmpButton *CancelBtn;
  void __fastcall FormPaint(TObject *Sender);
private:
public:
	virtual __fastcall TScanDir(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TScanDir *ScanDir;
//----------------------------------------------------------------------------
#endif    
