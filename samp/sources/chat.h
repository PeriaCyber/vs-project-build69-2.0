#pragma once

#define MAX_MESSAGE_LENGTH	144
#define MAX_LINE_LENGTH		MAX_MESSAGE_LENGTH / 2

typedef void(*CMDPROC)(const char *params);

enum eChatMessageType
{
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT,
	CHAT_TYPE_INFO,
	CHAT_TYPE_DEBUG
};

enum eChatType
{
	CHAT_SELECT = 0,
	CHAT_RP = 1,
	CHAT_TEAM = 2
};

typedef struct
{
	eChatMessageType	eType;
	char 				utf8Message[MAX_MESSAGE_LENGTH * 3 + 1];
	char 				szNick[MAX_PLAYER_NAME + 1];
	uint32_t			dwTextColor;
	uint32_t 			dwNickColor;
} CHAT_WINDOW_ENTRY;

class Chat
{
	friend class UI;
public:
	Chat();
	~Chat();

	void OnExitFromInput();

	void AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage);
	void AddInfoMessage(const char* szFormat, ...);
	void AddDebugMessage(char* szFormat, ...);
	void AddClientMessage(uint32_t dwColor, char* szStr);

	void AddCmdProc(const char* cmdname, CMDPROC cmdproc);

protected:
	void Render();
	bool onTouchEvent(int type, int x, int y);

public:
	void drawText(const char* u8Str, float posX, float posY, uint32_t dwColor);
private:
	void FilterInvalidChars(char* szString);
	void AddToChatBuffer(eChatMessageType eType, char* szString, char* szNick, 
		uint32_t dwTextColor, uint32_t dwNickColor);
	void PushBack(CHAT_WINDOW_ENTRY &entry);

private:

	bool m_bIsOpened;
	
	float m_fChatPosX, m_fChatPosY;
	float m_fChatSizeX, m_fChatSizeY;

	int m_iMaxMessages;
	int m_iOffsetY;
	int m_iLastPosY;
	
	//std::vector
	std::list<CHAT_WINDOW_ENTRY> m_ChatEntries;

	uint32_t m_dwTextColor;
	uint32_t m_dwInfoColor;
	uint32_t m_dwDebugColor;
};