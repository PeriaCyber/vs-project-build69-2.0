#include "../main.h"
#include "game.h"
#include "../net/netgame.h"

extern CNetGame *pNetGame;

#define PI 3.14159265

uintptr_t dwPlayerPedPtrs[PLAYER_PED_SLOTS];

extern char* PLAYERS_REALLOC;

PED_TYPE* GamePool_FindPlayerPed()
{
	return ((PED_TYPE*(*)(int))(g_libGTASA + 0x40B288 + 1))(-1);
}

PED_TYPE* GamePool_Ped_GetAt(int iID)
{
	return (( PED_TYPE* (*)(int))(g_libGTASA + 0x483D49))(iID);
}

int GamePool_Ped_GetIndex(PED_TYPE *pActor)
{
    return (( int (*)(uintptr_t))(g_libGTASA + 0x483D3A + 1))((uintptr_t)pActor);
}

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID)
{
	return (( VEHICLE_TYPE* (*)(int))(g_libGTASA + 0x483D2F))(iID);
}

int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle)
{
    return (( int (*)(VEHICLE_TYPE*))(g_libGTASA + 0x483D21))(pVehicle);
}

ENTITY_TYPE *GamePool_Object_GetAt(int iID)
{
	ENTITY_TYPE* (*GetPoolObj)(int iID);
	*(void **) (&GetPoolObj) = (void*)(g_libGTASA + 0x483D63);
	return (GetPoolObj)(iID);
}

void cp1251_to_utf8(char *out, const char *in, unsigned int len)
{
    static const int table[128] =
    {
        // 80
        0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
        0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
        // 90
        0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
        0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
        // A0
        0x80b8e0,	0x81b8e0,	0x82b8e0,	0x83b8e0,	0x84b8e0,	0x85b8e0,	0x86b8e0,	0x87b8e0,
        0x88b8e0,	0x89b8e0,	0x8ab8e0,	0x8bb8e0,	0x8cb8e0,	0x8db8e0,	0x8eb8e0,	0x8fb8e0,
        // B0
        0x90b8e0,	0x91b8e0,	0x92b8e0,	0x93b8e0,	0x94b8e0,	0x95b8e0,	0x96b8e0,	0x97b8e0,
        0x98b8e0,	0x99b8e0,	0x9ab8e0,	0x9bb8e0,	0x9cb8e0,	0x9db8e0,	0x9eb8e0,	0x9fb8e0,
        // C0
        0xa0b8e0,	0xa1b8e0,	0xa2b8e0,	0xa3b8e0,	0xa4b8e0,	0xa5b8e0,	0xa6b8e0,	0xa7b8e0,
        0xa8b8e0,	0xa9b8e0,	0xaab8e0,	0xabb8e0,	0xacb8e0,	0xadb8e0,	0xaeb8e0,	0xafb8e0,
        // D0
        0xb0b8e0,	0xb1b8e0,	0xb2b8e0,	0xb3b8e0,	0xb4b8e0,	0xb5b8e0,	0xb6b8e0,	0xb7b8e0,
        0xb8b8e0,	0xb9b8e0,	0xbab8e0,	0       ,	0       ,	0       ,	0       ,	0xbfb8e0,
        // E0
        0x80b9e0,	0x81b9e0,	0x82b9e0,	0x83b9e0,	0x84b9e0,	0x85b9e0,	0x86b9e0,	0x87b9e0,
        0x88b9e0,	0x89b9e0,	0x8ab9e0,	0x8bb9e0,	0x8cb9e0,	0x8db9e0,	0x8eb9e0,	0x8fb9e0,
        // F0
        0x90b9e0,	0x91b9e0,	0x92b9e0,	0x93b9e0,	0x94b9e0,	0x95b9e0,	0x96b9e0,	0x97b9e0,
        0x98b9e0,	0x99b9e0,	0x9ab9e0,	0x9bb9e0
    };

    int count = 0;

    while (*in)
    {
        if(len && (count >= len)) break;

        if (*in & 0x80)
        {
            register int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;   
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else // ASCII
            *out++ = *in++;

        count++;
    }

    *out = 0;
}

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk)
{
	return 0;
}

// 0.3.7
bool IsPedModel(unsigned int iModelID)
{
	if(iModelID < 0 || iModelID > 20000) return false;
    uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + 0x91DCB8);
    
    uintptr_t ModelInfo = dwModelArray[iModelID];
    if(ModelInfo && *(uintptr_t*)ModelInfo == (uintptr_t)(g_libGTASA + 0x667668/*CPedModelInfo vtable*/))
        return true;

    return false;
}

// 0.3.7
bool IsValidModel(unsigned int uiModelID)
{
    if(uiModelID < 0 || uiModelID > 20000) return false;
    uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + 0x91DCB8);

    uintptr_t dwModelInfo = dwModelArray[uiModelID];
    if(dwModelInfo && *(uintptr_t*)(dwModelInfo + 0x34/*pRwObject*/)) {
        return true;
    }

    return false;
}

uint16_t GetModelReferenceCount(int nModelIndex)
{
	if(nModelIndex < 0 || nModelIndex > 20000) return 0;

	uintptr_t *dwModelarray = (uintptr_t*)(g_libGTASA + 0x91DCB8);
	uint8_t *pModelInfoStart = (uint8_t*)dwModelarray[nModelIndex];
	
	return *(uint16_t*)(pModelInfoStart + 0x1E);
}

void InitPlayerPedPtrRecords()
{
	memset(&dwPlayerPedPtrs[0], 0, sizeof(uintptr_t) * PLAYER_PED_SLOTS);
}

void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr)
{
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}

uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr)
{
	uint8_t x = 0;
	while(x != PLAYER_PED_SLOTS)
	{
		if(dwPlayerPedPtrs[x] == dwPedPtr) return x;
		x++;
	}

	return 0;
}

uintptr_t GetTexture(const char* texture)
{
	// GetTexture
	uintptr_t pRwTexture = (( uintptr_t (*)(const char*))(g_libGTASA + 0x1E9CE4 + 1))(texture);
	if(pRwTexture) 
	{
		int iTextureCount = *(int*)(pRwTexture + 0x54);
		iTextureCount++;
		*(int*)(pRwTexture + 0x54) = iTextureCount;
	}

	return pRwTexture;
}

uintptr_t LoadTextureFromDB(const char* dbname, const char* texture)
{
	// TextureDatabaseRuntime::GetDatabase
	uintptr_t db_handle = (( uintptr_t (*)(const char*))(g_libGTASA + 0x1EAC8C + 1))(dbname);
	if(!db_handle)
	{
		Log("Error: Database not found! (%s)", dbname);
		return 0;
	}
	// TextureDatabaseRuntime::Register
	(( void (*)(uintptr_t))(g_libGTASA + 0x1E9BC8 + 1))(db_handle);
	uintptr_t tex = GetTexture(texture);

	if(!tex) Log("Error: Texture (%s) not found in database (%s)", texture, dbname);

	// TextureDatabaseRuntime::Unregister
	(( void (*)(uintptr_t))(g_libGTASA + 0x1E9C80 + 1))(db_handle);

	return tex;
}

void DefinedState()
{
	// DefinedState
	(( void (*)())(g_libGTASA + 0x5D0BC0 + 1))();
}

void DefinedState2d()
{
	return (( void (*)())(g_libGTASA + 0x5D0C65))();
}

void SetScissorRect(void* pRect)
{
	return ((void(*)(void*))(g_libGTASA + 0x2B3E55))(pRect);
}

float DegToRad(float fDegrees)
{
    if (fDegrees > 360.0f || fDegrees < 0.0f) return 0.0f;
    
    if (fDegrees > 180.0f) return (float)(-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
    else return (float)((fDegrees * PI) / 180.0f);
}

// 0.3.7
float FloatOffset(float f1, float f2)
{   
    if(f1 >= f2) return f1 - f2;
    else return (f2 - f1);
}

uintptr_t GetModelInfoByID(int iModelID)
{
	if (iModelID < 0 || iModelID > 20000) {
		return false;
	}

	uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + 0x91DCB8);
	return dwModelArray[iModelID];
}

void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject)
	{
		if (*(uint8_t*)rwObject == 1)
		{
			((void(*)(uintptr_t))( *(uintptr_t*)(rwObject + 0x48) ))(rwObject);
		} 
		else if (*(uint8_t*)rwObject == 2)
		{
			((void(*)(uintptr_t))(g_libGTASA + 0x2142DD))(rwObject);
		}
	}
}

void DestroyAtomicOrClump(uintptr_t rwObject)
{
	if (rwObject)
	{
		int type = *(int*)(rwObject);

		if (type == 1)
		{
			// RpAtomicDestroy
			((void(*)(uintptr_t))(g_libGTASA + 0x2141ED))(rwObject); 

			uintptr_t parent = *(uintptr_t*)(rwObject + 4);

			if (parent)
			{
				// RwFrameDestroy
				((void(*)(uintptr_t))(g_libGTASA + 0x1D846D))(parent); 
			}

		}
		else if (type == 2)
		{
			// RpClumpDestroy
			((void(*)(uintptr_t))(g_libGTASA + 0x21460D))(rwObject);
		}
	}
}

void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float *uv)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

		// CSprite2d::Draw
		((void(*)(uintptr_t, RECT*, uint32_t*, float, float, float, float, float, float, float, float))(g_libGTASA + 0x5C95C1))(texture, rect, &dwColor, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
	}
}

void calculateAimVector(VECTOR *vec1, VECTOR *vec2)
{
	float f1;
  	float f2;
  	float f3;

  	f1 = atan2(vec1->X, vec1->Y) - 1.570796370506287;
  	f2 = sin(f1);
  	f3 = cos(f1);
  	vec2->X = vec1->Y * 0.0 - f3 * vec1->Z;
  	vec2->Y = f2 * vec1->Z - vec1->X * 0.0;
  	vec2->Z = f3 * vec1->X - f2 * vec1->Y;
}

uintptr_t LoadTexture(const char* texname)
{
	static char* texdb[] = { "samp", "gta3", "gta_int", "player", "txd" };

	for (int i = 0; i < 5; i++)
	{
		uintptr_t texture = LoadTextureFromDB(texdb[i], texname);
		if (texture != 0) {
			return texture;
		}
	}

	Log("Texture %s not found!", texname);
	return 0;
}

void DeleteRwTexture(uintptr_t pRwTexture)
{
	return (( void (*)(uintptr_t))(g_libGTASA + 0x1DB765))(pRwTexture);
}

void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR *vecPos)
{
	vecOut->X = mat->at.X * vecPos->Z + mat->up.X * vecPos->Y + mat->right.X * vecPos->X + mat->pos.X;
	vecOut->Y = mat->at.Y * vecPos->Z + mat->up.Y * vecPos->Y + mat->right.Y * vecPos->X + mat->pos.Y;
	vecOut->Z = mat->at.Z * vecPos->Z + mat->up.Z * vecPos->Y + mat->right.Z * vecPos->X + mat->pos.Z;
}

void RwMatrixOrthoNormalize(MATRIX4X4 *matIn, MATRIX4X4 *matOut)
{
	// RwMatrixOrthoNormalize
	((void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA + 0x1E34A0 + 1))(matIn, matOut);
}

void RwMatrixInvert(MATRIX4X4 *matOut, MATRIX4X4 *matIn)
{
	// RwMatrixInvert
	((void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA + 0x1E3A28 + 1))(matOut, matIn);
}

void RwMatrixRotate(MATRIX4X4 *mat, int axis, float angle)
{
	static float rMat[3][3] = 
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};

	// RwMatrixRotate
	(( void (*)(MATRIX4X4*, float*, float, int))(g_libGTASA + 0x1E3974 + 1))(mat, rMat[axis], angle, 1);
}

void RwMatrixScale(MATRIX4X4 *matrix, VECTOR *vecScale)
{
	matrix->right.X *= vecScale->X;
	matrix->right.Y *= vecScale->X;
	matrix->right.Z *= vecScale->X;

	matrix->up.X *= vecScale->Y;
	matrix->up.Y *= vecScale->Y;
	matrix->up.Z *= vecScale->Y;

	matrix->at.X *= vecScale->Z;
	matrix->at.Y *= vecScale->Z;
	matrix->at.Z *= vecScale->Z;

	matrix->flags &= 0xFFFDFFFC;
}

bool IsPointInRect(float x, float y, RECT* rect)
{
	return (x >= rect->X1 && x <= rect->X2 && y >= rect->Y1 && y <= rect->Y2);
}

bool bTextDrawTextureSlotState[200];
uintptr_t TextDrawTexture[200];

int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++)
	{
		if (bTextDrawTextureSlotState[i] == false)
		{
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}

void DestroyTextDrawTexture(int index)
{
	if (index >= 0 && index < 200)
	{
		bTextDrawTextureSlotState[index] = false;
		TextDrawTexture[index] = 0x0;
	}
}

bool IsValidGameVehicle(VEHICLE_TYPE *pVehicle) 
{
	// IsVehiclePointerValid
	return (((bool (*)(VEHICLE_TYPE *))(g_libGTASA + 0x585390 + 1))(pVehicle));
}