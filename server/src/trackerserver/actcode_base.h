///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: actcode_base.h
// ��������: �����������붨��
// ����޸�: 2006-01-05
///////////////////////////////////////////////////////////////////////////////

#ifndef _ACTCODE_BASE_H_
#define _ACTCODE_BASE_H_

///////////////////////////////////////////////////////////////////////////////
// �궨��

#define MAKE_ACTION_CODE(dir, pro, biz, opr)    ((dir << 24) | (pro << 22) | (biz << 16) | opr)

///////////////////////////////////////////////////////////////////////////////
// ����������س���
//
//   |<-- bit31                       bit0 -->|
//   0000 0000  00  000000  0000 0000 0000 0000
//   ---------  --  ------  -------------------
//      ͨ      Э    ҵ            ��
//      Ѷ      ��    ��            ��
//      ��      ��    ��            ��
//      ��      ��    ��            ��
//      ��      ��    ��            ��
//

// ���� -----------------------------------------------------------------------
const uint AC_DIR_MASK          = 0xFF000000;
const uint AC_PRO_MASK          = 0x00C00000;
const uint AC_BIZ_MASK          = 0x003F0000;
const uint AC_OPR_MASK          = 0x0000FFFF;

// ͨѶ������ -----------------------------------------------------------------
const byte AC_DIR_CS            = 1;        // �ͻ��� -> �����
const byte AC_DIR_SC            = 2;        // ����� -> �ͻ���
const byte AC_DIR_CC            = 3;        // �ͻ��� -> �ͻ���

// Э������� -----------------------------------------------------------------
const byte AC_PRO_NONE          = 0;        // δָ��(���ں���Э������)
const byte AC_PRO_UDP           = 1;        // UDP Э��
const byte AC_PRO_DTP           = 2;        // DTP Э��

///////////////////////////////////////////////////////////////////////////////
// �������� (UDPҵ��)

// UDPҵ�������
const byte AC_BIZ_UDP_NONE      = 0;        // δָ��(���ں���ҵ������)
const byte AC_BIZ_UDP_FINDER    = 1;        // ϵͳҵ��
const byte AC_BIZ_UDP_TRANSFER  = 2;        // �������

// �ͻ��� -> �����
const uint CS_NOOP              = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 101);   // �޲���
const uint CS_DEBUG             = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 102);   // ����
const uint CS_ACK               = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 103);   // Ӧ��
const uint CS_FW_PACKET         = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_UDP, AC_BIZ_UDP_NONE, 104);   // �ͻ������������ת�����ݰ�

// ����� -> �ͻ���
//const uint SC_REQUEST_FILE      = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 203);                             //��ͻ���������Դ
const uint SC_ACK               = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 201);   // Ӧ��
const uint SC_FW_PACKET         = MAKE_ACTION_CODE(AC_DIR_SC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 202);   // �ͻ������������ת�����ݰ�

// �ͻ��� -> �ͻ���
const uint CC_NOOP              = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 301);   // �޲���
const uint CC_ACK               = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 302);   // Ӧ��
const uint CC_INV_CONN_REQ      = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_UDP, AC_BIZ_UDP_NONE, 303);   // ������������

///////////////////////////////////////////////////////////////////////////////
// �������� (DTPҵ��)

// DTPҵ�������
const byte AC_BIZ_DTP_NONE              = 0;        // δָ��(���ں���ҵ������)
const byte AC_BIZ_DTP_GREET             = 1;        // DTP����
const byte AC_BIZ_DTP_NETMGR            = 2;        // �������

// �ͻ��� -> �ͻ���
const uint CC_INV_CONNECT       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_GREET, 101);   // ��������
const uint CC_BIT_TORRENT       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_GREET, 102);   // BT����
const uint CC_APP_SERVICE       = MAKE_ACTION_CODE(AC_DIR_CC, AC_PRO_DTP, AC_BIZ_DTP_NETMGR, 103);  // �ڵ���������Э��

// �ͻ��� -> �����
const uint CS_TRACKER_SERVICE   = MAKE_ACTION_CODE(AC_DIR_CS, AC_PRO_DTP, AC_BIZ_DTP_NETMGR, 104);  // Tracker�Ĺ���Э��

///////////////////////////////////////////////////////////////////////////////

#endif // _ACTCODE_BASE_H_
