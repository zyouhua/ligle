///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// �ļ�����: global.h
// ��������: ȫ�ֳ��������Ͷ���
// ����޸�: 2005-12-1
///////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_types.h"

///////////////////////////////////////////////////////////////////////////////
// ��������

//-----------------------------------------------------------------------------
//-- �ļ�����:

// ��־�ļ��� (����·��)
#define SERVER_LOG_FILE         "log"

// �����ļ��� (����·��)
#define SERVER_CONFIG_FILE      "config.xml"

//-----------------------------------------------------------------------------
//-- ���糣��:

// �������ݰ�Э��汾����
const int PROTOCOL_VER_1                = 1;

//----------------------------------------------------------------------------
//-- ����ߴ糣��:

const int MAX_UDP_FILE_LIST_COUNT = 19;     // һ����������ܷ���File�ĸ���
const int MAX_UDP_PEER_LIST_COUNT = 66;     // һ����������ܷ���Peer�ĸ���
//const int MAX_HASHCODE_LENGTH = 20;
const int MAX_MD5_LENGTH = 30;              // MD5�ִ�����󳤶�
const int MAX_FILE_ID_LENGTH = 28;          // FileId�ִ�����󳤶�
const int MAX_URL_LENGTH = 260;             // URL�ִ�����󳤶�
const int MAX_IP_STRING_LENGTH = 15;        // �ִ����͵�IP��󳤶�
const int MAX_IP_RANGE_DESC_LENGTH = 260;   // IP����������󳤶�

#pragma pack(1)     // 1�ֽڶ���

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

typedef string HashCode;
//typedef string Md5Code;
//typedef int PeerId;
//typedef string FileId;

///////////////////////////////////////////////////////////////////////////////
// ������붨��

// ���ý������
const int RET_SUCCESS                   = 0;        // �ɹ�
const int RET_FAILURE                   = 1;        // ʧ��

const int RET_BASE                      = 100;      // �ǹ��ý��������� >= ��ֵ!


///////////////////////////////////////////////////////////////////////////////
// �����

// Э������(���ײ��ĵ�һ���ֽ�)
enum PacketProto {
    PP_UDP      = 1,        // TRACKERЭ�����ݰ�
    PP_UTP          = 2,        // UTP���ݰ�
};
enum { MAX_PACKET_PROTO = 2 };  // Э�������������

// �û������ӷ�ʽ
enum ConnectType {
    CT_UNKNOWN      = 0,        // ��������
    CT_DIRECT       = 1,        // ֱ��
    CT_SOCKS5       = 2,        // SOCKS5
    CT_LAN          = 3         // ������
};

// �û����ϻ���������������
enum NetType {
    NT_UNKNOWN      = 0         // �����ݿ����޷��ҵ����������
    // ������Ӫ�̷�������iprangetype.h
};

// Peer ����
enum PeerType {
    PT_UNKNOWN      = 0,        // δ֪�û�
    PT_PEER         = 1,        // ��ͨ�û�
    PT_SUPER_PEER   = 2         // CDN �ڵ�
};    

// �û���Ϣ
/*struct CPeerInfo
{
    uint nPeerId;               // PeerId
    uint nIp;                   // ����IP����Proxy��IP
    word nUdpPort;              // ����UDP Port����Proxy�����Port
    word nTcpPort;              // ����TCP Port
    uint nInternalIp;           // ����IP
    word nInternalUdpPort;      // ����UDP Port
    word nInternalTcpPort;      // ����TCP Port
    byte nConnectType;          // ������ʽ
    byte nPeerType;             // Peer ����(enum PeerType)
    
    CPeerInfo() { Clear(); }
    void Clear() { bzero(this, sizeof(*this)); }
};*/

// �ļ�֪ͨ������
enum NotifyFileState
{
    NFS_START_DOWNLOAD = 1,     // ����һ��������
    NFS_PAUSE_DOWNLOAD = 2,     // ��ͣ����
    NFS_RESUME_DOWNLOAD = 3,    // ��������
    NFS_COMPLETE_DOWNLOAD = 4   // �������
};

#pragma pack()


///////////////////////////////////////////////////////////////////////////////

#endif // _GLOBAL_H_ 
