#pragma once

#if !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
#include <vector>

#define CHAT_DATA_MAX_SIZE 1496
#define CHAT_HEADER_SIZE 4

class CChatAppLayer : public CBaseLayer
{
private:
    unsigned short m_totalLength;

    struct FragmentedPacket {
        unsigned char sequence;
        std::vector<unsigned char> data;
    };

    std::vector<FragmentedPacket> m_fragmentedPackets;

    void createPacket(const unsigned char* payload, unsigned short length, unsigned char type, int seq);

public:
    BOOL Receive(unsigned char* ppayload);
    BOOL Send(unsigned char* ppayload, int nlength);

    CChatAppLayer(char* pName);
    virtual ~CChatAppLayer();

    typedef struct _CHAT_APP
    {
        unsigned short capp_totlen;
        unsigned char capp_type;
        unsigned char capp_sequence;
        unsigned char capp_data[CHAT_DATA_MAX_SIZE];
    } CHAT_APP, * LPCHAT_APP;

protected:
    CHAT_APP m_chatPacket;
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)