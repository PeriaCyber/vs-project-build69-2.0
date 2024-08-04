#pragma once

typedef struct _CAMERA_AIM
{
	VECTOR f1;
	VECTOR pos1;
	VECTOR pos2;
	VECTOR f2;
} CAMERA_AIM;

extern uint8_t *pbyteCameraMode;
extern uint16_t *wCameraMode2;
extern uint8_t *pbyteCurrentPlayer;

void GameAimSyncInit();
void GameResetLocalPlayerSkills();

CAMERA_AIM* GameGetInternalAim();
void GameSetLocalPlayerAim();
void GameStoreLocalPlayerAim();
void GameSetRemotePlayerAim(uint8_t bytePlayerID);
void GameStoreRemotePlayerAim(uint8_t bytePlayerID, CAMERA_AIM *pAim);

float GameGetAspectRatio();
float GameGetLocalPlayerCameraExtZoom();
void GameSetLocalPlayerCameraExtZoom();
void GameStoreLocalPlayerCameraExtZoom();
void GameSetRemotePlayerCameraExtZoom(uint8_t bytePlayerID);
void GameSetPlayerCameraExtZoom(uint8_t bytePlayerID, float fExtZoom, float fAspectRatio);

uint8_t GameGetLocalPlayerCameraMode();
void GameSetLocalPlayerCameraMode(uint8_t byteMode);
uint8_t GameGetPlayerCameraMode(uint8_t bytePlayerID);
void GameSetPlayerCameraMode(uint8_t byteMode, uint8_t bytePlayerNumber);

void GameUpdateLocalPlayerSkill(int iSkillType, uint16_t wSkillLevel);
void GameSetLocalPlayerSkills();
void GameStoreLocalPlayerSkills();
void GameSetRemotePlayerSkills(uint8_t bytePlayerNumber);
void GameStoreRemotePlayerSkills(uint8_t bytePlayerNumber, int iSkillType, uint16_t wSkillLevel);