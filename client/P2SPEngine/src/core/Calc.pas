unit Calc;

interface

uses
  Classes, SysUtils, Math, CipherAlgo;

function CalcFileGuid(const FileName: string): string;

implementation

//-----------------------------------------------------------------------------
// 描述: 计算文件GUID (出错则抛出异常)
// 返回:
//   文件GUID (长度固定为32字节，大写HEX字符编码，如 "FEA52B0C...")
// 算法:
//   1. 若 FileSize <= 5 M, 则读取整个文件，计算哈希值；
//   2. 若 FileSize > 5 M, 则均匀挑选 N 个采样段，计算哈希值；
//   3. 挑选采样段的规则: 采样段固定8K，每 80K 抽取一个采样段，采样段的
//      个数 N 的最大值为 1000，若由于文件太大导致 N 超过最大值，则适当
//      加大采样间隔；
//   4. 计算哈希值时，文件长度(Int64)值首先看成文件内容参与哈希值计算；
//   5. 无论怎样采样，文件尾部必须被采样到；
//-----------------------------------------------------------------------------
function CalcFileGuid(const FileName: string): string;
const
  HashAlgorithmClass: THashClass = THash_MD5;    // 哈希算法(MD5)
  CriticalSizeOfFullHash         = 1024*1024*5;  // 全哈希临界文件大小(5M)
  SparseHashSampleSize           = 1024*8;       // 采样哈希时的采样段大小(8K)
  SparseHashSampleSpace          = 1024*80;      // 采样哈希时的采样间隔(80K)
  SparseHashMaxSamples           = 100;          // 采样哈希时的最大采样段数

  // 全哈希
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

  // 采样哈希
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

      // 计算实际采样间隔
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
          // 保证采样与文件末端对齐(即文件末端一定被采样)
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
