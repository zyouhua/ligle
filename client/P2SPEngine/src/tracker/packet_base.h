///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// packet_base.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _PACKET_BASE_H_
#define _PACKET_BASE_H_

//#include "ise_system.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////
// class CPacket

class CPacket
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // 缺省内存增长步长 (字节数，必须是 2 的 N 次方)

private:
    void Init();

protected:
    CMemoryStream *m_pStream;
    bool m_bAvailable;
    bool m_bIsPacked;

    void RaiseUnpackError();
    void RaisePackError();
    void CheckUnsafeSize(int nValue);

    void ReadBuffer(void *pBuffer, int nBytes);
    void WriteBuffer(void *pBuffer, int nBytes);
    void ReadString(string& str);
    void WriteString(const string& str);
    void ReadBlob(string& str);
    void ReadBlob(CStream& Stream);
    void ReadBlob(CBuffer& Buffer);
    void WriteBlob(void *pBuffer, int nBytes);

    void FixStrLength(string& str, int nLength);
    void TruncString(string& str, int nMaxLength);
protected:
    virtual void DoPack() {}
    virtual void DoUnpack() {}
    virtual void DoAfterPack() {}
    virtual void DoEncrypt() {}
    virtual void DoDecrypt() {}
public:
    CPacket();
    virtual ~CPacket();

    bool Pack();
    bool Unpack(void *pBuffer, int nBytes);
    void Clear();
    void EnsurePacked();

    void* GetBuffer() const { return (m_pStream? m_pStream->GetMemory() : NULL); }
    int GetSize() const { return (m_pStream? m_pStream->GetSize() : 0); }
    bool Available() const { return m_bAvailable; }
    bool IsPacked() const { return m_bIsPacked; }
};

///////////////////////////////////////////////////////////////////////////////

#endif // _PACKET_BASE_H_ 
