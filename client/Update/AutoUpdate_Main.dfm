object Form1: TForm1
  Left = 192
  Top = 114
  Width = 393
  Height = 149
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = #33258#21160#26356#26032
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 32
    Width = 111
    Height = 16
    AutoSize = False
    Caption = #21457#29616#26032#29256#26412#65292#35831#26356#26032
  end
  object Button1: TButton
    Left = 184
    Top = 72
    Width = 105
    Height = 25
    Caption = #24320#22987#26356#26032
    TabOrder = 0
    OnClick = Button1Click
  end
  object IdHTTP1: TIdHTTP
    Request.Accept = 'text/html, */*'
    Request.ContentLength = 0
    Request.ContentRangeEnd = 0
    Request.ContentRangeStart = 0
    Request.ProxyPort = 0
    Request.UserAgent = 'Mozilla/3.0 (compatible; Indy Library)'
    Left = 96
    Top = 72
  end
end
