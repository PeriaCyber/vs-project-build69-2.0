#include "../main.h"
#include "ui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chat.h"
#include "../spawnscreen.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../extrakeyboard.h"


extern CExtraKeyBoard *pExtraKeyBoard;
extern Chat *pChat;
extern SpawnScreen *pSpawnScreen;
extern PlayerTags *pPlayerTags;
extern Dialog *pDialog;
extern KeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CGame *pGame;

void ReadSettings();

UI::UI()
{
	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;

	// font Size
	m_fFontSize = scaleY( 30.0f );

	// mouse/touch
	m_bMousePressed = false;
	m_vecMousePos = ImVec2(0, 0);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = scaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsClassic();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sfonts/arial_bold.ttf", GetStorage());
	// cp1251 ranges
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E5B, // Thai
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
        0x2010, 0x205E, // Punctuations
        0
	};
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
}

UI::~UI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

bool UI::Init()
{
	ReadSettings();

    pExtraKeyBoard = new CExtraKeyBoard();
	pKeyBoard = new KeyBoard();
	pChat = new Chat();
	pSpawnScreen = new SpawnScreen();
	pPlayerTags = new PlayerTags();
	pDialog = new Dialog();

	return true;
}

void UI::draw()
{
	if(pGame)
	{
		if(pGame->IsGameLoaded())
		{
			if(pGame->IsGamePaused())
				return;
		}
	}

	if(pNetGame)
	{
		CTextDrawPool *pTextDrawPool = pNetGame->GetTextDrawPool();
		if(pTextDrawPool)
			pTextDrawPool->Draw();
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	if(pExtraKeyBoard) 
	    pExtraKeyBoard->Render();

	if(pPlayerTags)
		pPlayerTags->Render();
	
	if(pChat)
		pChat->Render();

	if(pSpawnScreen)
		pSpawnScreen->Render();

	if(pDialog)
		pDialog->Render();

	if(pKeyBoard)
		pKeyBoard->Render();

	// AUTHOR
	ImVec2 textPos = ImVec2(RsGlobal->maximumWidth * 0.75, scaleY(5));
	drawText(textPos, ImColor(CLIENT_COLOR), true, CLIENT_AUTHOR);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	if(m_bNeedClearMousePos)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNeedClearMousePos = false;
	}
}

bool UI::onTouchEvent(int type, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	if(pGame)
	{
		if(pGame->IsGameLoaded())
		{
			if(pGame->IsGamePaused())
				return true;
		}
	}

	if(pNetGame) 
	{
		if(pNetGame->GetTextDrawPool())
			pNetGame->GetTextDrawPool()->onTouchEvent(type, x, y);
	}

	if(pKeyBoard)
	{
		if(!pKeyBoard->onTouchEvent(type, x, y)) 
			return false;
	}

	if(pChat)
	{
		if(!pChat->onTouchEvent(type, x, y))
			return false;
	}

	switch(type)
	{
		case TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		break;

		case TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		break;

		case TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		break;
	}

	return true;
}

void UI::drawText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = 2;
	if(bOutline)
	{
		// left
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(getFont(), getFontSize(), posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(getFont(), getFontSize(), posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(getFont(), getFontSize(), posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(getFont(), getFontSize(), posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}
	ImGui::GetBackgroundDrawList()->AddText(getFont(), getFontSize(), posCur, col, text_begin, text_end);
}