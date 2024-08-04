#include "main.h"
#include "spawnscreen.h"
#include "ui/ui.h"
#include "game/game.h"
#include "net/netgame.h"

extern CNetGame *pNetGame;

SpawnScreen::SpawnScreen()
{
	m_bEnabled = false;
}

void SpawnScreen::Render()
{
	if(!m_bEnabled) return;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool)
	{
		CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
		if(pLocalPlayer)
		{
			ImGuiIO &io = ImGui::GetIO();

			ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);

			ImGui::PushStyleVar(
				ImGuiStyleVar_WindowPadding,
			ImVec2(8, 8));

			ImGui::SetNextWindowBgAlpha(0.55f);
			ImGui::Begin("###spawnScreen", nullptr,
				ImGuiWindowFlags_NoTitleBar |
				//ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings);

			ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor(0xFFFFFFFF));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0xFF0A0000));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0xFF515151));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.20f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.70f);

			ImVec2 bSize = ImVec2(
				ImGui::CalcTextSize("QWERTYUI").x,
			ImGui::CalcTextSize("QW").x);

			if(ImGui::Button("<<", bSize))
				pLocalPlayer->SendPrevClass();

			ImGui::SameLine(0, 25);
			if(ImGui::Button(">>", bSize))
				pLocalPlayer->SendNextClass();

			ImGui::SameLine(0, 25);
			if(ImGui::Button("Spawn", bSize))
				pLocalPlayer->SendSpawn();

			ImGui::SetWindowSize(ImVec2(-1, -1));
			ImVec2 size = ImGui::GetWindowSize();
			ImGui::SetWindowPos(ImVec2(((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y * 0.95) - size.y)));
			ImGui::End();
			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar(3);
		}
	}
}