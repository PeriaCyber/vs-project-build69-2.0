#include "main.h"
#include "ui/ui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "dialog.h"
#include "spawnscreen.h"
#include "extrakeyboard.h"

extern UI *pUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern Dialog *pDialog;
extern SpawnScreen *pSpawnScreen;
extern CTextDrawPool *pTextDrawPool;

CExtraKeyBoard::CExtraKeyBoard() 
{
m_bIsActive = false;
m_bIsExtraShow = false;

Log("Extrakeyboard initialized.");
}

CExtraKeyBoard::~CExtraKeyBoard() 
{ 

}

void CExtraKeyBoard::Show(bool bShow) 
{
m_bIsActive = bShow;
}

void CExtraKeyBoard::Render() 
{
if(!m_bIsActive || pSpawnScreen->m_bEnabled)
return;
	
ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.5f, 6.5f));
ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

bool bDontExpand = false;
if(pDialog->m_bIsActive)
bDontExpand = true;
bDontExpand = true;
CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
if(pPlayerPed) 
{
ImGuiIO &io = ImGui::GetIO();

ImGui::Begin("Extrakeyboard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

float m_fButWidth = ImGui::CalcTextSize("QWERTYUI").x;
float m_fButHeight = ImGui::CalcTextSize("QWER").x;
			
ImGui::SameLine(0, 5);
if(ImGui::Button(m_bIsExtraShow ? "<<" : ">>", ImVec2(m_fButWidth, m_fButHeight)))
m_bIsExtraShow = !m_bIsExtraShow;

if(m_bIsExtraShow) 
{
			

ImGui::SameLine(0, 15);
if(ImGui::Button("ALT", ImVec2(m_fButWidth, m_fButHeight)))
LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;

ImGui::SameLine(0, 15);
if(ImGui::Button("H", ImVec2(m_fButWidth, m_fButHeight)))
LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

ImGui::SameLine(0, 15);
if(ImGui::Button("F", ImVec2(m_fButWidth, m_fButHeight)))
LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;

ImGui::SameLine(0, 15);
if(ImGui::Button("Y", ImVec2(m_fButWidth, m_fButHeight))) 
LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;

ImGui::SameLine(0, 15);
if(ImGui::Button("N", ImVec2(m_fButWidth, m_fButHeight)))
LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;

ImGui::SameLine(0, 15);
if(ImGui::Button("2", ImVec2(m_fButWidth, m_fButHeight)))
{
    pNetGame->SendChatCommand("/engine");
}

// Button G 
CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
if(pVehiclePool) 
{
uint16_t sNearestVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
CVehicle *pVehicle = pVehiclePool->GetAt(sNearestVehicleID);
if(pVehicle)
{
if(pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f) 
{
CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
if(pPlayerPool) 
{
CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
if(pLocalPlayer)
{
if(!pLocalPlayer->IsSpectating()) 
{
if(!pPlayerPed->IsInVehicle())
{
ImGui::SameLine(0, 15);
if(ImGui::Button("G", ImVec2(m_fButWidth, m_fButHeight)))
{
pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
pLocalPlayer->SendEnterVehicleNotification(sNearestVehicleID, true);
}
}
}
}
}
}
}
}
}
ImGui::SetWindowSize(ImVec2(-1, -1));
ImVec2 size = ImGui::GetWindowSize();
ImGui::SetWindowPos(ImVec2(pUI->scaleX(10), pUI->scaleY(365)));
ImGui::End();
ImGui::PopStyleVar(2);
}
return;
}
