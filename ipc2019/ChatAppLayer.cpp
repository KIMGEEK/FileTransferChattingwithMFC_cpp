// 필요한 헤더 파일들을 포함합니다.
#include "stdafx.h"
#include "pch.h"
#include "ChatAppLayer.h"
#include <vector>
#include <algorithm>

// 디버그 모드에서 메모리 누수를 탐지하기 위한 매크로입니다.
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// ChatAppLayer 클래스의 생성자입니다. 부모 클래스 생성자를 호출하고 총 길이를 0으로 초기화합니다.
CChatAppLayer::CChatAppLayer(char* pName)
    : CBaseLayer(pName), m_totalLength(0)
{}

// ChatAppLayer 클래스의 소멸자입니다.
CChatAppLayer::~CChatAppLayer()
{}

// 패킷을 생성하고 하위 레이어로 전송하는 함수입니다.
void CChatAppLayer::createPacket(const unsigned char* payload, unsigned short length, unsigned char type, int seq) {
    // 패킷 길이를 계산합니다. 시작 또는 끝 패킷인 경우 1바이트를 추가합니다.
    unsigned short packetLength = (type == 0x00 || type == 0x03) ? length + 1 : length;

    // 패킷 헤더 정보를 설정합니다.
    m_chatPacket.capp_type = type;
    m_chatPacket.capp_totlen = packetLength;
    m_chatPacket.capp_sequence = static_cast<unsigned char>(seq);

    // 복사할 데이터의 크기를 결정합니다.
    size_t copySize = min(static_cast<size_t>(CHAT_DATA_MAX_SIZE), static_cast<size_t>(length));

    // 페이로드 데이터를 패킷에 복사합니다.
    std::copy(payload + (seq * CHAT_DATA_MAX_SIZE), payload + (seq * CHAT_DATA_MAX_SIZE) + copySize, m_chatPacket.capp_data);

    // 생성된 패킷을 하위 레이어(이더넷 레이어)로 전송합니다.
    static_cast<CEthernetLayer*>(GetUnderLayer())->Send(
        reinterpret_cast<unsigned char*>(&m_chatPacket),
        CHAT_HEADER_SIZE + copySize,
        0x2080
    );
}

// 상위 레이어로부터 받은 데이터를 전송하는 함수입니다.
BOOL CChatAppLayer::Send(unsigned char* payload, int length)
{
    // 데이터 길이가 최대 패킷 크기보다 작으면 단일 패킷으로 전송합니다.
    if (length < CHAT_DATA_MAX_SIZE) {
        createPacket(payload, length, 0x00, 0);
        return TRUE;
    }

    // 데이터 길이가 최대 패킷 크기보다 크면 여러 패킷으로 나누어 전송합니다.
    int remainingLength = length;
    int packetCount = 0;

    // 첫 번째 패킷을 전송합니다.
    createPacket(payload, length, 0x01, packetCount++);
    remainingLength -= CHAT_DATA_MAX_SIZE;

    // 중간 패킷들을 전송합니다.
    while (remainingLength > CHAT_DATA_MAX_SIZE) {
        createPacket(payload, CHAT_DATA_MAX_SIZE, 0x02, packetCount++);
        remainingLength -= CHAT_DATA_MAX_SIZE;
    }

    // 마지막 패킷을 전송합니다.
    createPacket(payload, remainingLength, 0x03, packetCount);
    return TRUE;
}

// 하위 레이어로부터 받은 데이터를 처리하는 함수입니다.
BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{
    // 받은 데이터를 CHAT_APP 구조체로 캐스팅합니다.
    LPCHAT_APP chatData = reinterpret_cast<LPCHAT_APP>(ppayload);

    // 단일 패킷 메시지인 경우
    if (chatData->capp_type == 0x00) {
        // 메시지를 CString으로 변환하고 상위 레이어로 전달합니다.
        CString msg(reinterpret_cast<char*>(chatData->capp_data), chatData->capp_totlen - 1);
        mp_aUpperLayer[0]->Receive(reinterpret_cast<unsigned char*>(msg.GetBuffer(0)));
        msg.ReleaseBuffer();
        return TRUE;
    }

    // 분할된 패킷의 첫 번째 패킷인 경우
    if (chatData->capp_type == 0x01) {
        // 기존의 분할 패킷 정보를 초기화하고 총 길이를 저장합니다.
        m_fragmentedPackets.clear();
        m_totalLength = chatData->capp_totlen;
    }

    // 분할된 패킷들을 처리합니다.
    if (chatData->capp_type == 0x01 || chatData->capp_type == 0x02 || chatData->capp_type == 0x03) {
        // 받은 패킷 정보를 저장합니다.
        m_fragmentedPackets.push_back(FragmentedPacket{
            chatData->capp_sequence,
            std::vector<unsigned char>(chatData->capp_data, chatData->capp_data +
                (chatData->capp_type == 0x03 ? chatData->capp_totlen : CHAT_DATA_MAX_SIZE))
            });

        // 마지막 패킷인 경우
        if (chatData->capp_type == 0x03) {
            // 패킷들을 시퀀스 번호 순으로 정렬합니다.
            std::sort(m_fragmentedPackets.begin(), m_fragmentedPackets.end(),
                [](const FragmentedPacket& a, const FragmentedPacket& b) { return a.sequence < b.sequence; });

            // 정렬된 패킷들을 하나의 메시지로 조립합니다.
            std::vector<char> assembledMessage;
            for (const auto& packet : m_fragmentedPackets) {
                assembledMessage.insert(assembledMessage.end(), packet.data.begin(), packet.data.end());
            }

            // 메시지 끝에 널 종료 문자를 추가합니다.
            assembledMessage.push_back('\0');

            // 조립된 메시지를 CString으로 변환합니다.
            CString msg(assembledMessage.data());

            // 변환된 메시지를 상위 레이어로 전달합니다.
            mp_aUpperLayer[0]->Receive(reinterpret_cast<unsigned char*>(msg.GetBuffer(0)));
            msg.ReleaseBuffer();

            // 분할 패킷 정보를 초기화합니다.
            m_fragmentedPackets.clear();
        }
        return TRUE;
    }

    // 알 수 없는 패킷 타입인 경우 FALSE를 반환합니다.
    return FALSE;
}