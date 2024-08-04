#pragma once

#include "../main.h"
#include "../game/RW/RenderWare.h"

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
std::string removeColorTags(std::string line);
ImVec2 CalcTextSizeWithoutTags(char* szStr);
ImVec2 CalcTextSizeWithoutTags(const float font_size, const char* szStr);
void drawText(const char* fmt, ...);
void drawText(ImVec2 pos, ImColor col, const char* szStr);
void drawText(ImFont *font, const float font_size, uint8_t outline, ImVec2 pos, ImColor col, const char* szStr);
void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor);
void ScrollWhenDraggingOnVoid();
void DrawRaster(stRect *rect, uint32_t color, RwRaster *raster = nullptr, stfRect *uv = nullptr);

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();