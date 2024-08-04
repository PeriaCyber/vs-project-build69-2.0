#pragma once

#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <malloc.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdarg>
#include <sys/mman.h>
#include <unistd.h>
#include <unordered_map>
#include <dlfcn.h>

#define CLIENT_AUTHOR      ""
#define CLIENT_COLOR       0xFF00FF00

#define SAMP_VERSION	"0.3.7"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/RakNet/SAMP/samp_netencr.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"

#include "vendor/inih/cpp/INIReader.h"

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(a) ( sizeof((a)) / sizeof(*(a)) )
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = NULL; } }
#endif

extern uintptr_t g_libGTASA;

void InitBassLibrary();

void Log(const char *fmt, ...);
uint32_t GetTickCount();
const char* GetStorage();

#include "game/common.h"
#include "game/util.h"

// BASS
extern int (*BASS_Init)(uint32_t, uint32_t, uint32_t);
extern int (*BASS_Free)(void);
extern int (*BASS_SetConfigPtr)(uint32_t, const char*);
extern int (*BASS_SetConfig)(uint32_t, uint32_t);
extern int (*BASS_ChannelStop)(uint32_t);
extern int (*BASS_StreamCreateURL)(char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_StreamCreateFile)(bool, char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_MusicLoad)(bool, char*, uint32_t, uint32_t, uint32_t, uint32_t);
extern int (*BASS_ChannelPlay)(uint32_t);
extern int *BASS_ChannelGetTags;
extern int *BASS_ChannelSetSync;
extern int *BASS_StreamGetFilePosition;
extern int (*BASS_StreamFree)(uint32_t);
extern int (*BASS_ChannelIsActive)(uint32_t);
extern int (*BASS_SetVolume)(float);