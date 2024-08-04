#pragma once
#include "../game/textdraw.h"

class CTextDrawPool
{
public:
    CTextDrawPool();
    ~CTextDrawPool();

    CTextDraw *New(unsigned short wText, TEXT_DRAW_TRANSMIT *TextDrawTransmit, char *szText);
    void Delete(unsigned short wText);

    void SetSelectState(bool bState, uint32_t dwColor);
    bool onTouchEvent(int type, int x, int y);
    void SendClickTextDraw();

    void Draw();
    void SnapshotProcess();

    bool GetSlotState(unsigned short wText)
	{
		if(wText < 0 || wText >= MAX_TEXT_DRAWS) return false;
		return m_bTextDrawSlotState[wText];
	};

    CTextDraw *GetAt(unsigned short wText) 
    {
        if(!GetSlotState(wText)) return 0;
        return m_pTextDraw[wText];
    }

    bool GetState()
    {
        return m_bSelectState;
    }

private:
    CTextDraw*          m_pTextDraw[MAX_TEXT_DRAWS];
    bool                m_bTextDrawSlotState[MAX_TEXT_DRAWS];
    
    bool                m_bSelectState;
    uint32_t            m_dwHoverColor;
    uint16_t            m_wClickedTextDrawID;
};