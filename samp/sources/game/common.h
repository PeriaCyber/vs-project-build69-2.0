#pragma once

#include "RW/RenderWare.h"

#define PLAYER_PED_SLOTS	210

typedef unsigned short VEHICLEID;
typedef unsigned short PLAYERID;

#define PADDING(x, y) uint8_t x[y]

//-----------------------------------------------------------

#pragma pack(push, 1)
typedef struct _VECTOR {
	float X, Y, Z;
	
	_VECTOR()
	{
		X = Y = Z = 0.0f;
	}

	_VECTOR(float f)
	{
		X = Y = Z = f;
	}
	
	_VECTOR(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}
} VECTOR, *PVECTOR;

typedef struct _RECT {
	float X1, Y1, X2, Y2;
} RECT, *PRECT;

#pragma pack(1)
typedef struct _MATRIX4X4 
{
	VECTOR right;		// 0-12 	; r11 r12 r13
	uint32_t  flags;	// 12-16
	VECTOR up;			// 16-28	; r21 r22 r23
	float  pad_u;		// 28-32
	VECTOR at;			// 32-44	; r31 r32 r33
	float  pad_a;		// 44-48
	VECTOR pos;			// 48-60
	float  pad_p;		// 60-64
} MATRIX4X4, *PMATRIX4X4;

//-----------------------------------------------------------

struct stRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

#pragma pack(1)
typedef struct _ENTITY_TYPE
{
	uint32_t vtable;					// 0-4		;vtable
	VECTOR vecPos;						// 4-16
	float fHeading;						// 16-20
	MATRIX4X4 *mat;						// 20-24	;mat
	
	union {
		uintptr_t pRwObject;
		uintptr_t pRpClump;
		uintptr_t pRpAtomic;
	};									// 24-28
	
	union {
		uint32_t dwEntityFlags;			// 28-32	;dwEntityFlags
		struct {
			uint32_t bUsesCollision : 1;
			uint32_t bCollisionProcessed : 1;
			uint32_t bIsStatic : 1;
			uint32_t bHasContacted : 1;
			uint32_t bIsStuck : 1;
			uint32_t bIsInSafePosition : 1;
			uint32_t bWasPostponed : 1;
			uint32_t bIsVisible : 1;

			uint32_t bIsBIGBuilding : 1;
			uint32_t bRenderDamaged : 1;
			uint32_t bStreamingDontDelete : 1;
			uint32_t bRemoveFromWorld : 1;
			uint32_t bHasHitWall : 1;
			uint32_t bImBeingRendered : 1;
			uint32_t bDrawLast :1;
			uint32_t bDistanceFade : 1;
		 
			uint32_t bDontCastShadowsOn : 1;
			uint32_t bOffscreen : 1;
			uint32_t bIsStaticWaitingForCollision : 1;
			uint32_t bDontStream : 1;
			uint32_t bUnderwater : 1;
			uint32_t bHasPreRenderEffects : 1;
			uint32_t bIsTempBuilding : 1;
			uint32_t bDontUpdateHierarchy : 1;
		 
			uint32_t bHasRoadsignText : 1;
			uint32_t bDisplayedSuperLowLOD : 1;
			uint32_t bIsProcObject : 1;
			uint32_t bBackfaceCulled : 1;
			uint32_t bLightObject : 1;
			uint32_t bUnimportantStream : 1;
			uint32_t bTunnel : 1;
			uint32_t bTunnelTransition : 1;
		} wEntityFlags;
	};
	
	PADDING(_pad1, 6);					// 32-38
	uint16_t wModelIndex;				// 38-40	;ModelIndex
	PADDING(_pad93__, 11);				// 40-51
	uint8_t byteAreaCode;				// 51-52
	PADDING(_pad93, 6);					// 52-58
	uint8_t nControlFlags; 				// 58-59
	PADDING(_pad95, 9);					// 59-68

	union {
		uint32_t dwPhysicalFlags;		// 68-72	;dwPhysicalFlags
        struct {
            uint32_t bMakeMassTwiceAsBig : 1;
            uint32_t bApplyGravity : 1;
            uint32_t bDisableCollisionForce : 1;
            uint32_t bCollidable : 1;
            uint32_t bDisableTurnForce : 1;
            uint32_t bDisableMoveForce : 1;
            uint32_t bInfiniteMass : 1;
            uint32_t bDisableZ : 1;

            uint32_t bSubmergedInWater : 1;
            uint32_t bOnSolidSurface : 1;
            uint32_t bBroken : 1;
            uint32_t bProcessCollisionEvenIfStationary : 1; // ref @ 0x6F5CF0
            uint32_t b13 : 1; // only used for peds
            uint32_t bDontApplySpeed : 1;
            uint32_t b15 : 1;
            uint32_t bProcessingShift : 1;

            uint32_t b17 : 1;
            uint32_t bDisableSimpleCollision : 1; // ref @ CPhysical::ProcessCollision
            uint32_t bBulletProof : 1;
            uint32_t bFireProof : 1;
            uint32_t bCollisionProof : 1;
            uint32_t bMeeleProof : 1;
            uint32_t bInvulnerable : 1;
            uint32_t bExplosionProof : 1;

            uint32_t bDontCollideWithFlyers : 1;
            uint32_t bAttachedToEntity : 1;
            uint32_t bAddMovingCollisionSpeed : 1;
            uint32_t bTouchingWater : 1;
            uint32_t bCanBeCollidedWith : 1;
            uint32_t bDestroyed : 1;
            uint32_t b31 : 1;
            uint32_t b32 : 1;
        } wPhysicalFlags;
    };

	VECTOR vecMoveSpeed; 				// 72-84	;vecMoveSpeed
	VECTOR vecTurnSpeed; 				// 84-96	;vecTurnSpeed
	PADDING(_pad94, 88);				// 96-184
	uint32_t dwUnkModelRel;				// 184-188 ;ñîòêà èíôà
} ENTITY_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _WEAPON_SLOT_TYPE
{
	uint32_t dwType;
	uint32_t dwState;
	uint32_t dwAmmoInClip;
	uint32_t dwAmmo;
	uint32_t dwTimeForNextShot;
	PADDING(_pwep1, 8);
} WEAPON_SLOT_TYPE; // MUST BE EXACTLY ALIGNED TO 28 bytes

#pragma pack(1)
typedef struct
{
	char unk[0x14];
	int iNodeId;
} AnimBlendFrameData;

#pragma pack(1)
typedef struct _PED_TASKS_TYPE
{
	uint32_t * pdwPed; // 0-4
	// Basic Tasks
	uint32_t * pdwDamage; // 4-8
	uint32_t * pdwFallEnterExit; // 8-12
	uint32_t * pdwSwimWasted; // 12-16
	uint32_t * pdwJumpJetPack; // 16-20
	uint32_t * pdwAction; // 20-24
	// Extended Tasks
	uint32_t * pdwFighting; // 24-28
	uint32_t * pdwCrouching; // 28-32
	uint32_t * pdwExtUnk1; // 32-36
	uint32_t * pdwExtUnk2; // 36-40
	uint32_t * pdwExtUnk3; // 40-44
	uint32_t * pdwExtUnk4; // 44-48
} PED_TASKS_TYPE;

#pragma pack(1)
typedef struct _PED_TYPE
{
	ENTITY_TYPE entity; 				// 0000-0188	;entity
	PADDING(_pad106, 174);				// 0188-0362
	uint32_t _pad107;					// 0362-0366	;dwPedType
	PADDING(_pad101, 722);				// 0366-1088
	PED_TASKS_TYPE *Tasks; 				// 1088-1092
	uint32_t dwPlayerInOffset;			// 1092-1096
	PADDING(_pad1000_, 4);				// 1096-1110
	uint32_t dwAction;					// 1110-1104	;Action
	PADDING(_pad102, 36);				// 1104-1140
	uintptr_t dwInvulFlags; 			// 1140-1144	0x1000 = can_decap
	PADDING(_pad228, 12); 				// 1144-1156
	uint32_t dwStateFlags; 				// 1156-1160	;StateFlags
	PADDING(_pad103, 12);				// 1160-1172
	AnimBlendFrameData* aPedBones[19];	// 1172-1248
	PADDING(_pad111_, 44);				// 1248-1292
	uint32_t dwActiveVision; 			// 1292-1296	;dwActiveVision
	PADDING(_pad103_, 52);				// 1296-1348
	float fHealth;		 				// 1348-1352	;Health
	float fMaxHealth;					// 1352-1356	;MaxHealth
	float fArmour;						// 1356-1360	;Armour
	float fAim;							// 1360-1364
	PADDING(_pad104, 8);				// 1364-1372
	float fRotation1;					// 1372-1376	;Rotation1
	float fRotation2;					// 1376-1380	;Rotation2
	PADDING(_pad177, 8);				// 1380-1388
	uint32_t dwStandingOnEntity; 		// 1388-1392
	PADDING(_pad292, 24);				// 1392-1416
	uint32_t dwContactEntity; 			// 1416-1420
	PADDING(_pad105, 4);				// 1420-1424
	uint32_t pVehicle;					// 1424-1428	;pVehicle
	PADDING(_pad108, 8);				// 1428-1436
	uint32_t dwPedType;					// 1436-1440	;dwPedType
	uint32_t dwUnk1;	 				// 1440-1444
	WEAPON_SLOT_TYPE WeaponSlots[13]; 	// 1444-1808
	PADDING(_pad270, 12); 				// 1808-1820
	uint8_t byteCurWeaponSlot; 			// 1820-1821
	PADDING(_pad2811, 20); 				// 1821-1841
	uint8_t byteFightingStyle; 			// 1841-1842
	uint8_t byteFightingType; 			// 1842-1843
	PADDING(_pad280, 1); 				// 1843-1844
	uint32_t pFireObject;	 			// 1844-1848
	PADDING(_pad281, 44);		 		// 1848-1892
	uint32_t dwWeaponUsed; 				// 1892-1896
	uintptr_t pdwDamageEntity; 			// 1896-1900
	PADDING(_pad282, 36);		 		// 1900-1936
	uint32_t *dwEntryExit;				// 1936-1940
	PADDING(_pad283, 12);		 		// 1940-1952
	uint32_t dwTarget;					// 1952-1956
} PED_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _VEHICLE_TYPE
{
	ENTITY_TYPE entity;					// 0000-0184
	PADDING(_pad999, 357);				// 0184-0541
	uint8_t byteComponent[15];			// 0541-0556
	PADDING(_pad201, 508);				// 0556-1064

	union {
		uint8_t byteFlags;				// 1064-1072	;byteFlags
		struct {
			unsigned char bIsLawEnforcer : 1;
			unsigned char bIsAmbulanceOnDuty : 1;
			unsigned char bIsFireTruckOnDuty : 1;
			unsigned char bIsLocked : 1;
			unsigned char bEngineOn : 1;
			unsigned char bIsHandbrakeOn : 1;
			unsigned char bLightsOn : 1;
			unsigned char bFreebies : 1;

			unsigned char bIsVan : 1;
			unsigned char bIsBus : 1;
			unsigned char bIsBig : 1;
			unsigned char bLowVehicle : 1;
			unsigned char bComedyControls : 1;
			unsigned char bWarnedPeds : 1;
			unsigned char bCraneMessageDone : 1;
			unsigned char bTakeLessDamage : 1;

			unsigned char bIsDamaged : 1;
			unsigned char bHasBeenOwnedByPlayer : 1;
			unsigned char bFadeOut : 1;
			unsigned char bIsBeingCarJacked : 1;
			unsigned char bCreateRoadBlockPeds : 1;
			unsigned char bCanBeDamaged : 1;
			unsigned char bOccupantsHaveBeenGenerated : 1;
			unsigned char bGunSwitchedOff : 1;

			unsigned char bVehicleColProcessed : 1;
			unsigned char bIsCarParkVehicle : 1;
			unsigned char bHasAlreadyBeenRecorded : 1;
			unsigned char bPartOfConvoy : 1;
			unsigned char bHeliMinimumTilt : 1;
			unsigned char bAudioChangingGear : 1;
			unsigned char bIsDrowning : 1;
			unsigned char bTyresDontBurst : 1;

			unsigned char bCreatedAsPoliceVehicle : 1;
			unsigned char bRestingOnPhysical : 1;
			unsigned char bParking : 1;
			unsigned char bCanPark : 1;
			unsigned char bFireGun : 1;
			unsigned char bDriverLastFrame : 1;
			unsigned char bNeverUseSmallerRemovalRange : 1;
			unsigned char bIsRCVehicle : 1;

			unsigned char bAlwaysSkidMarks : 1;
			unsigned char bEngineBroken : 1;
			unsigned char bVehicleCanBeTargetted : 1;
			unsigned char bPartOfAttackWave : 1;
			unsigned char bWinchCanPickMeUp : 1;
			unsigned char bImpounded : 1;
			unsigned char bVehicleCanBeTargettedByHS : 1;
			unsigned char bSirenOrAlarm : 1;

			unsigned char bHasGangLeaningOn : 1;
			unsigned char bGangMembersForRoadBlock : 1;
			unsigned char bDoesProvideCover : 1;
			unsigned char bMadDriver : 1;
			unsigned char bUpgradedStereo : 1;
			unsigned char bConsideredByPlayer : 1;
			unsigned char bPetrolTankIsWeakPoint : 1;
			unsigned char bDisableParticles : 1;

			unsigned char bHasBeenResprayed : 1;
			unsigned char bUseCarCheats : 1;
			unsigned char bDontSetColourWhenRemapping : 1;
			unsigned char bUsedForReplay : 1;
		} dwFlags;
	};

	uint32_t dwCreationTime;			// 1072-1076
	uint8_t byteColor1;					// 1076-1077
	uint8_t byteColor2;					// 1077-1078
	uint8_t byteColor3;					// 1078-1079
	uint8_t byteColor4;					// 1079-1080
	PADDING(_pad203, 36);				// 1080-1116
	uint16_t sAlarmState;				// 1116-1118
	PADDING(_pad203__, 2);				// 1118-1120
	PED_TYPE *pDriver;					// 1120-1124
	PED_TYPE *pPassengers[8];			// 1124-1156
	PADDING(_pad235, 4); 				// 1156-1160F
	uint8_t byteNumOfSeats; 			// 1160-1161
	PADDING(_pad236, 7); 				// 1161-1168
	uint32_t pFireObject;				// 1168-1172
	PADDING(_pad241__, 20); 			// 1172-1192
	uint8_t byteMoreFlags;				// 1192-1193
	PADDING(_pad275_, 31); 				// 1193-1224
	float fHealth;						// 1224-1228
	uint32_t dwTractor;					// 1228-1232
	uint32_t dwTrailer;					// 1232-1236
	PADDING(_pad208, 48);				// 1236-1284
	uint32_t dwDoorsLocked;				// 1284-1288
	PADDING(_pad2423, 24);				// 1288-1312
	uint8_t byteHorn2;					// 1312-1313
	PADDING(_pad2424, 115);				// 1313-1428
	RwTexture *pCustomCarPlate;			// 1428-1432
	PADDING(_pad1111, 28);				// 1432-1460
	union {
		struct {
			PADDING(_pad245, 1);		// 1460-1461
			uint8_t byteWheelStatus[4]; // 1461-1465
			uint8_t byteDoorStatus[6];	// 1465-1471
			uint8_t byteDamageUnk;		// 1471-1472
			uint32_t dwLightStatus;		// 1472-1476
			uint32_t dwPanelStatus;		// 1476-1480
		};
		struct {
			float fTrainSpeed;			// 1460-1464
			PADDING(_pad210, 16);		// 1464-1480
		};
	};
	PADDING(_pad213, 28);				// 1480-1508
	uint32_t *VehicleAttachedFront;		// 1508-1512
	uint32_t *VehicleAttachedBottom; 	// 1512-1516
	PADDING(_pad211, 108);				// 1516-1624
	float fBikeLean;					// 1624-1628
	uint32_t dwBikeUnk;					// 1628-1632
	PADDING(pad211_, 12);				// 1632-1644
	uint8_t byteBikeWheelStatus[2];		// 1644-1646
	PADDING(_pad21331, 526);			// 1646-2172
	uint16_t sHydraThrustAngle;			// 2172-2174
	PADDING(_pad245123, 350);  			// 2174-2524
	float fPlaneLandingGear;			// 2524-2526
} VEHICLE_TYPE;

//-----------------------------------------------------------

#define	VEHICLE_SUBTYPE_CAR				1
#define	VEHICLE_SUBTYPE_BIKE			2
#define	VEHICLE_SUBTYPE_HELI			3
#define	VEHICLE_SUBTYPE_BOAT			4
#define	VEHICLE_SUBTYPE_PLANE			5
#define	VEHICLE_SUBTYPE_PUSHBIKE		6
#define	VEHICLE_SUBTYPE_TRAIN			7

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0 

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE		331 // was 332
#define WEAPON_MODEL_GOLFCLUB			333
#define WEAPON_MODEL_NITESTICK			334
#define WEAPON_MODEL_KNIFE				335
#define WEAPON_MODEL_BAT				336
#define WEAPON_MODEL_SHOVEL				337
#define WEAPON_MODEL_POOLSTICK			338
#define WEAPON_MODEL_KATANA				339
#define WEAPON_MODEL_CHAINSAW			341
#define WEAPON_MODEL_DILDO				321
#define WEAPON_MODEL_DILDO2				322
#define WEAPON_MODEL_VIBRATOR			323
#define WEAPON_MODEL_VIBRATOR2			324
#define WEAPON_MODEL_FLOWER				325
#define WEAPON_MODEL_CANE				326
#define WEAPON_MODEL_GRENADE			342 // was 327
#define WEAPON_MODEL_TEARGAS			343 // was 328
#define WEAPON_MODEL_MOLOTOV			344 // was 329
#define WEAPON_MODEL_COLT45				346
#define WEAPON_MODEL_SILENCED			347
#define WEAPON_MODEL_DEAGLE				348
#define WEAPON_MODEL_SHOTGUN			349
#define WEAPON_MODEL_SAWEDOFF			350
#define WEAPON_MODEL_SHOTGSPA			351
#define WEAPON_MODEL_UZI				352
#define WEAPON_MODEL_MP5				353
#define WEAPON_MODEL_AK47				355
#define WEAPON_MODEL_M4					356
#define WEAPON_MODEL_TEC9				372
#define WEAPON_MODEL_RIFLE				357
#define WEAPON_MODEL_SNIPER				358
#define WEAPON_MODEL_ROCKETLAUNCHER		359
#define WEAPON_MODEL_HEATSEEKER			360
#define WEAPON_MODEL_FLAMETHROWER		361
#define WEAPON_MODEL_MINIGUN			362
#define WEAPON_MODEL_SATCHEL			363
#define WEAPON_MODEL_BOMB				364
#define WEAPON_MODEL_SPRAYCAN			365
#define WEAPON_MODEL_FIREEXTINGUISHER	366
#define WEAPON_MODEL_CAMERA				367
#define WEAPON_MODEL_NIGHTVISION		368	// newly added
#define WEAPON_MODEL_INFRARED			369	// newly added
#define WEAPON_MODEL_JETPACK			370	// newly added
#define WEAPON_MODEL_PARACHUTE			371

#define OBJECT_PARACHUTE				3131
#define OBJECT_CJ_CIGGY					1485
#define OBJECT_DYN_BEER_1				1486
#define OBJECT_CJ_BEER_B_2				1543
#define OBJECT_CJ_PINT_GLASS			1546