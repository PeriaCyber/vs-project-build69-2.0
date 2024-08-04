#include "../main.h"
#include "game.h"

CAMERA_AIM * pcaInternalAim = nullptr;
uint8_t *pbyteCameraMode = nullptr;
uint16_t *wCameraMode2 = nullptr;
float *pfCameraExtZoom = nullptr;
float *pfAspectRatio = nullptr;
uint8_t *pbyteCurrentPlayer = nullptr;
float *pPlayerStats = nullptr;

CAMERA_AIM caLocalPlayerAim;
CAMERA_AIM caRemotePlayerAim[MAX_PLAYERS];

uint8_t byteCameraMode[MAX_PLAYERS];

float fCameraExtZoom[MAX_PLAYERS];
float fLocalCameraExtZoom;

float fCameraAspectRatio[MAX_PLAYERS];
float fLocalAspectRatio;

float fRemoteSkillLevel[MAX_PLAYERS][11];
float fLocalSkillLevel[11];

void GameAimSyncInit()
{
	memset(&caLocalPlayerAim, 0, sizeof(CAMERA_AIM));
	memset(caRemotePlayerAim, 0, sizeof(CAMERA_AIM) * MAX_PLAYERS);
	memset(byteCameraMode, 4, MAX_PLAYERS);

	for(int i = 0; i < MAX_PLAYERS; i++) 
	{
		fCameraExtZoom[i] = 1.0f;
		fCameraAspectRatio[i] = 0.333333f;

		for(int x = 0; x < ARRAY_SIZE(fLocalSkillLevel); x++)
			fRemoteSkillLevel[i][x] = 999.0f;
	}

	pcaInternalAim = (CAMERA_AIM*)(g_libGTASA + 0x952280);
	pbyteCameraMode = (uint8_t*)(g_libGTASA + 0x952126);
	wCameraMode2 = (uint16_t*)(g_libGTASA + 0x95275C);
	pfAspectRatio = (float*)(g_libGTASA + 0xA26A90);
	pfCameraExtZoom = (float*)(g_libGTASA + 0x9521A4);
	pbyteCurrentPlayer = (uint8_t*)(g_libGTASA + 0x96B9C4);
	pPlayerStats = (float*)(g_libGTASA + 0x9647E4);
}

void GameResetLocalPlayerSkills()
{
	for(int i = 0; i < ARRAY_SIZE(fLocalSkillLevel); i++)
		fLocalSkillLevel[i] = 999.0f;

	GameSetLocalPlayerSkills();
}

CAMERA_AIM* GameGetInternalAim()
{
	return pcaInternalAim;
}

void GameSetLocalPlayerAim()
{
	memcpy(pcaInternalAim, &caLocalPlayerAim, sizeof(CAMERA_AIM));
}

void GameStoreLocalPlayerAim()
{
	memcpy(&caLocalPlayerAim, pcaInternalAim, sizeof(CAMERA_AIM));
}

void GameSetRemotePlayerAim(uint8_t bytePlayerID)
{
	memcpy(pcaInternalAim, &caRemotePlayerAim[bytePlayerID], sizeof(CAMERA_AIM));
}

void GameStoreRemotePlayerAim(uint8_t bytePlayerNumber, CAMERA_AIM* pAim)
{
	memcpy(&caRemotePlayerAim[bytePlayerNumber], pAim, sizeof(CAMERA_AIM));
}

float GameGetAspectRatio()
{
	return *pfAspectRatio;
}

float GameGetLocalPlayerCameraExtZoom()
{
	return (*pfCameraExtZoom - 35.0f) / 35.0f;	// normalize for 35.0 to 70.0
}

void GameSetLocalPlayerCameraExtZoom()
{
	*pfCameraExtZoom = fLocalCameraExtZoom;
	*pfAspectRatio = fLocalAspectRatio;
}

void GameStoreLocalPlayerCameraExtZoom()
{
	fLocalCameraExtZoom = *pfCameraExtZoom;
	fLocalAspectRatio = *pfAspectRatio;
}

void GameSetRemotePlayerCameraExtZoom(uint8_t bytePlayerID)
{
	*pfCameraExtZoom = fCameraExtZoom[bytePlayerID] * 35.0f + 35.0f; // unnormalize for 35.0 to 70.0
	*pfAspectRatio = fCameraAspectRatio[bytePlayerID];
}

void GameSetPlayerCameraExtZoom(uint8_t bytePlayerID, float fExtZoom, float fAspectRatio)
{
	fCameraExtZoom[bytePlayerID] = fExtZoom;
	fCameraAspectRatio[bytePlayerID] = fAspectRatio;
}

uint8_t GameGetLocalPlayerCameraMode()
{
	return *pbyteCameraMode;
}

void GameSetLocalPlayerCameraMode(uint8_t byteMode)
{
	*pbyteCameraMode = byteMode;
}

uint8_t GameGetPlayerCameraMode(uint8_t bytePlayerID)
{
	return byteCameraMode[bytePlayerID];
}

void GameSetPlayerCameraMode(uint8_t byteMode, uint8_t bytePlayerNumber)
{
	byteCameraMode[bytePlayerNumber] = byteMode;
}

void GameUpdateLocalPlayerSkill(int iSkillType, uint16_t wSkillLevel)
{
	if(iSkillType < ARRAY_SIZE(fLocalSkillLevel))
		pPlayerStats[69 + iSkillType] = (float)wSkillLevel;
}

void GameSetLocalPlayerSkills()
{
	for(int i = 0; i < ARRAY_SIZE(fLocalSkillLevel); i++)
		pPlayerStats[69 + i] = fLocalSkillLevel[i];
}

void GameStoreLocalPlayerSkills()
{
	for(int i = 0; i < ARRAY_SIZE(fLocalSkillLevel); i++)
		fLocalSkillLevel[i] = pPlayerStats[69 + i];
}

void GameSetRemotePlayerSkills(uint8_t bytePlayerNumber)
{
	for(int i = 0; i < ARRAY_SIZE(fLocalSkillLevel); i++)
		pPlayerStats[69 + i] = fRemoteSkillLevel[bytePlayerNumber][i];
}

void GameStoreRemotePlayerSkills(uint8_t bytePlayerNumber, int iSkillType, uint16_t wSkillLevel)
{
	if(iSkillType < ARRAY_SIZE(fLocalSkillLevel))
		fRemoteSkillLevel[bytePlayerNumber][iSkillType] = (float)wSkillLevel;
}