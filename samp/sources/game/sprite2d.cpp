#include "../main.h"
#include "sprite2d.h"

CSprite2D::CSprite2D()
{
	((void (*)(CSprite2D *))(g_libGTASA + 0x5C8800 + 1))(this);  
}

CSprite2D::~CSprite2D()
{
	((void (*)(CSprite2D *))(g_libGTASA + 0x5C8806 + 1))(this);  
}

void CSprite2D::Draw(float x, float y, float width, float height, CRGBA const& color)
{
	((void (*)(CSprite2D *, float, float, float, float, CRGBA))(g_libGTASA + 0x5C8ED0 + 1))(this, x, y, width, height, color);  
}

void CSprite2D::Delete()
{
	((void (*)(CSprite2D *))(g_libGTASA + 0x5C881C + 1))(this);
}
	
