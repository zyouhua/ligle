unit Calc;

interface

uses
  Classes, SysUtils, Math, CipherAlgo;

function CalcFileGuid(const FileName: string): string;

implementation

//-----------------------------------------------------------------------------
// ����: �����ļ�GUID (�������׳��쳣)
// ����:
//   �ļ�GUID (���ȹ̶�Ϊ32�ֽڣ���дHEX�ַ����룬�� "FEA52B0C...")
// �㷨:
//   1. �� FileSize <= 5 M, ���ȡ�����ļ��������ϣֵ��
//   2. �� FileSize > 5 M, �������ѡ N �������Σ������ϣֵ��
//   3. ��ѡ�����εĹ���: �����ι̶�8K��ÿ 80K ��ȡһ�������Σ������ε�
//      ���� N �����ֵΪ 1000���������ļ�̫���� N �������ֵ�����ʵ�
//      �Ӵ���������
//   4. �����ϣֵʱ���ļ�����(Int64)ֵ���ȿ����ļ����ݲ����ϣֵ���㣻
//   5. ���������������ļ�β�����뱻��������
//-----------------------------------------------------------------------------
function CalcFileGuid(const FileName: string): string;
const
  HashAlgorithmClass: THashClass = THash_MD5;    // ��ϣ�㷨(MD5)
  CriticalSizeOfFullHash         = 1024*1024*5;  // ȫ��ϣ�ٽ��ļ���С(5M)
  SparseHashSampleSize           = 1024*8;       // ������ϣʱ�Ĳ����δ�С(8K)
  SparseHashSampleSpace          = 1024*80;      // ������ϣʱ�Ĳ������(80K)
  SparseHashMaxSamples           = 100;          // ������ϣʱ������������

  // ȫ��ϣ
  function FullHashFile(FileStream: TFileStream; FileSize: Int64): string;
  const
    HashBlockSize = 1024*4;
  var
    Hash: THash;
    BlockBuf: PChar;
    BlockBytes: Integer;
  begin
    FileStream.Position := 0;
    Hash := HashAlgorithmClass.Create;
    BlockBuf := AllocMem(HashBlockSize);
    try
      Hash.Init;

      Hash.Calc(FileSize, SizeOf(Int64));
      repeat
        BlockBytes := FileStream.Read(BlockBuf^, HashBlockSize);
        if BlockBytes > 0 then
          Hash.Calc(BlockBuf^, BlockBytes);
      until BlockBytes < HashBlockSize;

      Hash.Done;
      Result := Hash.DigestBase16;
    finally
      FreeMem(BlockBuf);
      Hash.Free;
    end;
  end;

  // ������ϣ
  function SparseHashFile(FileStream: TFileStream; FileSize: Int64): string;
  var
    Hash: THash;
    SampleBuf: PChar;
    Bytes, TailBytes, SampleSpace: Integer;
    FilePos: Int64;
  begin
    FileStream.Position := 0;
    
    Hash := HashAlgorithmClass.Create;
    SampleBuf := AllocMem(SparseHashSampleSize);
    try
      Hash.Init;

      // ����ʵ�ʲ������
      SampleSpace := SparseHashSampleSpace;
      if FileSize div SparseHashSampleSpace > SparseHashMaxSamples then
        SampleSpace := FileSize div SparseHashMaxSamples;

      Hash.Calc(FileSize, SizeOf(Int64));
      while True do
      begin
        Bytes := FileStream.Read(SampleBuf^, SparseHashSampleSize);
        if Bytes > 0 then
          Hash.Calc(SampleBuf^, Bytes);
        if Bytes < SparseHashSampleSize then
          Break;

        FilePos := FileStream.Position;
        if (FileSize - FilePos > 0) and
          (FileSize - FilePos <= SampleSpace - SparseHashSampleSize) then
        begin
          // ��֤�������ļ�ĩ�˶���(���ļ�ĩ��һ��������)
          TailBytes := Min(SparseHashSampleSize, FileSize - FilePos);
          FileStream.Seek(-TailBytes, soEnd);
        end else
          FileStream.Seek(SampleSpace - SparseHashSampleSize, soCurrent);
      end;

      Hash.Done;
      Result := Hash.DigestBase16;
    finally
      FreeMem(SampleBuf);
      Hash.Free;
    end;
  end;

var
  FileStream: TFileStream;
  FileSize: Int64;
begin
  FileStream := TFileStream.Create(FileName, fmOpenRead or fmShareDenyNone);
  try
    FileSize := FileStream.Size;
    if FileSize <= CriticalSizeOfFullHash then
      Result := FullHashFile(FileStream, FileSize)
    else
      Result := SparseHashFile(FileStream, FileSize);
  finally
    FileStream.Free;
  end;
end;

end.
