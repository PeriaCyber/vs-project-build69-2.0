#include "main.h"
#include "cmdprocs.h"
#include "chat.h"
#include "game/game.h"
#include "net/netgame.h"
#include "ui/ui.h"

extern Chat *pChat;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern UI *pUI;

extern char g_nick[30];
extern char g_pass[30];

void cmd_quit(const char *params)
{
    if(pNetGame)
    {
        if(pNetGame->GetGameState() == GAMESTATE_CONNECTED)
            pNetGame->GetRakClient()->Disconnect(0);
        SAFE_DELETE(pNetGame);
    }
}

void cmd_reconnect(const char *params) 
{
	pChat->AddInfoMessage("-> Reconnecting...");
    if(pNetGame)
    {
        if(pNetGame->GetGameState() == GAMESTATE_CONNECTED)
            pNetGame->GetRakClient()->Disconnect(200);

	    pNetGame->ShutDownForGameRestart();
        pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
    }
}

void cmd_savepos(const char *params)
{
    CPlayerPed *pPlayer = pGame->FindPlayerPed();
    FILE *fileOut = nullptr;
    uint32_t dwVehicleID;
    float fZAngle = 0.0f;
    
    char ff[0xFF];
    sprintf(ff, "%sSAMP/savedpos.txt", GetStorage());
    fileOut = fopen(ff, "a");
    if(!fileOut)
        pChat->AddInfoMessage("Cant open savedpos.txt");
        
    if(pPlayer->IsInVehicle())
    {
        // in vehicle savepos
        VEHICLE_TYPE *pVehicle = pPlayer->GetGtaVehicle();
        VEHICLEID m_dwGTAId = GamePool_Vehicle_GetIndex(pVehicle);
        ScriptCommand(&get_car_z_angle, m_dwGTAId, &fZAngle);
        fprintf(fileOut, "AddStaticVehicle(%u,%.4f,%.4f,%.4f,%.4f,%u,%u); // %s\n",
            pVehicle->entity.wModelIndex,
            pVehicle->entity.mat->pos.X, 
            pVehicle->entity.mat->pos.Y, 
            pVehicle->entity.mat->pos.Z, 
            fZAngle,
            pVehicle->byteColor1,
            pVehicle->byteColor2,
            params
        );
        pChat->AddInfoMessage("-> In car position saved.");
    }
    else
    {
    	// on foot savepos
    	PED_TYPE *pActor = pPlayer->GetGtaActor();
    	ScriptCommand(&get_actor_z_angle, pPlayer->m_dwGTAId, &fZAngle);
    	fprintf(fileOut,"AddPlayerClass(%u,%.4f,%.4f,%.4f,%.4f,0,0,0,0,0,0); // %s\n",
    		pPlayer->GetModelIndex(),
    		pActor->entity.mat->pos.X,
    		pActor->entity.mat->pos.Y,
    		pActor->entity.mat->pos.Z,
    		fZAngle,
    		params
    	);
    	pChat->AddInfoMessage("-> On foot position saved.");
    }
    fclose(fileOut);
}

void cmd_rcon(const char *params)
{
    if(!params || !strlen(params))
    	return;
    	
    uint8_t bytePacketID = ID_RCON_COMMAND;
    RakNet::BitStream bsCommand;
    bsCommand.Write(bytePacketID);
    uint32_t dwCmdLen = (uint32_t)strlen(params);
    bsCommand.Write(dwCmdLen);
    bsCommand.Write(params, dwCmdLen);
    pNetGame->GetRakClient()->Send(&bsCommand, HIGH_PRIORITY, RELIABLE, 0);
    return;
}

void cmd_connect(const char *params)
{

    if(!strlen(params))
        return pChat->AddInfoMessage("-> Usage: /connect {00FF00}[ip] [port]");

    char ip[30];
    char port[10];
    
    sscanf(params, "%s%s", ip, port);
    if(strlen(ip) && strlen(port))
    {
        if(pNetGame->GetGameState() == GAMESTATE_CONNECTED)
            pNetGame->GetRakClient()->Disconnect(200);

        pNetGame = new CNetGame(ip, atoi(port), g_nick, g_pass);
    }
}

void cmd_skin(const char *params)
{
    if(!strlen(params))
        return pChat->AddInfoMessage("-> Usage: /chskin {00FF00}[skin id]");

    CPlayerPed *pPlayer = pGame->FindPlayerPed();
    pPlayer->SetModelIndex(atoi(params));
}

void cmd_clear(const char* params)
{
    for(int i = 0; i < 20; i++)
        pChat->AddClientMessage(0xFFFFFFFF, " ");
}

void cmd_client(const char* params)
{
    pChat->AddClientMessage(0xFFFFFFFF, "-> {00FF00}Client Commands:");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/quit (/q)");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/reconnect (/rec)");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/connect");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/save");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/rcon");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/chskin");
    pChat->AddClientMessage(0xFFFFFFFF, "   - {00FF00}/chatclear");
}

void SetupCommands()
{
    pChat->AddCmdProc("q", cmd_quit);
    pChat->AddCmdProc("quit", cmd_quit);
    pChat->AddCmdProc("rec", cmd_reconnect);
    pChat->AddCmdProc("reconnect", cmd_reconnect);
    pChat->AddCmdProc("save", cmd_savepos);
    pChat->AddCmdProc("rcon", cmd_rcon);
    pChat->AddCmdProc("connect", cmd_connect);
    pChat->AddCmdProc("chskin", cmd_skin);
    pChat->AddCmdProc("chatclear", cmd_clear);
    pChat->AddCmdProc("client", cmd_client);
}
