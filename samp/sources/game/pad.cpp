#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../armhook/armhook.h"
#include "../chat.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern Chat* pChat;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurWeaponProcessingPlayer = 0;
uintptr_t dwCurPlayerActor = 0;
uint8_t byteInternalPlayer = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteSavedCameraMode;
uint16_t usSavedCameraMode2;
uint8_t byteSavedControlFlags;
uint8_t byteHornSaveControlFlags;
uint8_t byteSavedNightVision = 0;
uint8_t byteSavedThermalVision = 0;
float fSavedBikeLean;
float dwSavedBikeUnk;
MATRIX4X4 *matSavedMatrix;
VECTOR vecSavedMoveSpeed;

uint16_t (*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if((dwResult == 0xFF80 || dwResult == 0x80) && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK]) {
			dwResult = 0x40;
		}

		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		uint16_t usResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if((usResult == 0xFF80 || usResult == 0x80) &&  
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			usResult = 0x40;
		}

		return usResult;
	}
	else
	{
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t (*CPad__SprintJustDown)(uintptr_t thiz);
uint32_t CPad__SprintJustDown_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__SprintJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		uint32_t dwResult = CPad__DuckJustDown(thiz, unk);
		if(dwResult) {
			return 1;
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__DuckJustDown(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) 
		{
			return 2;
		}
			
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

uint16_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint16_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		uint16_t usResult = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = usResult;
		return usResult;
	}
}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurPlayer != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurPlayer != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurPlayer != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(*pbyteCurrentPlayer)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		uint16_t usResult = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = usResult;
		return usResult;
	}
}

uint32_t (*CPad__GetHorn)(uintptr_t thiz);
uint32_t CPad__GetHorn_hook(uintptr_t thiz)
{
	if(byteCurPlayer != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		// local player
		uint32_t horn = CPad__GetHorn(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
	dwCurPlayerActor = thiz;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(thiz);

	if(dwCurPlayerActor && (byteCurPlayer != 0) && byteInternalPlayer == 0)
	{
		// disable goggles temporarily for remote players.
		byteSavedNightVision = *(uint8_t*)(g_libGTASA + 0xA477EC);
		*(uint8_t*)(g_libGTASA + 0xA477EC) = 0;
		byteSavedThermalVision = *(uint8_t*)(g_libGTASA + 0xA477FC);
		*(uint8_t*)(g_libGTASA + 0xA477FC) = 0;

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		usSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if(*wCameraMode2 == 4) *wCameraMode2 = 0;

		// save the camera zoom factor, apply the context.
		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// weapon skills.
		GameStoreLocalPlayerSkills();
    	GameSetRemotePlayerSkills(byteCurPlayer);

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		ARMHook::makeNOP(g_libGTASA + 0x4A2A22, 2); 

		CPed__ProcessControl(thiz);

		// restore
    	ARMHook::writeMemory(g_libGTASA + 0x4A2A22, (uintptr_t)"\xF0\xF4\x42\xEB", 4); // +

		// restore weapon skills.
		GameSetLocalPlayerSkills();

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = usSavedCameraMode2;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoom();

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else
	{
		// restore state of these two "weapons"
		*(uint8_t*)(g_libGTASA + 0xA477EC) = byteSavedNightVision;
		*(uint8_t*)(g_libGTASA + 0xA477FC) = byteSavedThermalVision;

		// Apply the original code to set ped rot from Cam
		ARMHook::writeMemory(g_libGTASA + 0x539BA6, (uintptr_t)"\xC4\xF8\x60\x55", 4);
    	(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
    	ARMHook::makeNOP(g_libGTASA + 0x539BA6, 2);
	}
}

void ProcessControl(VEHICLE_TYPE* pVehicle, uintptr_t call_addr)
{
	uintptr_t pVehiclePtr = (uintptr_t)pVehicle;

	if(!pVehiclePtr) {
		return;
	}

	uintptr_t pDriver = *(uintptr_t *)(pVehiclePtr + 0x464);
	uintptr_t pTemp = *(uintptr_t *)(pVehiclePtr + 1124); 

	if(pVehicle && pDriver) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pDriver);
	}

	if( pDriver && *(uint32_t *)(pTemp + 1436) == 0 &&
		pDriver != (uintptr_t)GamePool_FindPlayerPed() && 
		*(uint8_t*)(g_libGTASA + 0x96B9C4) == 0) // 96B9C4 ; CWorld::PlayerInFocus
	{
		*(uint8_t*)(g_libGTASA + 0x96B9C4) = 0;

		*(uint32_t *)(pTemp + 1436) = 4;

		// CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_libGTASA + 0x3ACDB5))(pVehiclePtr + 0x13C); // 0x3ACDB5 thiz + 0x13C

		*(uint32_t *)(pTemp + 1436) = 0;
	} 
	else
	{
		// CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_libGTASA + 0x3ACDB5))(pVehiclePtr + 0x13C); // 0x3ACDB5 thiz + 0x13C
	}

	/* -------------------------- adjust address -------------------------- */
	// protection or dolboebizm? YES!

	// CAutomobile
    if(call_addr == 0x553DD5)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x553DD4 + 1))(pVehicle);
    }

    // CBoat
    if(call_addr == 0x56BE51)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x56BE50 + 1))(pVehicle);
    }

    // CBike
    if(call_addr == 0x561A21)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x561A20 + 1))(pVehicle);
    }

    // CPlane
    if(call_addr == 0x575C88)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x575C88 + 1))(pVehicle);
    }

    // CHeli
    if(call_addr == 0x571238)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x571238 + 1))(pVehicle);
    }

    // CBmx
    if(call_addr == 0x568B15)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x568B14 + 1))(pVehicle);
    }

    // CMonsterTruck
    if(call_addr == 0x5747F4)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x5747F4 + 1))(pVehicle);
    }

    // CQuadBike
    if(call_addr == 0x57A280)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x57A280 + 1))(pVehicle);
    }

    // CTrain
    if(call_addr == 0x57D030)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_libGTASA + 0x57D030 + 1))(pVehicle);
    }
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
		// CAutomobile
		case 0x66D688:
		call_addr = 0x553DD5; // +
		break;

		// CBoat
		case 0x66DA30:
		call_addr = 0x56BE51; // +
		break;

		// CBike
		case 0x66D800:
		call_addr = 0x561A21; // +
		break;

		// CPlane
		case 0x66DD94: 
		call_addr = 0x575C88; // +
		break;

		// CHeli
		case 0x66DB44: 
		call_addr = 0x571238; // +
		break;

		// CBmx
		case 0x66D918: // ?
		call_addr = 0x568B15; // +
		break;

		// CMonsterTruck
		case 0x66DC6C: 
		call_addr = 0x5747F4; // +
		break;

		// CQuadBike
		case 0x66DEBC:
		call_addr = 0x57A280; // +
		break;

		// CTrain
		case 0x66E10C:
		call_addr = 0x57D030; // +
		break;
	}

	ProcessControl(pVehicle, call_addr);
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	ARMHook::installPLTHook(g_libGTASA + 0x6692B4, (uintptr_t)CPed__ProcessControl_hook, (uintptr_t*)&CPed__ProcessControl);

	ARMHook::installMethodHook(g_libGTASA + 0x66D6B4, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66DA5C, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66D82C, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66DDC0, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66DB70, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66D944, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66DC98, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66DEE8, (uintptr_t)AllVehicles__ProcessControl_hook);
 	ARMHook::installMethodHook(g_libGTASA + 0x66E138, (uintptr_t)AllVehicles__ProcessControl_hook);

 	ARMHook::installPLTHook(g_libGTASA + 0x6706D0, (uintptr_t)CPad__GetPedWalkUpDown_hook, (uintptr_t*)&CPad__GetPedWalkUpDown);
 	ARMHook::installPLTHook(g_libGTASA + 0x671014, (uintptr_t)CPad__GetPedWalkLeftRight_hook, (uintptr_t*)&CPad__GetPedWalkLeftRight);
 	ARMHook::installPLTHook(g_libGTASA + 0x670CE0, (uintptr_t)CPad__GetSprint_hook, (uintptr_t*)&CPad__GetSprint);                                                            
 	ARMHook::installPLTHook(g_libGTASA + 0x670274, (uintptr_t)CPad__JumpJustDown_hook, (uintptr_t*)&CPad__JumpJustDown);                                                         
 	ARMHook::installPLTHook(g_libGTASA + 0x66FAE0, (uintptr_t)CPad__GetJump_hook, (uintptr_t*)&CPad__GetJump);                                                              
 	ARMHook::installPLTHook(g_libGTASA + 0x674A0C, (uintptr_t)CPad__GetAutoClimb_hook, (uintptr_t*)&CPad__GetAutoClimb);                                                         
 	ARMHook::installPLTHook(g_libGTASA + 0x672FD0, (uintptr_t)CPad__DiveJustDown_hook, (uintptr_t*)&CPad__DiveJustDown);                                                         
 	ARMHook::installPLTHook(g_libGTASA + 0x674030, (uintptr_t)CPad__SwimJumpJustDown_hook, (uintptr_t*)&CPad__SwimJumpJustDown);                                                     
 	ARMHook::installPLTHook(g_libGTASA + 0x6727CC, (uintptr_t)CPad__DuckJustDown_hook, (uintptr_t*)&CPad__DuckJustDown);                                                         
 	ARMHook::installPLTHook(g_libGTASA + 0x67127C, (uintptr_t)CPad__MeleeAttackJustDown_hook, (uintptr_t*)&CPad__MeleeAttackJustDown);                                                  
 	ARMHook::installPLTHook(g_libGTASA + 0x66FAD8, (uintptr_t)CPad__GetBlock_hook, (uintptr_t*)&CPad__GetBlock);                                                             
 	ARMHook::installPLTHook(g_libGTASA + 0x673D84, (uintptr_t)CPad__GetSteeringLeftRight_hook, (uintptr_t*)&CPad__GetSteeringLeftRight);                                                 
 	ARMHook::installPLTHook(g_libGTASA + 0x672C14, (uintptr_t)CPad__GetSteeringUpDown_hook, (uintptr_t*)&CPad__GetSteeringUpDown);                                                    
 	ARMHook::installPLTHook(g_libGTASA + 0x67482C, (uintptr_t)CPad__GetAccelerate_hook, (uintptr_t*)&CPad__GetAccelerate);                                                        
 	ARMHook::installPLTHook(g_libGTASA + 0x66EBE0, (uintptr_t)CPad__GetBrake_hook, (uintptr_t*)&CPad__GetBrake);                                                             
 	ARMHook::installPLTHook(g_libGTASA + 0x670514, (uintptr_t)CPad__GetHandBrake_hook, (uintptr_t*)&CPad__GetHandBrake);                                                         
 	ARMHook::installPLTHook(g_libGTASA + 0x673010, (uintptr_t)CPad__GetHorn_hook, (uintptr_t*)&CPad__GetHorn);                                                          
}