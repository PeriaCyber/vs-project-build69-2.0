#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../ui/ui.h"

extern UI *pUI;

RwTexture *splashTexture = nullptr;

void LoadSplashTexture()
{
	splashTexture = (RwTexture*)LoadTextureFromDB("samp", "mylogo");
}

void RenderSplash()
{
	stRect rect;
	stfRect uv;

	// background
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = RsGlobal->maximumWidth;
	rect.y2 = RsGlobal->maximumHeight;
	DrawRaster(&rect, 0xFF000000);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// texture
	rect.x1 = RsGlobal->maximumWidth * 0.05;
	rect.y1 = RsGlobal->maximumHeight * 0.25;
	rect.x2 = RsGlobal->maximumWidth - rect.x1;
	rect.y2 = RsGlobal->maximumHeight - rect.y1;
	uv.x1 = 0.0f;
	uv.y1 = 0.0f;
	uv.x2 = 1.0;
	uv.y2 = 1.0;
	DrawRaster(&rect, 0xFFFFFFFF, splashTexture->raster, &uv);
}

void RenderSplashScreen()
{
	RenderSplash();

	if(!pUI) return;

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	ImGui::GetOverlayDrawList()->AddText(ImVec2(10, 10), CLIENT_COLOR, CLIENT_AUTHOR);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}