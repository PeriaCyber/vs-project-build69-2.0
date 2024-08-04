#include "../main.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "imguirender.h"
#include "ui.h"
#include "../chat.h"
#include <stdlib.h>
#include <string.h>

extern UI *pUI;
extern Chat *pChat;  

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end-start);
	if(hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if(sscanf(hex, "%x", &hexColor)	> 0)
		{
			color.x = static_cast< float >((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast< float >((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast< float >((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if(hexCount == 8)
				color.w = static_cast< float >((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

std::string removeColorTags(std::string line)
{
	std::string string;
	for(uint32_t it = 0; it < line.size(); it++)
	{
		if(it+7 < line.size())
		{
			if(line[it] == '{' && line[it+7] == '}')
			{
				it += 7;
				continue;
			}
		}
		string.push_back(line[it]);
	}
	return string;
}

ImVec2 CalcTextSizeWithoutTags(char* szStr)
{
	if(!szStr) return ImVec2(0, 0);

	char szNonColored[2048 + 1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;

	return ImGui::CalcTextSize(szNonColored);
}

ImVec2 CalcTextSizeWithoutTags(const float font_size, const char* szStr)
{
	if(!szStr) return ImVec2(0, 0);

	char szNonColored[2048];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos + 7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos + 7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	return ImGui::CalcTextSize(pUI->getFont(), font_size, szNonColored);
}

void drawText(ImVec2 pos, ImColor col, const char* szStr)
{
	if(!pUI) return;

	char tempStr[4096];

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				//ImGui::TextUnformatted(textStart, textCur);
				pUI->drawText(vecPos, col, true, textStart, textCur);
				//ImGui::SameLine(0.0f, 0.0f);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				//ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				//ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			//ImGui::TextUnformatted(textStart, textCur);
			pUI->drawText(vecPos, col, true, textStart, textCur);
			vecPos.x = pos.x;//+= ImGui::CalcTextSize(textStart, textCur).x;
			vecPos.y += pUI->getFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
	{
		pUI->drawText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
	{
		vecPos.y += pUI->getFontSize();
	}
}

void drawText(const char* fmt, ...)
{
	char tempStr[4096];

	va_list argPtr;
	va_start(argPtr, fmt);
	vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
	va_end(argPtr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				ImGui::TextUnformatted(textStart, textCur);
				ImGui::SameLine(0.0f, 0.0f);
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			ImGui::TextUnformatted(textStart, textCur);
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
		ImGui::TextUnformatted(textStart, textCur);
	else
		ImGui::NewLine();

	if(pushedColorStyle)
		ImGui::PopStyleColor();
}

void drawText(ImFont *font, const float font_size, uint8_t outline, ImVec2 pos, ImColor col, const char* szStr)
{
	char tempStr[4096];

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				pUI->drawText(vecPos, col, outline, textStart, textCur);
				vecPos.x += ImGui::CalcTextSize(pUI->getFont(), font_size, textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
				pushedColorStyle = false;

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			pUI->drawText(vecPos, col, outline, textStart, textCur);
			vecPos.x = pos.x;
			vecPos.y += font_size;
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
	{
		pUI->drawText(vecPos, col, outline, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(pUI->getFont(), font_size, textStart, textCur).x;
	}
	else
		vecPos.y += font_size;
}

void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor)
{
	uint16_t linesCount = 0;
	std::string strUtf8 = utf8string;
	int size = strUtf8.length();
	std::string color;

	for(uint32_t i = 0; i < size; i++)
	{
		if(i+7 < strUtf8.length())
		{
			if(strUtf8[i] == '{' && strUtf8[i+7] == '}' )
			{
				color = strUtf8.substr(i, 7 + 1);
			}
		}
		if(strUtf8[i] == '\n')
		{
			linesCount++;
			if(i + 1 < strUtf8.length() && !color.empty())
			{
				strUtf8.insert(i + 1 , color);
				size += color.length();
				color.clear();
			}
		}
		if(strUtf8[i] == '\t')
		{
			strUtf8.replace(i, 1, " ");
		}
	}
	pos.y += pUI->getFontSize()*(linesCount / 2);
	if(linesCount)
	{
		uint16_t curLine = 0;
		uint16_t curIt = 0;
		for(uint32_t i = 0; i < strUtf8.length(); i++)
		{
			if(strUtf8[i] == '\n')
			{
				if(strUtf8[curIt] == '\n' )
				{
					curIt++;
				}
				ImVec2 _pos = pos;
				_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, i-curIt).c_str()).x / 2;
				_pos.y -= ( pUI->getFontSize()*(linesCount - curLine) );
				drawText( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, i-curIt).c_str() );
				curIt = i;
				curLine++;
			}
		}
		if(strUtf8[curIt] == '\n')
		{
			curIt++;
		}
		if(strUtf8[curIt] != '\0')
		{
			ImVec2 _pos = pos;
			_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, strUtf8.size()-curIt).c_str()).x / 2;
			_pos.y -= ( pUI->getFontSize()*(linesCount - curLine) );
			drawText( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, strUtf8.length()-curIt).c_str() );
		}
	}
	else
	{
		pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.c_str()).x / 2;
		drawText( pos, __builtin_bswap32(dwColor), (char*)strUtf8.c_str() );
	}
}

void ScrollWhenDraggingOnVoid()
{
	ImGuiIO &io = ImGui::GetIO();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	if(io.MousePos.x >= windowPos.x && io.MousePos.x <= windowPos.x + windowSize.x &&
		io.MousePos.y >= windowPos.y && io.MousePos.y <= windowPos.y + windowSize.y)
	{
		ImVec2 mouse_delta = io.MouseDelta;
		if(mouse_delta.x != 0.0f && mouse_delta.x <= 100.0f) ImGui::SetScrollX(ImGui::GetScrollX() + -mouse_delta.x);
		if(mouse_delta.y != 0.0f && mouse_delta.y <= 100.0f) ImGui::SetScrollY(ImGui::GetScrollY() + -mouse_delta.y); 
	}
}

void DrawRaster(stRect *rect, uint32_t color, RwRaster *raster, stfRect *uv)
{
	static RwIm2DVertex vert[4];
	const RwReal nearScreenZ 	= *(RwReal*)(g_libGTASA + 0xA7C348);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip 	= *(RwReal*)(g_libGTASA + 0xA7C344);	// CSprite2d::RecipNearClip

	RwIm2DVertexSetScreenX(&vert[0], rect->x1);
	RwIm2DVertexSetScreenY(&vert[0], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	vert[0].emissiveColor = color;
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect->x2);
	RwIm2DVertexSetScreenY(&vert[1], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	vert[1].emissiveColor = color;
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect->x1);
	RwIm2DVertexSetScreenY(&vert[2], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	vert[2].emissiveColor = color;
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect->x2);
	RwIm2DVertexSetScreenY(&vert[3], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	vert[3].emissiveColor = color;
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}

/* imgui_impl_renderware.h */
RwRaster* g_FontRaster = nullptr;
RwIm2DVertex* g_pVB = nullptr; // vertex buffer
unsigned int g_VertexBufferSize = 5000;
ImVec4 g_ScissorRect;

void _SetScissorRect(void* pRect);
void _SetScissorRect(void* pRect) { return ((void(*)(void*))(g_libGTASA + 0x2B3E55))(pRect); }

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data)
{
	const RwReal nearScreenZ = 		*(RwReal*)(g_libGTASA + 0xA7C348);	// CSprite2d::NearScreenZ 009DAA60
	const RwReal recipNearClip = 	*(RwReal*)(g_libGTASA + 0xA7C344);	// CSprite2d::RecipNearClip 009DAA64

	if(!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if(g_pVB) { delete g_pVB; g_pVB = nullptr; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		g_pVB = new RwIm2DVertex[g_VertexBufferSize];
		if(!g_pVB)
		{
			return;
		}
	}

	RwIm2DVertex* vtx_dst = g_pVB;
	int vtx_offset = 0;

	for(int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_src = cmd_list->IdxBuffer.Data;

        for(int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
        	RwIm2DVertexSetScreenX(vtx_dst, vtx_src->pos.x);
        	RwIm2DVertexSetScreenY(vtx_dst, vtx_src->pos.y);
        	RwIm2DVertexSetScreenZ(vtx_dst, nearScreenZ);
        	RwIm2DVertexSetRecipCameraZ(vtx_dst, recipNearClip);
        	vtx_dst->emissiveColor = vtx_src->col;
        	RwIm2DVertexSetU(vtx_dst, vtx_src->uv.x, recipCameraZ);
        	RwIm2DVertexSetV(vtx_dst, vtx_src->uv.y, recipCameraZ);

        	vtx_dst++;
        	vtx_src++;
        }

        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
        	const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

        	if(pcmd->UserCallback)
        	{
        		pcmd->UserCallback(cmd_list, pcmd);
        	}
        	else
        	{
        		g_ScissorRect.x = pcmd->ClipRect.x;
        		g_ScissorRect.y = pcmd->ClipRect.w;
        		g_ScissorRect.z = pcmd->ClipRect.z;
        		g_ScissorRect.w = pcmd->ClipRect.y;
        		_SetScissorRect((void*)&g_ScissorRect);

        		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
  				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
  				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
  				RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
  				RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
  				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2);
  				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
  				RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
  				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pcmd->TextureId);
  				RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, 
  					&g_pVB[vtx_offset], (RwInt32)cmd_list->VtxBuffer.Size,
  					(RwImVertexIndex*)idx_buffer, pcmd->ElemCount);
  				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

  				g_ScissorRect.x = 0;
        		g_ScissorRect.y = 0;
        		g_ScissorRect.z = 0;
        		g_ScissorRect.w = 0;
        		_SetScissorRect((void*)&g_ScissorRect);
        	}

        	idx_buffer += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
	}

	return;
}

bool ImGui_ImplRenderWare_Init()
{
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)RsGlobal->maximumWidth, RsGlobal->maximumHeight);
	return true;
}

bool ImGui_ImplRenderWare_CreateDeviceObjects()
{
	// Build texture atlas
	ImGuiIO &io = ImGui::GetIO();
	unsigned char* pxs;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pxs, &width, &height, &bytes_per_pixel);

	RwImage *font_img = RwImageCreate(width, height, bytes_per_pixel*8);
	RwImageAllocatePixels(font_img);

	RwUInt8 *pixels = font_img->cpPixels;
	for(int y = 0; y < font_img->height; y++)
	{
		memcpy((unsigned char*)pixels, pxs + font_img->stride * y, font_img->stride);
		pixels += font_img->stride;
	}

	RwInt32 w, h, d, flags;
	RwImageFindRasterFormat(font_img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	g_FontRaster = RwRasterCreate(w, h, d, flags);
	g_FontRaster = RwRasterSetFromImage(g_FontRaster, font_img);
	RwImageDestroy(font_img);

	io.Fonts->TexID = (ImTextureID*)g_FontRaster;
	return true;
}

void ImGui_ImplRenderWare_ShutDown()
{
	ImGuiIO &io = ImGui::GetIO();

	// destroy raster
	RwRasterDestroy(g_FontRaster);
	g_FontRaster = nullptr;
	io.Fonts->TexID = nullptr;

	// destroy vertex buffer
	if(g_pVB) { delete g_pVB; g_pVB = nullptr; }
	return;
}

void ImGui_ImplRenderWare_NewFrame()
{
	if(!g_FontRaster)
		ImGui_ImplRenderWare_CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();

}