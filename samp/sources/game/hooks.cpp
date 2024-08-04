#include "../main.h"
#include "../armhook/armhook.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../net/netgame.h"
#include "../ui/ui.h"
#include "../chat.h"

extern CNetGame *pNetGame;
extern UI *pUI;
extern Chat *pChat;
extern CGame *pGame;

extern bool bGameInited;

void MainLoop();
void HookCPad();

typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[315];
int PedsModelsCount = 0;

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
extern struct _ATOMIC_MODEL* ATOMIC_MODELS;

extern "C" uintptr_t get_lib() 
{
 	return g_libGTASA;
}

// ----------------------------------------------------------------------------------------------------

bool g_bPlaySAMP = false;
void (*MainMenuScreen__Update)(uintptr_t thiz, float unk);
void MainMenuScreen__Update_hook(uintptr_t thiz, float unk)
{
	if(!g_bPlaySAMP)
	{
		// StartGameScreen::OnNewGameCheck(void)
		((void (*)())(g_libGTASA + 0x2A7200 + 1))();

		g_bPlaySAMP = true;
		return;
	}
	return MainMenuScreen__Update(thiz, unk);
}

void (*Render2dStuff)();
void Render2dStuff_hook()
{
	Render2dStuff();
	MainLoop();
}

void RenderSplashScreen();
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
	if(!g_bPlaySAMP) return;

	RwCamera* camera = *(RwCamera**)(g_libGTASA + 0x9FC93C);
	if(RwCameraBeginUpdate(camera))
	{
		DefinedState2d();
		(( void (*)())(g_libGTASA + 0x5C89A8 + 1))(); // CSprite2d::InitPerFrame()
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		(( void (*)(bool))(g_libGTASA + 0x1C07D0 + 1))(false); // emu_GammaSet()
		RenderSplashScreen();
		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}
}

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	Render2dStuffAfterFade();
}

void (*MobileMenu__Render)(uintptr_t);
void MobileMenu__Render_hook(uintptr_t thiz)
{
	MobileMenu__Render(thiz);

	uint32_t col = ImColor(CLIENT_COLOR);
	CFont::SetColor(&col);
	CFont::SetFontStyle(1);
	CFont::SetScale(1.0, 2.0);
	CFont::SetCentreSize(0);
	if(pUI) CFont::PrintString(pUI->scaleX(5), pUI->scaleY(5), CLIENT_AUTHOR);
}

struct stFile
{
	int isFileExist;
	FILE *f;
};

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)
{
	char path[0xFF] = { 0 };

	// ----------------------------
	if(!strncmp(r1+12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", GetStorage());
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", GetStorage());
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%s/SAMP/peds.ide", GetStorage());
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%s/SAMP/gta.dat", GetStorage());
		goto open;
	}

orig:
	return NvFOpen(r0, r1, r2, r3);

open:
	stFile *st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE *f  = fopen(path, "rb");
	if(f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		st = nullptr;
		return 0;
	}
}

// bydlocode
/*void (*CFileMgr__Initialise)(uintptr_t thiz);
void CFileMgr__Initialise_hook(uintptr_t thiz)
{
	uintptr_t zipStorage = ((uintptr_t(*)(const char*))(g_libGTASA + 0x26FE55))("/Android/data/com.rockstargames.gtasa/samp.obb");

	if(zipStorage) {
		((int (*)(uintptr_t))(g_libGTASA + 0x26FF71))(zipStorage);
	}
   
	return CFileMgr__Initialise(thiz);
}*/

void (*CStream__InitImageList)();
void CStream__InitImageList_hook()
{
	// fuck
	ARMHook::unprotect(g_libGTASA + 0x792DA8);
	ARMHook::unprotect(g_libGTASA + 0x792DA4);
	ARMHook::unprotect(g_libGTASA + 0x792DD4);
	ARMHook::unprotect(g_libGTASA + 0x792DD8);
	ARMHook::unprotect(g_libGTASA + 0x792E04);
	ARMHook::unprotect(g_libGTASA + 0x792E08);
	ARMHook::unprotect(g_libGTASA + 0x792E34);
	ARMHook::unprotect(g_libGTASA + 0x792E38);
	ARMHook::unprotect(g_libGTASA + 0x792E64);
	ARMHook::unprotect(g_libGTASA + 0x792E68);
	ARMHook::unprotect(g_libGTASA + 0x792E94);
	ARMHook::unprotect(g_libGTASA + 0x792E98);
	ARMHook::unprotect(g_libGTASA + 0x792EC4);
	ARMHook::unprotect(g_libGTASA + 0x792EC8);
	ARMHook::unprotect(g_libGTASA + 0x792EF4);
	ARMHook::unprotect(g_libGTASA + 0x792D78);

  	*(uint8_t *)(g_libGTASA + 0x792DA8) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792DA4) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792DD4) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792DD8) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792E04) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792E08) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792E34) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792E38) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792E64) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792E68) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792E94) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792E98) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792EC4) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792EC8) = 0;
  	*(uint32_t *)(g_libGTASA + 0x792EF4) = 0;
  	*(uint8_t *)(g_libGTASA + 0x792D78) = 0;

	// CStreaming::AddImageToList
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x2CF761))("TEXDB\\GTA3.IMG", 1);
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x2CF761))("TEXDB\\GTA_INT.IMG", 1);
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x2CF761))("TEXDB\\SAMP.IMG", 1);
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x2CF761))("TEXDB\\SAMPCOL.IMG", 1);
}

void (*CGame__InitialiseRenderWare)();
void CGame__InitialiseRenderWare_hook()
{
	CGame__InitialiseRenderWare();

	((void (*)(const char*, int, int))(g_libGTASA + 0x1EA8E5))("samp", 0, 5);

	pUI = new UI();
}

PED_MODEL* (*CModelInfo__AddPedModel)(int id);
PED_MODEL* CModelInfo__AddPedModel_hook(int id)
{
	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));
	model->vtable = (uintptr_t)(g_libGTASA + 0x667668);

	// CClumpModelInfo::CClumpModelInit()
	((uintptr_t(*)(PED_MODEL*))(*(void**)(model->vtable + 0x1C)))(model);

	*(PED_MODEL**)(g_libGTASA + 0x91DCB8 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}

uintptr_t (*CModelInfo__AddAtomicModel)(int iModel);
uintptr_t CModelInfo__AddAtomicModel_hook(int iModel)
{
	uint32_t iCount = *(uint32_t*)(g_libGTASA + 0x820738);
	_ATOMIC_MODEL* model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_libGTASA + 0x820738) = iCount + 1;

	// CClumpModelInfo::CClumpModelInit()
	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);

	_ATOMIC_MODEL** ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_libGTASA + 0x91DCB8); // CModelInfo::ms_modelInfoPtrs
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;
}

void (*CPools__Initialise)();
void CPools__Initialise_hook()
{
	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + 0x95AC38) = ms_pPtrNodeSingleLinkPool; // ms_pPtrNodeSingleLinkPool
	uintptr_t g_pPool = g_libGTASA + 0x95AC38;

	*(PoolAllocator::Pool**)(g_pPool + 4) = ms_pPtrNodeDoubleLinkPool; // ms_pPtrNodeDoubleLinkPool
	*(PoolAllocator::Pool**)(g_pPool + 8) = ms_pEntryInfoNodePool; // ms_pEntryInfoNodePool
	*(PoolAllocator::Pool**)(g_pPool + 12) = ms_pPedPool; // ms_pPedPool
	*(PoolAllocator::Pool**)(g_pPool + 16) = ms_pVehiclePool; // ms_pVehiclePool
	*(PoolAllocator::Pool**)(g_pPool + 20) = ms_pBuildingPool; // ms_pBuildingPool
	*(PoolAllocator::Pool**)(g_pPool + 24) = ms_pObjectPool; // ms_pObjectPool
	*(PoolAllocator::Pool**)(g_pPool + 28) = ms_pDummyPool; // ms_pDummyPool
	*(PoolAllocator::Pool**)(g_pPool + 32) = ms_pColModelPool; // ms_pColModelPool
	*(PoolAllocator::Pool**)(g_pPool + 36) = ms_pTaskPool; // ms_pTaskPool
	*(PoolAllocator::Pool**)(g_pPool + 40) = ms_pEventPool; // ms_pEventPool
	*(PoolAllocator::Pool**)(g_pPool + 44) = ms_pPointRoutePool; // ms_pPointRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 48) = ms_pPatrolRoutePool; // ms_pPatrolRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 52) = ms_pNodeRoutePool; // ms_pNodeRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 56) = ms_pTaskAllocatorPool; // ms_pTaskAllocatorPool
	*(PoolAllocator::Pool**)(g_pPool + 60) = ms_pPedIntelligencePool; // ms_pPedIntelligencePool
	*(PoolAllocator::Pool**)(g_pPool + 64) = ms_pPedAttractorPool; // ms_pPedAttractorPool

	cacheflush(g_libGTASA + 0x95AC00, g_libGTASA + 0x95AC00 + 255, 1);
}

void (*CPlaceable__InitMatrixArray)();
void CPlaceable__InitMatrixArray_hook()
{	
	// CMatrixLinkList::Init
	reinterpret_cast<int(*)(unsigned long, unsigned long)>(g_libGTASA + 0x407F85)(g_libGTASA + 0x95A988, 10000);
}

int CCustomRoadsignMgr__RenderRoadsignAtomic_hook(uintptr_t thiz, uintptr_t a2)
{
	if(!thiz) {
		return 0;
	}

	// 0x5A60DC
	return ((int(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x5A60DC + 1))(thiz, a2);
}

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	if(bGameInited && pUI && (pUI->onTouchEvent(type, posX, posY)) == true)
		return TouchEvent(type, num, posX, posY);
}

void InstallSAMPHooks()
{
	ARMHook::installHook(g_libGTASA + 0x2697C0, (uintptr_t)TouchEvent_hook, (uintptr_t*)&TouchEvent);
	ARMHook::installHook(g_libGTASA + 0x43AF28, (uintptr_t)CLoadingScreen_DisplayPCScreen_hook, (uintptr_t*)&CLoadingScreen_DisplayPCScreen);
	ARMHook::installHook(g_libGTASA + 0x299B64, (uintptr_t)MobileMenu__Render_hook, (uintptr_t*)&MobileMenu__Render);
}

int (*CUpsideDownCarCheck__IsCarUpsideDown)(int _thiz, uintptr_t _pVehicle);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int _thiz, uintptr_t _pVehicle)
{
	if (*(uintptr_t*)(_pVehicle + 20) && _pVehicle != -1)
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(_thiz, _pVehicle);
	}

	Log("[x / Crash Preventer] CODE #1");

	return 0;
}

int (*RwFrameAddChild)(int _thiz, int _pData);
int RwFrameAddChild_hook(int _thiz, int _pData)
{
	if(!_thiz || !_pData) {
		Log("[x / Crash Preventer] CODE #2");
		return 0;
	} 

	return RwFrameAddChild(_thiz, _pData);
}

int (*RLEDecompress)(int _a1, unsigned int _a2, const char* _a3, unsigned int _a4, unsigned int _a5);
int RLEDecompress_hook(int _a1, unsigned int _a2, const char* _a3, unsigned int _a4, unsigned int _a5)
{
	if (!_a3) {
		Log("[x / Crash Preventer] CODE #3");
		return 0;
	}

	return RLEDecompress(_a1, _a2, _a3, _a4, _a5);
}

int (*CAnimBlendNode__FindKeyFrame)(uintptr_t _thiz, float _a2, int _a3, int _a4);
int CAnimBlendNode__FindKeyFrame_hook(uintptr_t _thiz, float _a2, int _a3, int _a4)
{
	if (!_thiz || !*((uintptr_t*)_thiz + 4)) {
		Log("[x / Crash Preventer] CODE #4");
		return 0;
	}

	return CAnimBlendNode__FindKeyFrame(_thiz, _a2, _a3, _a4);
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1 == 0 || !a1) {
		Log("[x / Crash Preventer] CODE #5");
		return 0;
	}

	return _rwFreeListFreeReal(a1, a2);
}

int (*SetCompAlphaCB)(int a1, char a2);
int SetCompAlphaCB_hook(int a1, char a2)
{
	if (!a1) {
		Log("[x / Crash Preventer] CODE #6");
		return 0;
	}

	return SetCompAlphaCB(a1, a2);
}

int (*CAnimManager__UncompressAnimation)(uintptr_t _thiz, uintptr_t _a2);
int CAnimManager__UncompressAnimation_hook(uintptr_t _thiz, uintptr_t _a2)
{
	if(!_thiz) {
		Log("[x / Crash Preventer] CODE #7");
		return 0;
	}

	return CAnimManager__UncompressAnimation(_thiz, _a2);
}

void (*CStreaming__Init2)(uintptr_t thiz);
void CStreaming__Init2_hook(uintptr_t thiz)
{
	CStreaming__Init2(thiz);

	// 685FA0 ; CStreaming::ms_memoryAvailable
	*(uint32_t*)(g_libGTASA + 0x685FA0) = 0x10000000;
}

char** (*CPhysical__Add)(uintptr_t _thiz);
char** CPhysical__Add_hook(uintptr_t _thiz)
{
	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];

						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							return 0;
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();

							// check attached object valid position
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											return 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CPhysical__Add(_thiz);
}

void (*CPhysical__RemoveAndAdd)(uintptr_t _thiz);
void CPhysical__RemoveAndAdd_hook(uintptr_t _thiz)
{
	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];

						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							return;
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();

							// check attached object valid position
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CPhysical__RemoveAndAdd(_thiz);
}

void InstallCrashFixHooks()
{
  	ARMHook::installPLTHook(g_libGTASA + 0x66EB0C, (uintptr_t)CUpsideDownCarCheck__IsCarUpsideDown_hook, (uintptr_t*)&CUpsideDownCarCheck__IsCarUpsideDown); // 66EB0C
	ARMHook::installPLTHook(g_libGTASA + 0x675490, (uintptr_t)RwFrameAddChild_hook, (uintptr_t*)&RwFrameAddChild); // 675490
	ARMHook::installPLTHook(g_libGTASA + 0x6701C8, (uintptr_t)RLEDecompress_hook, (uintptr_t*)&RLEDecompress); // 6701C8
	ARMHook::installPLTHook(g_libGTASA + 0x67213C, (uintptr_t)CAnimBlendNode__FindKeyFrame_hook, (uintptr_t*)&CAnimBlendNode__FindKeyFrame); // 67213C
	ARMHook::installPLTHook(g_libGTASA + 0x6787F0, (uintptr_t)_rwFreeListFreeReal_hook, (uintptr_t*)&_rwFreeListFreeReal); // 6787F0
	ARMHook::installPLTHook(g_libGTASA + 0x6769F8, (uintptr_t)SetCompAlphaCB_hook, (uintptr_t*)&SetCompAlphaCB); // 6769F8
	ARMHook::installPLTHook(g_libGTASA + 0x6750D4, (uintptr_t)CAnimManager__UncompressAnimation_hook, (uintptr_t*)&CAnimManager__UncompressAnimation); // 6750D4
	ARMHook::installPLTHook(g_libGTASA + 0x6700D0, (uintptr_t)CStreaming__Init2_hook, (uintptr_t*)&CStreaming__Init2); // 6700D0

	ARMHook::installPLTHook(g_libGTASA + 0x667CC4, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);
	ARMHook::installPLTHook(g_libGTASA + 0x66F4B0, (uintptr_t)CPhysical__RemoveAndAdd_hook, (uintptr_t*)&CPhysical__RemoveAndAdd);
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
    if(!pNetGame) return false;

	VEHICLEID vehicleId;
	
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
	{
		vehicleId = pVehiclePool->FindIDFromGtaPtr(_pVehicle);
		if(vehicleId == INVALID_VEHICLE_ID) return false;
		if(!pVehiclePool->GetSlotState(vehicleId)) return false;

		CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
		if(pVehicle)
		{
			if(pVehicle->GetDoorState()) return false;
			if(pVehicle->m_pVehicle->entity.wModelIndex == TRAIN_PASSENGER) return false;
		
			if(pVehicle->m_pVehicle->pDriver && pVehicle->m_pVehicle->pDriver->dwPedType != 0)
				return false;
		}
	}
	
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	if(pLocalPlayer)
	{
		if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == 46)
			pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);

		pLocalPlayer->SendEnterVehicleNotification(vehicleId, false);
		return true;
	}

	return false;
}

void (*CTaskComplexEnterCarAsDriver)(uintptr_t thiz, VEHICLE_TYPE *_pVehicle);
void CTaskComplexEnterCarAsDriver_hook(uintptr_t thiz, VEHICLE_TYPE *_pVehicle)
{
    uintptr_t dwRetAddr = 0;
    __asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
    dwRetAddr -= g_libGTASA;

    if(dwRetAddr == 0x40AC30 + 1) 
    {
        // It's coming from CPlayerInfo::Process, so do some upcalls
        if(!NotifyEnterVehicle(_pVehicle)) 
        {
            if(thiz != 0) 
            {
                // CTask::operator delete
                (( void (*)(uintptr_t))(g_libGTASA + 0x4D6A60 + 1))(thiz);
            }

            (( void (*)())(g_libGTASA + 0x40AC40 + 1))();
            return;
        }
    }
    CTaskComplexEnterCarAsDriver(thiz, _pVehicle);
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) 
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x40A819 || dwRetAddr == 0x409A43) 
	{
		if(pNetGame) 
		{
			if(*(uint32_t*)((uintptr_t)GamePool_FindPlayerPed() + 0x590) == (uint32_t)pVehicle) 
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				uint16_t VehicleID = pVehiclePool->FindIDFromGtaPtr(*(VEHICLE_TYPE**)((uintptr_t)GamePool_FindPlayerPed() + 0x590));
				CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

				if(pLocalPlayer) {
					pLocalPlayer->SendExitVehicleNotification(VehicleID);
				}
			}
		}
	}
 
 	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();

	if(pNetGame)
	{
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if(pTextDrawPool)
			pTextDrawPool->SnapshotProcess();
	}
}

uint32_t (*CHudColours__GetIntColour)(uintptr_t thiz, int a2);
uint32_t CHudColours__GetIntColour_hook(uintptr_t thiz, int a2)
{
	return TranslateColorCodeToRGBA(a2);
}

void (*CWorld__ProcessPedsAfterPreRender)();
void CWorld__ProcessPedsAfterPreRender_hook()
{
	CWorld__ProcessPedsAfterPreRender();

	if(pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
			pPlayerPool->ProcessAttachedObjects();
	}
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if(pNetGame && pNetGame->GetGangZonePool() && pGame->FindPlayerPed()->m_bPlayerControllable)
		pNetGame->GetGangZonePool()->Draw();
}

void InstallGlobalHooks()
{
	ARMHook::installPLTHook(g_libGTASA + 0x6625E0, (uintptr_t)MainMenuScreen__Update_hook, (uintptr_t*)&MainMenuScreen__Update);
	ARMHook::installPLTHook(g_libGTASA + 0x67589C, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
	// ARMHook::installPLTHook(g_libGTASA + 0x3F6550, (uintptr_t)Render2dStuffAfterFade_hook, (uintptr_t*)&Render2dStuffAfterFade);
	ARMHook::installPLTHook(g_libGTASA + 0x672448, (uintptr_t)NvFOpen_hook, (uintptr_t*)&NvFOpen); 
	ARMHook::installPLTHook(g_libGTASA + 0x674C68, (uintptr_t)CStream__InitImageList_hook, (uintptr_t*)&CStream__InitImageList); 
	ARMHook::installPLTHook(g_libGTASA + 0x66F2D0, (uintptr_t)CGame__InitialiseRenderWare_hook, (uintptr_t*)&CGame__InitialiseRenderWare); 
	ARMHook::installPLTHook(g_libGTASA + 0x675D98, (uintptr_t)CModelInfo__AddPedModel_hook, (uintptr_t*)&CModelInfo__AddPedModel);
	ARMHook::installPLTHook(g_libGTASA + 0x67579C, (uintptr_t)CModelInfo__AddAtomicModel_hook, (uintptr_t*)&CModelInfo__AddAtomicModel);
	ARMHook::installPLTHook(g_libGTASA + 0x672468, (uintptr_t)CPools__Initialise_hook, (uintptr_t*)&CPools__Initialise);
	ARMHook::installPLTHook(g_libGTASA + 0x675554, (uintptr_t)CPlaceable__InitMatrixArray_hook, (uintptr_t*)&CPlaceable__InitMatrixArray);
	ARMHook::installPLTHook(g_libGTASA + 0x66FE58, (uintptr_t)CGame__Process_hook, (uintptr_t*)&CGame__Process);
	ARMHook::installPLTHook(g_libGTASA + 0x673950, (uintptr_t)CHudColours__GetIntColour_hook, (uintptr_t*)&CHudColours__GetIntColour);
	// ARMHook::installPLTHook(g_libGTASA + 0x443EE0, (uintptr_t)CRadar__DrawRadarGangOverlay_hook, (uintptr_t*)&CRadar__DrawRadarGangOverlay);	
	// ARMHook::installPLTHook(g_libGTASA + 0x675C68, (uintptr_t)CWorld__ProcessPedsAfterPreRender_hook, (uintptr_t*)&CWorld__ProcessPedsAfterPreRender);

	// CTaskComplexEnterCarAsDriver
	ARMHook::installHook(g_libGTASA + 0x4F6F70, (uintptr_t)CTaskComplexEnterCarAsDriver_hook, (uintptr_t*)&CTaskComplexEnterCarAsDriver);

	// CTaskComplexLeaveCar::CTaskComplexLeaveCar
	ARMHook::installHook(g_libGTASA + 0x4F8904, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)&CTaskComplexLeaveCar);

	InstallCrashFixHooks();
	InstallSAMPHooks();
	HookCPad();
}