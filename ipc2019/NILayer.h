#pragma once
// NILayer.h: interface for the CNILayer class.

#if !defined(AFX_NILayer_H)
#define AFX_NILayer_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include <tchar.h>
#include <Packet32.h>
#include <WinSock2.h>
#include <thread>
#include <cstring> // for memset, memcpy

#pragma comment(lib, "packet.lib")
#pragma comment(lib, "ws2_32.lib")

class CNILayer : public CBaseLayer { //Thread ����
    pcap_if_t* allDevices = nullptr; // ��Ʈ��ũ ��ġ ��� (nullptr�� �ʱ�ȭ)
    pcap_if_t* device = nullptr;     // ���õ� ��ġ (nullptr�� �ʱ�ȭ)
    pcap_t* m_AdapterObject = nullptr; // ��Ʈ��ũ ����� ��ü
    UCHAR data[1500] = { 0 };         // ������ ����, 0���� �ʱ�ȭ
    char errbuf[PCAP_ERRBUF_SIZE] = { 0 }; // pcap ���� ����
    UCHAR macAddress[6] = { 0 };      // MAC �ּ�, 0���� �ʱ�ȭ
    LPADAPTER adapter = nullptr;    // ��Ʈ��ũ ����� (nullptr�� �ʱ�ȭ)
    PPACKET_OID_DATA OidData = nullptr; // OID ������ (nullptr�� �ʱ�ȭ)
    bool canRead = false;           // �б� �÷���

public:
    CNILayer(char* pName); // ������: pcap_findalldevs�� ����� ���� ����
    ~CNILayer();           // �Ҹ���

    BOOL Receive(unsigned char* pkt); // ��Ŷ�� �޾Ƽ� ���� layer�� ����
    BOOL Send(unsigned char* ppayload, int nlength); // ��Ŷ ����
    void GetMacAddressList(CStringArray& adapterlist); // ����� ��� ��ȯ
    UCHAR* SetAdapter(const int index); // ����� ���� �� MAC �ּ� ��ȯ

    static UINT ThreadFunction_RECEIVE(LPVOID pParam); // ���� ������ �Լ�
    static UINT ThreadFunction_SEND(LPVOID pParam);    // ���� ������ �Լ�
    void flip(); // byte ���� ���� �Լ� (�ʿ�� little endian <-> big endian ��ȯ)
};

#endif // !defined(AFX_NILayer_H)