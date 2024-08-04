#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "ui/ui.h"
#include "chat.h"
#include "extrakeyboard.h"
#include "keyboard.h"
#include "dialog.h"
#include "spawnscreen.h"
#include "playertags.h"
//#include "game/snapshothelper.h"
#include "game/audiostream.h"
#include "armhook/armhook.h"

CExtraKeyBoard *pExtraKeyBoard = nullptr;
Chat *pChat = nullptr;
SpawnScreen *pSpawnScreen = nullptr;
PlayerTags *pPlayerTags = nullptr;
Dialog *pDialog = nullptr;
UI *pUI = nullptr;
KeyBoard *pKeyBoard = nullptr;

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
//CSnapShotHelper *pSnapShotHelper = nullptr;
CAudioStream *pAudioStream = nullptr;

uintptr_t g_libGTASA = 0;

void InitRenderWareFunctions();
void InstallGlobalHooks();
void ApplyGlobalPatches();

// settings
char g_ip[30];
int  g_port;
char g_nick[30];
char g_pass[30];

bool bGameInited = false;
bool bNetworkInited = false;

void* InitSAMP(void* p)
{
	while(*(uint32_t*)(g_libGTASA + 0xA987C8) != 7)
    	usleep(500);

	Log("SAMP library loaded! Build time: " __DATE__ " " __TIME__);
	Log("Storage: %s", GetStorage());

	pGame = new CGame();
	pGame->StartGame();
	
	pthread_exit(0);
}

void ReadSettings()
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", GetStorage());

	INIReader reader(buff);
	size_t length = 0;

	length = reader.Get("nickname", "Name", "").copy(g_nick, 30);
	g_nick[length] = '\0';

	length = reader.Get("server", "IP", "").copy(g_ip, 30);
	g_ip[length] = '\0';

	g_port = reader.GetInteger("server", "Port", 7777);

	length = reader.Get("server", "Password", "").copy(g_pass, 30);
	g_pass[length] = '\0';
}

void DoInitStuff()
{
	if(!bGameInited)
	{
		// pSnapShotHelper = new CSnapShotHelper();
		pAudioStream = new CAudioStream();

		pGame->Init();
		pGame->SetMaxStats();
		pGame->ToggleThePassingOfTime(0);
		pAudioStream->Initialize();

		bGameInited = true;
		return;
	}
	
	if(!bNetworkInited)
	{
		pNetGame = new CNetGame(g_ip, g_port, g_nick, g_pass);
		bNetworkInited = true;
	}
}

void MainLoop()
{
	DoInitStuff();

	if(pUI)
		pUI->draw();

	if(pNetGame)
		pNetGame->Process();

	if(pAudioStream)
		pAudioStream->Process();
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if(info->si_signo == SIGSEGV)
	{
		Log("backtrace:");
		Log("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		Log("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
		Log("3: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - ARMHook::getLibraryAddress("libsamp.so"));
		Log("4: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - ARMHook::getLibraryAddress("libsamp.so"));

		exit(0);
	}
	return;
}

/*__attribute__((constructor))
void Init()*/
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	g_libGTASA = ARMHook::getLibraryAddress("libGTASA.so");
	if(g_libGTASA == 0)
	{
		Log("libGTASA.so address not found!");
		return 0;
	}

	ARMHook::initializeTrampolines(0x180044, 0x800);
	srand(time(0));
	InitBassLibrary();

	InitRenderWareFunctions();
	InstallGlobalHooks();
	ApplyGlobalPatches();

	pthread_t Init_Thread;
	pthread_create(&Init_Thread, 0, InitSAMP, 0);

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, 0);

	return JNI_VERSION_1_6;
}

void Log(const char *fmt, ...)
{	
	if(!GetStorage()) return;

	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && GetStorage() != nullptr)
	{
		sprintf(buffer, "%sSAMP/client.log", GetStorage());
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

/* --------------------------------------------------------------------------------------------------------------------------------------------- */
// BASS

int (*BASS_Init)(uint32_t, uint32_t, uint32_t);
int (*BASS_Free)(void);
int (*BASS_SetConfigPtr)(uint32_t, const char*);
int (*BASS_SetConfig)(uint32_t, uint32_t);
int (*BASS_ChannelStop)(uint32_t);
int (*BASS_StreamCreateURL)(char*, uint32_t, uint32_t, uint32_t);
int (*BASS_StreamCreateFile)(bool, char*, uint32_t, uint32_t, uint32_t);
int (*BASS_ChannelPlay)(uint32_t);
int *BASS_ChannelGetTags;
int *BASS_ChannelSetSync;
int *BASS_StreamGetFilePosition;
int (*BASS_StreamFree)(uint32_t);
int (*BASS_ChannelIsActive)(uint32_t);
int (*BASS_SetVolume)(float);
int (*BASS_MusicLoad)(bool, char*, uint32_t, uint32_t, uint32_t, uint32_t);

void InitBassLibrary()
{
	void* _handle = dlopen("/data/data/com.rockstargames.gtasa/lib/libbass.so", 1);
	if ( !_handle ) {
		Log("%s", dlerror());
	}

	BASS_Init = (int (*)(uint32_t, uint32_t, uint32_t))dlsym(_handle, "BASS_Init");
	BASS_Free = (int (*)(void))dlsym(_handle, "BASS_Free");
	BASS_SetConfigPtr = (int (*)(uint32_t, const char*))dlsym(_handle, "BASS_SetConfigPtr");
	BASS_SetConfig = (int (*)(uint32_t, uint32_t))dlsym(_handle, "BASS_SetConfig");
	BASS_ChannelStop = (int (*)(uint32_t))dlsym(_handle, "BASS_ChannelStop");
	BASS_StreamCreateURL = (int (*)(char*, uint32_t, uint32_t, uint32_t))dlsym(_handle, "BASS_StreamCreateURL");
	BASS_StreamCreateFile = (int (*)(bool, char*, uint32_t, uint32_t, uint32_t))dlsym(_handle, "BASS_StreamCreateFile");
	BASS_ChannelPlay = (int (*)(uint32_t))dlsym(_handle, "BASS_ChannelPlay");
	BASS_ChannelGetTags = (int *)dlsym(_handle, "BASS_ChannelGetTags");
	BASS_ChannelSetSync = (int *)dlsym(_handle, "BASS_ChannelSetSync");
	BASS_StreamGetFilePosition = (int *)dlsym(_handle, "BASS_StreamGetFilePosition");
	BASS_StreamFree = (int (*)(uint32_t))dlsym(_handle, "BASS_StreamFree");
	BASS_ChannelIsActive = (int (*)(uint32_t))dlsym(_handle, "BASS_ChannelIsActive");
	BASS_SetVolume = (int (*)(float))dlsym(_handle, "BASS_SetVolume");
	BASS_MusicLoad = (int (*)(bool, char*, uint32_t, uint32_t, uint32_t, uint32_t))dlsym(_handle, "BASS_MusicLoad");
}

const char* GetStorage()
{
	if(g_libGTASA == 0)
		return nullptr;
	return (const char*)(g_libGTASA + 0x6D687C);
}