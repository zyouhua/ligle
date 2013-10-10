//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>

#include "icl_stlport.h"
#include <Buttons.hpp>
#include <Graphics.hpp>
#include "trayicon.h"

//自定义消息
#define WM_NEWTASK    WM_APP+101
#define WM_MULITPTASK WM_USER+102
#define WM_NOTIFYICON WM_USER +103
#define APP_NAME "Ligle"

AnsiString ErrorInfomation[7]={"","连接超时","域名无法解析","URL地址错误",
                              "磁盘已满","文件读写错误","无法访问网站"};
//---------------------------------------------------------------------------
class TMainFrm : public TForm
{
__published:	// IDE-managed Components
  TPanel *FormPanel;
  TListView *DownLoadView;
  TTimer *Timer1;
  TPopupMenu *DownLoadPopupMenu;
  TMenuItem *D_StartTask;
  TMenuItem *D_StopTask;
  TListView *CompleteView;
  TListView *RecycleView;
  TPopupMenu *CompletePopupMenu;
  TMenuItem *C_DelTask;
  TMenuItem *D_DelTask;
  TPanel *LeftPanel;
  TSplitter *LeftSplitter;
  TPanel *RecyclePanel;
  TPanel *CompletePanel;
  TPanel *DownLoadPanel;
  TTreeView *ScanTree;
  TMainMenu *MainMenu;
  TMenuItem *File;
  TMenuItem *M_NewTask;
  TMenuItem *S1;
  TMenuItem *M_StartTask;
  TMenuItem *M_StopTask;
  TMenuItem *M_DelTask;
  TMenuItem *M_ReDown;
  TMenuItem *s2;
  TMenuItem *M_Exit;
  TMenuItem *Edit;
  TMenuItem *View;
  TMenuItem *M_ToolBar;
  TMenuItem *M_Status;
  TMenuItem *SetUp;
  TMenuItem *Help;
  TStatusBar *MainStatusBar;
  TToolBar *MainToolBar;
  TImageList *TreeImage;
  TMenuItem *N1;
  TMenuItem *D_SelectAll;
  TMenuItem *D_StartAll;
  TMenuItem *D_StopAll;
  TMenuItem *D_DelAll;
  TMenuItem *N2;
  TMenuItem *N3;
  TMenuItem *C_SelectAll;
  TMenuItem *C_DelAll;
  TPopupMenu *RecyclePopupMenu;
  TMenuItem *R_DelTask;
  TMenuItem *R_DelAll;
  TMenuItem *C_Open;
  TMenuItem *N4;
  TImageList *ResImageList;
  TSpeedButton *T_NewTask;
  TToolButton *ToolButton1;
  TSpeedButton *T_StartTask;
  TSpeedButton *T_StopTask;
  TSpeedButton *T_DelTask;
  TToolButton *ToolButton2;
  TSpeedButton *T_Open;
  TSpeedButton *T_Dir;
  TMenuItem *R_Open;
  TToolBar *MenuBar;
  TImage *Image1;
  TLabel *Label1;
  TPanel *RightPanel;
  TPanel *InfoPanel;
  TSplitter *vSplitter;
  TImage *InfoImage;
  TLabel *Label2;
  TListBox *InfoBox;
  TPopupMenu *TrayMenu;
  TMenuItem *N5;
  TMenuItem *N6;
  TMenuItem *C_ReDown;
  TMenuItem *N7;
  TMenuItem *N8;
  TMenuItem *N9;
  TMenuItem *R_ReDown;
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall D_StartTaskClick(TObject *Sender);
  void __fastcall D_StopTaskClick(TObject *Sender);
  void __fastcall DownLoadViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall CompleteViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall D_DelTaskClick(TObject *Sender);
  void __fastcall C_DelTaskClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ScanTreeMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall M_NewTaskClick(TObject *Sender);
  void __fastcall M_ToolBarClick(TObject *Sender);
  void __fastcall M_StatusClick(TObject *Sender);
  void __fastcall Tool_NewClick(TObject *Sender);
  void __fastcall D_SelectAllClick(TObject *Sender);
  void __fastcall C_SelectAllClick(TObject *Sender);
  void __fastcall D_StartAllClick(TObject *Sender);
  void __fastcall D_StopAllClick(TObject *Sender);
  void __fastcall D_DelAllClick(TObject *Sender);
  void __fastcall C_DelAllClick(TObject *Sender);
  void __fastcall RecycleViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall R_DelTaskClick(TObject *Sender);
  void __fastcall R_DelAllClick(TObject *Sender);
  void __fastcall DownLoadViewDblClick(TObject *Sender);
  void __fastcall CompleteViewDblClick(TObject *Sender);
  void __fastcall C_OpenClick(TObject *Sender);
  void __fastcall T_NewTaskClick(TObject *Sender);
  void __fastcall T_StartTaskClick(TObject *Sender);
  void __fastcall T_StopTaskClick(TObject *Sender);
  void __fastcall T_DelTaskClick(TObject *Sender);
  void __fastcall T_OpenClick(TObject *Sender);
  void __fastcall R_OpenClick(TObject *Sender);
  void __fastcall T_DirClick(TObject *Sender);
  void __fastcall M_ExitClick(TObject *Sender);
  void __fastcall MainToolBarCustomDraw(TToolBar *Sender,
          const TRect &ARect, bool &DefaultDraw);
  void __fastcall MenuBarCustomDraw(TToolBar *Sender, const TRect &ARect,
          bool &DefaultDraw);
  void __fastcall N5Click(TObject *Sender);
  void __fastcall N6Click(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall C_ReDownClick(TObject *Sender);
  void __fastcall R_ReDownClick(TObject *Sender);
private:	// User declarations
  void __fastcall CNewTask(TMessage& msg);
  void __fastcall CMulitpTask(TMessage& msg);
  void __fastcall WndProc(Messages::TMessage   &Message);
  AnsiString bmppath;
  NOTIFYICONDATA icon;
  void __fastcall NotifyIcon(TMessage &msg);
  void ShowTaskInfo(int TaskId);
public:		// User declarations
  BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_NEWTASK,TMessage,CNewTask)
    MESSAGE_HANDLER(WM_MULITPTASK,TMessage,CMulitpTask)
    MESSAGE_HANDLER(WM_NOTIFYICON,TMessage,NotifyIcon)
  END_MESSAGE_MAP(TForm)
  bool SerachTask(int TaskId,int& Index,TListView* ListView);
  __fastcall TMainFrm(TComponent* Owner);
  void DoUpDate(int);
  void DoComplete(int);
  void DoInsert(int);
  void DoDelete(int);
  AnsiString Rate(double DownLoad,double Length);
  AnsiString Speed(int Speed);
  AnsiString Size(double Length);
  void InitView(TListView*, int);
  AnsiString GetFileName(AnsiString FullName);
  void SetDownLoadPopup(bool m1, bool m2, bool m3, bool m4, bool m5, bool m6);
  void SetCompletePopup(bool m1, bool m2, bool m3, bool m4);
  void SetToolBar(bool t1, bool t2, bool t3, bool t4, bool t5);
  AnsiString GetReMainTime(__int64 Down, __int64 Size,int Speed);
private:
  bool CanExit,TrayShow;
  void ReDownTask(int TaskId,TListView *View);

};
//---------------------------------------------------------------------------
extern PACKAGE TMainFrm *MainFrm;
//---------------------------------------------------------------------------
#endif
