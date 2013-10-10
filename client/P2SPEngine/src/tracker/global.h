///////////////////////////////////////////////////////////////////////////////
// P2SP Client Daemon (P2SP_CD)
//
// �ļ�����: global.h
// ��������: ȫ�ֳ��������Ͷ���
// ����޸�: 2005-12-22
///////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"

#include "icl_types.h"
#include "icl_sysutils.h"
#include "icl_socket.h"

using namespace icl;

///////////////////////////////////////////////////////////////////////////////

// disable warning: "cannot create pre-compiled header"
#ifdef __BORLANDC__
#pragma warn -pch
#endif

///////////////////////////////////////////////////////////////////////////////
// ��Ŀ����س���

// ������� (��������)
const char* const SOFTWARE_NAME         = "p2spcd";
// ��Ŀ���� (�����ڲ�����)
const char* const PROJECT_NAME          = "p2spcd";

///////////////////////////////////////////////////////////////////////////////
// �ļ�����س���

// ��־�ļ��� (����·��)
const char* const LOG_FILENAME          = "log";
// �־���Ϣ�ļ��� (����·��)
const char* const PERSISTENT_FILENAME   = "config";
// Ԫ�ļ�Ŀ¼��
const char* const METAFILES_DIR_NAME    = "metafiles";

///////////////////////////////////////////////////////////////////////////////
// ������س���

// Tracker �������ĵ�ַ�Ͷ˿�
const char* const TRACKER_SERVER_ADDR   = "192.168.0.108";
const int TRACKER_SERVER_UDP_PORT       = 9100;

// �ͻ��˵Ķ˿�
const int CLIENT_UDP_PORT_START         = 7752;       // �ͻ���UDP��ʼ�˿�
const int CLIENT_TCP_PORT_START         = 7752;       // �ͻ���TCP��ʼ�˿�
const int CHECK_PORT_TIMES              = 1000;       // �ͻ��˰󶨶˿ڵ����Դ���

///////////////////////////////////////////////////////////////////////////////
// BT ��س���

// ��Ĵ�С(�ֽ���)
const int BT_PIECE_SIZE                 = 1024*256;
// Ƭ�Ĵ�С(�ֽ���)
const int BT_SLICE_SIZE                 = 1024*16;
// һ�����е�Ƭ��
const int BT_SLICES_PER_PIECE           = BT_PIECE_SIZE / BT_SLICE_SIZE;

// BT�����У�BT���ݰ�������ֽ���
const int BT_MAX_PACKET_SIZE            = 1024*1024*1;
// BT�ļ�������ֽ���(8G)
const int64 BT_MAX_FILE_SIZE            = (int64)1024*1024*1024*8;
// ����һ���ļ�ʱ�����ͬʱ���ؼ�����
const int BT_MAX_RECVING_PIECES         = 4;
// ��ˮ��������������
const int BT_MAX_REQUESTS               = 8;
// ϵͳ����������� (�����ͨ�û�)
const int BT_MAX_CONNS_FOR_PEER         = 80;
// ϵͳ����������� (���CDN�ڵ�)
const int BT_MAX_CONNS_FOR_CDN          = 10000;

// ����һ���ļ�ʱ���������ٸ�����
const int BT_CONNS_PER_RECVING_FILE     = 10;
// ������ͨ�û�������һ���ļ��������������м���CDN�ڵ�����
const int BT_MAX_CDN_CONN_FOR_PEER      = 1;
// ÿ�λ�ȡ���� Peers
const int BT_GET_PEERS_PER_TIME         = 30;
// ����һ���ļ�ʱ�����(�������ٶ� - CDN�����ٶ�) > ��ֵ����Ͽ�CDN���ӡ�(Bytes/S)
const int BT_DISCARD_CDN_COND_RATE      = 1024*100;
// ����һ���ļ�ʱ����������ٶ� < ��ֵ����ϵ��������ӡ�(Bytes/S)
const int BT_DISCARD_WEAK_COND_RATE     = 1024*100;
// ����С�ڴ�ֵ�����ӣ�����Ϊ�ǵ������ӡ�(Bytes/S)
const int BT_WEAK_CONN_RATE             = 1024*5;
// ϵͳ������������ ��ֵ% ʱ���������������
const int BT_CONNS_ALERT_PERCENT        = 90;

// �ļ�ID�ĳ���
const int BT_FILE_ID_SIZE               = 28;
// BT�������ֵĳ�ʱʱ��(����)
const int BT_HANDSHAKE_TIMEOUT          = 1000*60*1;
// ����һ����ʱ����������������Ϊ��ʱ(����)
const int BT_PIECE_RECV_TIMEOUT         = 1000*30;
// ����һ��Ƭʱ����������������Ϊ��ʱ(����)
const int BT_SLICE_RECV_TIMEOUT         = 1000*5;
// BT���ӹ�ϣ����Ͱ������(����: 1543,3079,6151,12289,24593,49157,98317,196613,...)
const int BT_CONN_HASH_TABLE_BUCKETS    = 12289;     // (ռ��48K)

///////////////////////////////////////////////////////////////////////////////
// ��Ŀ�ɵ�����

// DTP�����ڽ��ܴ������е��ͣ��ʱ��(����)
const int DTP_ACCEPT_TIMEOUT            = 1000*60;
// DTP���ӽ��ܴ��������������
const int DTP_ACCEPT_HANDLER_CAPACITY   = 10000;

// TCP�������ӳ�ʱʱ��(����)
const int TCP_NORMAL_CONN_TIMEOUT       = 1000*15;
// TCP�������ӳ�ʱʱ��(����)
const int TCP_INVERT_CONN_TIMEOUT       = 1000*30;
// TCP���ӷ������ְ��ĳ�ʱʱ��(����)
const int TCP_SEND_GREET_PKT_TIMEOUT    = 1000*30;

// ����UDP����Ĭ�����Դ���
const int DEF_UDP_SEND_TRY_TIMES        = 5;
// �ȴ�����UDP����Ĭ��ʱ��(����)
const int DEF_UDP_SEND_RECV_TIMEOUT     = 1000*3;

// ϵͳ�����ռ���ʱ����(����)
const int COLLECT_GARBAGE_INTERVAL      = 1000*10;

///////////////////////////////////////////////////////////////////////////////
// ���ý������

const int RET_SUCCESS                   = 0;        // �ɹ�
const int RET_FAILURE                   = 1;        // ʧ��

const int RET_BASE                      = 100;      // �ǹ��ý��������� >= ��ֵ!

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

#pragma pack(1)     // 1�ֽڶ���

// Э������(���ײ��ĵ�һ���ֽ�)
enum PacketProto {
    PP_UDP         = 1,         // UDPҵ���
    PP_UTP         = 2,         // UTP���ݰ�
};
enum { MAX_PACKET_PROTO = 2 };  // Э�������������

// ���䷽��
enum TransferDir {
    TD_SEND,                    // ����
    TD_RECV                     // ����
};

// �û����ӷ�ʽ
enum ConnectType {
    CT_UNKNOWN      = 0,        // ��������
    CT_DIRECT       = 1,        // ֱ��
    CT_SOCKS5       = 2,        // SOCKS5
    CT_LAN          = 3         // ������
};

// Peer ����
enum PeerType {
    PT_UNKNOWN      = 0,        // δ֪�û�
    PT_PEER         = 1,        // ��ͨ�û�
    PT_CDN          = 2         // CDN �ڵ�
};

// ����״̬
enum DownloadState {
    DS_UNKNOWN      = 0,        // δ֪״̬
    DS_DOWNLOAD     = 1,        // ��������
    DS_PAUSE        = 2,        // ��ͣ����
    DS_COMPLETE     = 3,        // �������
    DS_FAILURE      = 4         // ����ʧ��
};

// ��������
enum TaskKind {
    TK_INCOMPLETE   = 0,        // δ���
    TK_COMPLETE     = 1,        // �����
    TK_ALL          = 2         // ȫ��
};

// ��������ַ
struct CServerAddress
{
    uint nIp;
    word nUdpPort;
    word nTcpPort;

    CServerAddress() { Clear(); }
    void Clear() { ZeroBuffer(this, sizeof(*this)); }
    CPeerAddress GetUdpAddr() const { return CPeerAddress(nIp, nUdpPort); }
    CPeerAddress GetTcpAddr() const { return CPeerAddress(nIp, nTcpPort); }
};

// �û���Ϣ (����GetPeerList)
struct CPeerInfo
{
    uint nPeerId;               // PeerId
    uint nIp;                   // IP (���ҷ����Կ��õ�IP����ͬ)
    word nUdpPort;              // UDP Port
    word nTcpPort;              // TCP Port
    byte nPeerType;             // Peer ����

public:    
    CPeerInfo() { Clear(); }
    void Clear() { ZeroBuffer(this, sizeof(*this)); }
    CPeerAddress GetUdpAddr() const { return CPeerAddress(nIp, nUdpPort); }
    CPeerAddress GetTcpAddr() const { return CPeerAddress(nIp, nTcpPort); }
};

typedef vector<CPeerInfo> PeerInfoList;

// �ļ���Ϣ (����UploadFileList)
struct CFileInfo
{
    string strFileId;

public:
    CFileInfo() {}
    CFileInfo(const string& strFileId) { this->strFileId = strFileId; }
};

typedef vector<CFileInfo> FileInfoList;

#pragma pack()

///////////////////////////////////////////////////////////////////////////////

#endif // _GLOBAL_H_
