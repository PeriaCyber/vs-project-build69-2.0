#pragma once

#include "../vendor/imgui/imgui.h"
#include "imguirender.h"

#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

enum eTouchType
{
	TOUCH_POP = 1,
	TOUCH_PUSH = 2,
	TOUCH_MOVE = 3
};

class UI
{
public:
	UI();
	~UI();

	bool Init();
	void draw();

	float scaleX(float x) { return m_vecScale.x * x; }
	float scaleY(float y) { return m_vecScale.y * y; }
	ImFont* getFont() { return m_pFont; }
	float getFontSize() { return m_fFontSize; }

	bool onTouchEvent(int type, int x, int y);
	void drawText(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr);

private:
	ImFont* 	m_pFont;

	ImVec2		m_vecScale;
	float 		m_fFontSize;

	bool 		m_bMousePressed;
	ImVec2		m_vecMousePos;

	bool		m_bNeedClearMousePos;
};