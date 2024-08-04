#include "main.h"
#include "ui/ui.h"
#include "chat.h"
#include "keyboard.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/util.h"

uint32_t MAX_CHAT_MESSAGES = 40; // 8

extern UI *pUI;
extern KeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CGame * pGame;

std::unordered_map<std::string, CMDPROC> m_mapCmds;

void ChatInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	if(!pNetGame) return;

	if(*str == '/')
	{
		char *szCmdEndPos = (char*)str + 1;
		while(*szCmdEndPos && *szCmdEndPos != ' ') szCmdEndPos++;
		if(*szCmdEndPos == '\0') {
			std::unordered_map<std::string, CMDPROC>::iterator cmd = m_mapCmds.find(str + 1);
			if(cmd != m_mapCmds.end())
			{
				cmd->second("");
			}
			else
			{
				if(pNetGame)
				{
					pNetGame->SendChatCommand(str);
				}
			}
		} else {
			char szCopiedBuffer[256];
			strcpy(szCopiedBuffer, str);

			*szCmdEndPos = '\0';
			szCmdEndPos++;

			std::unordered_map<std::string, CMDPROC>::iterator cmd = m_mapCmds.find(str + 1);
			if(cmd != m_mapCmds.end())
			{
				cmd->second(szCmdEndPos);
			}
			else
			{
				if(pNetGame)
				{
					pNetGame->SendChatCommand(szCopiedBuffer);
				}
			}
		}
	}
	else
		pNetGame->SendChatMessage(str);
	return;
}


Chat::Chat()
{
	m_fChatPosX = pUI->scaleX( 325.0f );
	m_fChatPosY = pUI->scaleY( 25.0f );
	m_fChatSizeX = pUI->scaleX( 1150.0f );
	m_fChatSizeY = pUI->scaleY( 220.0f );
	m_iMaxMessages = 10; 

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;

	MAX_CHAT_MESSAGES = m_iMaxMessages*5;

	m_bIsOpened = false;
	m_iOffsetY = 0;
}

Chat::~Chat()
{
	m_mapCmds.clear();
}

void Chat::OnExitFromInput()
{ 
	m_bIsOpened = false;
}

bool Chat::onTouchEvent(int type, int x, int y)
{
	static bool bWannaOpenChat = false;

	float size = pUI->getFontSize()*m_iMaxMessages;

	switch(type)
	{
		case TOUCH_PUSH:
			if(x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
				bWannaOpenChat = true;
		break;

		case TOUCH_POP:
			if(bWannaOpenChat &&
				x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{
				m_bIsOpened = true;
				pKeyBoard->Open(&ChatInputHandler);
			}
			bWannaOpenChat = false;
		break;

		case TOUCH_MOVE:
		break;
	}

	return true;
}


void Chat::Render()
{
	float size = pUI->getFontSize()*m_iMaxMessages; 
	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY+size-pUI->getFontSize());

	for (std::list<CHAT_WINDOW_ENTRY>::iterator entry = m_ChatEntries.end(); entry != m_ChatEntries.begin(); entry--)
	{
		switch(entry->eType)
		{
			case CHAT_TYPE_CHAT:
				if(entry->szNick[0] != 0)
				{
					drawText(entry->szNick, pos.x, pos.y, entry->dwNickColor);
					pos.x += ImGui::CalcTextSize(entry->szNick).x + ImGui::CalcTextSize(" ").x; //+ pUI->getFontSize() * 0.4;
				}
				drawText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor);
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				drawText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor);
			break;
		}

		pos.x = m_fChatPosX;
		pos.y -= pUI->getFontSize();

	}
}

void Chat::drawText(const char* u8Str, float posX, float posY, uint32_t dwColor)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	ImVec4 col;

	while(*textCur)
	{
		if(textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			if(textCur != textStart)
			{
				pUI->drawText(posCur, colorCur, true, textStart, textCur);
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// �������� ����
			if(ProcessInlineHexColor(textCur + 1, textCur+7, col))
				colorCur = col;

			// ������� ��������
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if(textCur != textStart)
		pUI->drawText(posCur, colorCur, true, textStart, textCur);

	return;
}

void Chat::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void Chat::AddInfoMessage(const char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void Chat::AddDebugMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void Chat::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void Chat::PushBack(CHAT_WINDOW_ENTRY &entry)
{
	if(m_ChatEntries.size() >= MAX_CHAT_MESSAGES)
	{
		m_ChatEntries.pop_front();
	}

	m_ChatEntries.push_back(entry);
	return;
}

void Chat::AddToChatBuffer(eChatMessageType type, char* szString, char* szNick, 
	uint32_t dwTextColor, uint32_t dwNickColor)
{

	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else 
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// ������� ������ ������ � �����
		while(szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		// ���� ��������� ����� ������ 12 ��������
		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			// ������� �� MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			// ������� ����� MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			// ������� �� �������
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			// ������� ����� �������
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength + 1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	return;
}

void Chat::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}

void Chat::AddCmdProc(const char *cmdname, CMDPROC cmdproc)
{
	m_mapCmds.insert(std::make_pair(cmdname, cmdproc));
}