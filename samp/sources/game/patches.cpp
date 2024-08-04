#include "../main.h"
#include "../armhook/armhook.h"
#include "common.h"

char* PLAYERS_REALLOC = nullptr;

struct _ATOMIC_MODEL
{
    uintptr_t func_tbl;
    char data[56];
};
struct _ATOMIC_MODEL *ATOMIC_MODELS;

void ApplyRadarPatches()
{
    // CRadar::AddBlipToLegendList
    ARMHook::makeRET(g_libGTASA + 0x441A08);
    // CRadar::DrawLegend
    ARMHook::makeRET(g_libGTASA + 0x441B74);
}

void ApplyGlobalPatches()
{    
    PLAYERS_REALLOC = (( char* (*)(int))(g_libGTASA + 0x198A70))(404*1004);
    ARMHook::unprotect(g_libGTASA + 0x6783C8);
    *(char**)(g_libGTASA + 0x6783C8) = PLAYERS_REALLOC;
    
    ARMHook::writeMemory(g_libGTASA + 0x3F3648, (uintptr_t)"\x06\x20", 2); // CGame::InitialiseOnceBeforeRW
    
    ARMHook::makeNOP(g_libGTASA + 0x56C150, 4); // CBoat::ProcessControl
    
    ARMHook::makeNOP(g_libGTASA + 0x3FCD34, 2); // CRealTimeShadowManager::ReturnRealTimeShadow ~ CPhysical
    ARMHook::writeMemory(g_libGTASA + 0x408AE6, (uintptr_t)"\x4F\xF4\x7A\x61\x00", 4); // CPlaceable::InitMatrixArray
    
    ARMHook::unprotect(g_libGTASA + 0x41C33C); // CStats::GetFatAndMuscleModifier
    *(uint32_t *)(g_libGTASA + 0x41C33C) = 0x43300000;
    
    ARMHook::unprotect(g_libGTASA + 0x2BD94C); // CWidgetPlayerInfo::RenderArmorBar
    *(uint32_t *)(g_libGTASA + 0x2BD94C) = 0x43300000; 
    
    ARMHook::makeNOP(g_libGTASA + 0x3056D6, 4); // CEntryExit::GenerateAmbientPeds
    ARMHook::makeNOP(g_libGTASA + 0x43FE0A, 2); // CSprite2d::Draw
    ARMHook::makeNOP(g_libGTASA + 0x44095E, 2); // CSprite2d::Draw
    ARMHook::makeNOP(g_libGTASA + 0x43FE08, 3); // CSprite2d::Draw
    ARMHook::makeNOP(g_libGTASA + 0x44095C, 3); // CSprite2d::Draw
    
    // CStreaming::ms_memoryAvailable from ~CStreaming::Init2
    ARMHook::makeNOP(g_libGTASA + 0x46BE18, 1);
    
    // RwCameraEndUpdate
    // ARMHook::makeNOP(g_libGTASA + 0x3F6C8C, 2);
    
    ARMHook::makeNOP(g_libGTASA + 0x5D87A6, 2); // CCamera::CamShake
    ARMHook::makeNOP(g_libGTASA + 0x5D8734, 2); // CCamera::CamShake
    
    // --------------------------------------------------------------------------------------------------
    
    ATOMIC_MODELS = new _ATOMIC_MODEL[20000];
    for (int i = 0; i < 20000; i++)
    {
        // CBaseModelInfo::CBaseModelInfo
        ((void(*)(_ATOMIC_MODEL*))(g_libGTASA + 0x384F89))(&ATOMIC_MODELS[i]);
        ATOMIC_MODELS[i].func_tbl = g_libGTASA + 0x667454;
        memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
    }
    
    // CAudioEngine::StartLoadingTune
    ARMHook::makeNOP(g_libGTASA + 0x5E4916, 2);
    
    // Tasks
    ARMHook::makeRET(g_libGTASA + 0x3976AC);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
    ARMHook::makeRET(g_libGTASA + 0x4211A0);	// CPlaceName::Process
    ARMHook::makeRET(g_libGTASA + 0x538C8C);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
    ARMHook::makeRET(g_libGTASA + 0x50AA58);	// CCarEnterExit::SetPedInCarDirect

    // generator
    ARMHook::makeRET(g_libGTASA + 0x56E350);    // CTheCarGenerators::Process
    ARMHook::makeRET(g_libGTASA + 0x4CF26C);    // CPopulation::AddPed
    ARMHook::makeRET(g_libGTASA + 0x2E82CC);    // CCarCtrl::GenerateRandomCars
    ARMHook::makeRET(g_libGTASA + 0x579214);    // CPlane::DoPlaneGenerationAndRemoval
    ARMHook::makeRET(g_libGTASA + 0x46B548);    // CFileLoader::LoadPickup
    ARMHook::makeRET(g_libGTASA + 0x306EC0);    // CEntryExit::GenerateAmbientPeds

    // hud patch
    ARMHook::makeRET(g_libGTASA + 0x436FCC);     // CHud::SetHelpMessageStatUpdate
    ARMHook::makeRET(g_libGTASA + 0x438634);    // CHud::DrawVehicleName
    ARMHook::makeRET(g_libGTASA + 0x436F5C); // CHud::SetHelpMessage

    // shadow patch
    ARMHook::makeNOP(g_libGTASA + 0x3FCD34, 2);	     // ReturnRealTimeShadow
    ARMHook::makeNOP(g_libGTASA + 0x3FCD74, 2);	    // ReturnRealTimeShadow
    ARMHook::makeRET(g_libGTASA + 0x5B83FC);	 // CRealTimeShadowManager::Update

    // interior patch
    ARMHook::makeRET(g_libGTASA + 0x445E98); // Interior_c::AddPickups
    ARMHook::makeRET(g_libGTASA + 0x448984); // InteriorGroup_c::Exit
    ARMHook::makeRET(g_libGTASA + 0x40B028); // CPlayerInfo::FindObjectToSteal
    
    // player
    ARMHook::makeNOP(g_libGTASA + 0x40BED6, 2);    // CPlayerInfo::KillPlayer
    
    // CPed::RemoveWeaponWhenEnteringVehicle
    ARMHook::makeNOP(g_libGTASA + 0x4A5328, 6);		

    // DefaultPCSaveFileName
    ARMHook::unprotect(g_libGTASA + 0x6B0130);
    memcpy((void*)(g_libGTASA + 0x6B0130), "GTASAMP", 8);

    // CVehicleModelInfo::SetupCommonData ~ Increase matrix
    ARMHook::writeMemory(g_libGTASA + 0x468B7E, (uintptr_t)"\x4F\xF4\x00\x30", 4);
    ARMHook::writeMemory(g_libGTASA + 0x468B88, (uintptr_t)"\xF7\x20", 2);
    ARMHook::writeMemory(g_libGTASA + 0x468B8A, (uintptr_t)"\xF7\x25", 2);
    ARMHook::writeMemory(g_libGTASA + 0x468BCC, (uintptr_t)"\xF7\x28", 2);

    // CPlayerPed Task Patch
    ARMHook::writeMemory(g_libGTASA + 0x4C3673, (uintptr_t)"\xB3", 1);

    // CPlayerPed::ProcessAnimGroups
    ARMHook::makeNOP(g_libGTASA + 0x4C5EFA, 2);

    // CBike::ProcessAI
    ARMHook::makeNOP(g_libGTASA + 0x564CC0, 1); 

    // Vehicle Process Patch
    ARMHook::makeNOP(g_libGTASA + 0x553E26, 2);
    ARMHook::makeNOP(g_libGTASA + 0x561A52, 2);
    ARMHook::makeNOP(g_libGTASA + 0x56BE64, 2);
    ARMHook::makeNOP(g_libGTASA + 0x57D054, 2);      

    // FPS
    ARMHook::writeMemory(g_libGTASA + 0x5E4978, (uintptr_t)"\x3C", 1);
    ARMHook::writeMemory(g_libGTASA + 0x5E4990, (uintptr_t)"\x3C", 1);
    ARMHook::writeMemory(g_libGTASA + 0x9FC908, (uintptr_t)"\x3C", 1);
    ARMHook::writeMemory(g_libGTASA + 0x679118, (uintptr_t)"\x3C", 1);
    *(bool*)(g_libGTASA + 0x98F1AD) = true;

    // _rwOpenGLRasterCreate
    ARMHook::writeMemory(g_libGTASA + 0x1AE95E, (uintptr_t)"\x01\x22", 2);

    // exit social club
    uintptr_t libSCAnd = ARMHook::getLibraryAddress("libSCAnd.so");
    if(libSCAnd)
    {
        ARMHook::writeMemory(libSCAnd + 0x1E16DC, (uintptr_t)"com/rockstargames/hal/andViewManager", 37);
        ARMHook::writeMemory(libSCAnd + 0x1E1738, (uintptr_t)"staticExitSocialClub", 21);
        ARMHook::writeMemory(libSCAnd + 0x1E080C, (uintptr_t)"()V", 4);
    }
    
    ApplyRadarPatches();
}

void ApplySAMPPatchesInGame()
{    
    // CTheZones::ZonesVisited[100]
    memset((void*)(g_libGTASA + 0x98D252), 1, 100);
    
    // CTheZones::ZonesRevealed
    *(uint32_t*)(g_libGTASA + 0x98D2B8) = 100;
}