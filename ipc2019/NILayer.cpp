// NILayer.cpp: implementation of the NILayer class.

#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// CNILayer 생성자
// CBaseLayer를 상속받고 pName을 부모 클래스에 전달
// pcap_findalldevs로 네트워크 장치 정보를 초기화
CNILayer::CNILayer(char* pName)
	: CBaseLayer(pName), device(nullptr) {
	// 모든 네트워크 장치 검색
	char errbuf[PCAP_ERRBUF_SIZE]; //오류 메시지 저장 버퍼
	if (pcap_findalldevs(&allDevices, errbuf) == -1) {
		fprintf(stderr, "장치 검색 실패: %s\n", errbuf);
		allDevices = nullptr; //장치 검색 실패 시 포인터 초기화
	}

	// OidData 메모리 할당 및 초기화
	OidData = static_cast<PPACKET_OID_DATA>(malloc(sizeof(PACKET_OID_DATA)));
	if (OidData == nullptr) {
		fprintf(stderr, "메모리 할당 실패\n");
		return; //메모리 할당 실패 시 함수 종료
	}
	OidData->Oid = OID_802_3_PERMANENT_ADDRESS; // 네트워크 어댑터 식별자
	OidData->Length = sizeof(macAddress); // MAC 주소 길이 설정
}

// CNILayer 소멸자
CNILayer::~CNILayer() {
	// 장치 리스트 메모리 해제
	pcap_if_t* currentDevice = allDevices;
	while (currentDevice) {
		pcap_if_t* nextDevice = currentDevice->next;
		free(currentDevice); // 현재 장치 메모리 해제
		currentDevice = nextDevice; // 다음 장치로 이동
	}
	free(OidData); // OidData 메모리 해제
}

// 패킷 수신 함수
// 수신한 패킷을 상위 레이어로 전달
BOOL CNILayer::Receive(unsigned char* pkt) {
	if (!pkt) return FALSE; // 유효성 검사
	//상위 레이어로 패킷 전달하고 결과 반환
	return mp_aUpperLayer[0]->Receive(pkt);
}

// 패킷 전송 함수
// 네트워크 어댑터로 패킷을 전송
BOOL CNILayer::Send(unsigned char* ppayload, int nlength) {
	//패킷 전송 시 오류 발생하면 메시지 박스 표시
	if (pcap_sendpacket(m_AdapterObject, ppayload, nlength) != 0) {
		AfxMessageBox(_T("패킷 전송 중 오류발생"));
		return FALSE; // 전송 실패 시 FALSE 반환
	}
	return TRUE; // 전송 성공 시 TRUE 반환
}

// 네트워크 어댑터 설정 함수
UCHAR* CNILayer::SetAdapter(int index) {
	//지정된 인덱스의 장치를 선택하여 어댑터 열기
	device = allDevices; // 장치 리스트의 첫 번째 장치를 가리킴
	for (int i = 0; i < index && device != nullptr; i++) {
		device = device->next; // 지정된 인덱스의 장치 선택
	}

	//네트워크 어댑터 열기
	char errbuf[PCAP_ERRBUF_SIZE]; //오류 메시지 저장
	m_AdapterObject = pcap_open_live(device->name, 65536, 0, 1000, errbuf);
	if (!m_AdapterObject) {
		AfxMessageBox(_T("어댑터 열기 실패: ") + CString(errbuf));
		return nullptr; // 어댑터 열기 실패 시 nullptr 반환
	}

	//MAC 주소 요청
	LPADAPTER adapter = PacketOpenAdapter(device->name);
	PacketRequest(adapter, FALSE, OidData); // MAC 주소 요청
	memcpy(macAddress, OidData->Data, sizeof(macAddress)); // MAC 주소 복사
	PacketCloseAdapter(adapter); // 어댑터 닫기

	return macAddress; // MAC 주소 반환
}

// 네트워크 어댑터 목록 가져오기
void CNILayer::GetMacAddressList(CStringArray& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(d->description); // 어댑터 설명 추가
	}
}

// 패킷 수신 스레드 함수
UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {
	auto pNI = static_cast<CNILayer*>(pParam); // CNILayer 포인터 캐스팅
	struct pcap_pkthdr* header;
	const u_char* pkt_data;

	while (pNI->canRead) {
		int result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data);
		if (result == 1) {
			//패킷을 수신했을 때 처리
			pNI->Receive((unsigned char*)pkt_data); // 패킷 처리
		}
	}
	return 0; // 스레드 종료
}

// 상태 전환 함수
void CNILayer::flip() {
	canRead = !canRead; // 상태 전환
}


















