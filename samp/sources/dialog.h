#pragma once

#include <string>
#include "game/game.h"

#define DIALOG_STYLE_MSGBOX		0
#define DIALOG_STYLE_INPUT		1
#define DIALOG_STYLE_LIST		2
#define DIALOG_STYLE_PASSWORD	3
#define DIALOG_STYLE_TABLIST	4
#define DIALOG_STYLE_TABLIST_HEADERS	5

class Dialog
{
	friend class UI;
public:
	Dialog();
	~Dialog();
	
	void Clear();
	void Show(bool bShow);
	void SetInfo(char* szInfo, int length);
	
	bool IsDialogList();
	bool GetState() { return m_bIsActive; };
	
protected:
	void Render();
	
private:
	void DrawList();
	void DrawDefault();

public:
	bool		m_bIsActive;
	uint8_t 	m_byteDialogStyle;
	uint16_t	m_wDialogID;
	char		m_utf8Title[64*3 + 1];
	char*		m_putf8Info;
	char* 		m_pszInfo;
	char		m_utf8Button1[64*3 + 1];
	char		m_utf8Button2[64*3 + 1];
	
	uint8_t		m_iSelectedItem;
	uint8_t		m_iLastSelectedItem;
	std::string m_strSelectedItemText;
};