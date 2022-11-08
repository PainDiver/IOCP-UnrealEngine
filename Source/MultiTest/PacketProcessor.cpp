// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketProcessor.h"
#include "NetHandler.h"
#include "ComfyMacro.h"
#include "MyGameInstance.h"

PacketProcessor::~PacketProcessor()
{
}

bool Handle_INVALID(TSharedPtr<NetHandler> netHandler, BYTE* buffer, uint32 size)
{
	return false;
}

bool Handle_S_LOGIN(TSharedPtr<NetHandler> netHandler, Protocol::S_LOGIN pkt)
{

	if (pkt.success() && netHandler)
	{
		auto player = pkt.player()[0];
		TSharedPtr<PlayerInfo> playerInfo = MakeShared<PlayerInfo>();
		playerInfo->name = ConvertUTF8toFString(player.name());
		playerInfo->playerType = player.playertype();
		playerInfo->SetLocation(rand()%500, rand() % 500, 0);
		playerInfo->SetRotation(0,0,0);
		playerInfo->id = pkt.connectingplayerindex();
		netHandler->SetMyPlayerIndex(playerInfo->id);

		netHandler->PushPlayerInfo(playerInfo->id, playerInfo);
		netHandler->PushSpawnJob([=]() {netHandler->GetOwnerInstance()->SpawnPlayer(playerInfo->id,playerInfo->GetLocation(),playerInfo->GetRotation()); });
		
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, TEXT("S_LOGIN Handled"));


		Protocol::C_ENTER_GAME enterPkt;
		enterPkt.set_playerindex(playerInfo->id);
		netHandler->GetOwnerInstance()->SendPacket(enterPkt,PKT_C_ENTER_GAME);
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, TEXT("C_ENTER SENT"));

	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, TEXT("Connection Failed"));
		//netHandler->SetConnectionState(false);
		return false;
	}

	return true;
}

bool Handle_S_ENTER_GAME(TSharedPtr<NetHandler> netHandler, Protocol::S_ENTER_GAME pkt)
{
	
	if (pkt.success() && netHandler)
	{
		auto players = pkt.player();
		for (auto player : players)
		{
			TSharedPtr<PlayerInfo> playerInfo = MakeShared<PlayerInfo>();
			playerInfo->name = ConvertUTF8toFString(player.name());
			playerInfo->playerType = player.playertype();
			playerInfo->SetLocation(0,0,0);
			playerInfo->SetRotation(0, 0, 0);
			playerInfo->id = player.id();

			netHandler->PushPlayerInfo(playerInfo->id, playerInfo);
			netHandler->PushSpawnJob([=]() {netHandler->GetOwnerInstance()->SpawnPlayer(playerInfo->id); });
		}
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, TEXT("S_ENTER Handled"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, TEXT("Enter Failed"));
		return false;
	}
	return true;
}

bool Handle_S_CHAT(TSharedPtr<NetHandler> netHandler, Protocol::S_CHAT pkt)
{
	if (netHandler)
	{
		int32 playerid = pkt.playerid();
		TCHAR* str = UTF8_TO_TCHAR(pkt.msg().c_str());
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::FromInt(playerid) + FString(" : ") + str);
	}
	else
	{
		return false;
	}
	// InGame Logic
	return true;
}

bool Handle_S_MOVE(TSharedPtr<NetHandler> netHandler, Protocol::S_MOVE pkt)
{
	if(netHandler)
	{
		TSharedPtr<PlayerInfo> playerInfo = netHandler->GetPlayerInfoRef(pkt.playerid());
		if (playerInfo)
		{
			playerInfo->SetLocation(pkt.x(), pkt.y(), pkt.z());
			playerInfo->SetRotation(pkt.xrot(), pkt.yrot(), pkt.zrot());
			playerInfo->SetSpeed(pkt.speed());
			playerInfo->SetMoving(pkt.moving());
		}
	}
	else
	{
		return false;
	}
	return true;
}
