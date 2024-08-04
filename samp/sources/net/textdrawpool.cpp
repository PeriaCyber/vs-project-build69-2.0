#include "../main.h"
#include "../ui/ui.h"
#include "../game/game.h"
#include "textdrawpool.h"
#include "../keyboard.h"
#include "netgame.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern KeyBoard *pKeyBoard;

CTextDrawPool::CTextDrawPool()
{
    for(int i = 0; i < MAX_TEXT_DRAWS; i++)
    {
        m_pTextDraw[i] = NULL;
        m_bTextDrawSlotState[i] = false;
    }

    m_bSelectState = false;
	m_dwHoverColor = 0;
	m_wClickedTextDrawID = -1;
}

CTextDrawPool::~CTextDrawPool()
{
    for(int i = 0; i < MAX_TEXT_DRAWS; i++)
        Delete(i);

    m_bSelectState = false;
	m_dwHoverColor = 0;
	m_wClickedTextDrawID = -1;
}

CTextDraw *CTextDrawPool::New(unsigned short wText, TEXT_DRAW_TRANSMIT *TextDrawTransmit, char *szText)
{
    if(wText < 0 || wText >= MAX_TEXT_DRAWS)
        return 0;

	Delete(wText);

    m_pTextDraw[wText] = new CTextDraw(TextDrawTransmit, szText);
    if(m_pTextDraw[wText])
    {
        m_bTextDrawSlotState[wText] = true;
        return m_pTextDraw[wText];
    }

    return 0;
}

void CTextDrawPool::Delete(unsigned short wText)
{
    if(!m_pTextDraw[wText] && !GetSlotState(wText)) 
        return;

    SAFE_DELETE(m_pTextDraw[wText]);

   m_bTextDrawSlotState[wText] = false;
}

void CTextDrawPool::SetSelectState(bool bState, uint32_t dwColor)
{
	if(bState) 
    {
		m_bSelectState = true;
		m_dwHoverColor = dwColor;
        m_wClickedTextDrawID = -1;
		pGame->DisplayHUD(false);
		pGame->FindPlayerPed()->TogglePlayerControllable(false);
	}
	else 
    {
		m_bSelectState = false;
		m_dwHoverColor = 0;
		m_wClickedTextDrawID = -1;
		pGame->DisplayHUD(true);
		pGame->FindPlayerPed()->TogglePlayerControllable(true);

		SendClickTextDraw();

		for(int i = 0; i < MAX_TEXT_DRAWS; i++)
		{
			if(m_pTextDraw[i] && GetSlotState(i)) 
            {
				m_pTextDraw[i]->m_bHovered = false;
				m_pTextDraw[i]->m_dwHoverColor = 0;
			}
		}
	}
}

bool CTextDrawPool::onTouchEvent(int type, int x, int y)
{
	if(!GetState()) 
        return true;

    if(pKeyBoard && pKeyBoard->IsOpen()) 
        return true;

    m_wClickedTextDrawID = -1;
    for(int i = 0; i < MAX_TEXT_DRAWS; i++)
    {
        if(m_pTextDraw[i] && GetSlotState(i)) 
        {
            CTextDraw* pTextDraw = m_pTextDraw[i];
            pTextDraw->m_bHovered = false;
            pTextDraw->m_dwHoverColor = 0;
 
            if(pTextDraw->m_TextDrawData.byteSelectable)
            {
                if(IsPointInRect(x, y, &pTextDraw->m_rectArea))
                {
                    switch(type)
                    {
                        case TOUCH_PUSH:
                        case TOUCH_MOVE:
                        m_wClickedTextDrawID = -1;
                        pTextDraw->m_bHovered = true;
                        pTextDraw->m_dwHoverColor = m_dwHoverColor;
                        break;
 
                        case TOUCH_POP:
                        m_wClickedTextDrawID = i;
                        SendClickTextDraw();
                        pTextDraw->m_bHovered = false;
                        pTextDraw->m_dwHoverColor = 0;
                        break;
                    }
                }
            }
        }
    }

	return false;
}

void CTextDrawPool::SendClickTextDraw()
{
	RakNet::BitStream bsSend;
	bsSend.Write(m_wClickedTextDrawID);
	pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

void CTextDrawPool::Draw()
{
    for(int i = 0; i < MAX_TEXT_DRAWS; i++)
    {
        if(m_pTextDraw[i] && GetSlotState(i))
            m_pTextDraw[i]->Draw();
    }
}

void CTextDrawPool::SnapshotProcess()
{
	for(int i = 0; i < MAX_TEXT_DRAWS; i++)
	{
		//if(m_pTextDraw[i] && GetSlotState(i))
			//m_pTextDraw[i]->SnapshotProcess();
	}
}