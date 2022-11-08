// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Buffers.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
class NetHandler;
class ANetCharacter;

UCLASS()
class MULTITEST_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:

	UMyGameInstance();

	virtual void Init()override;

	virtual void Shutdown()override;

	template<typename T>
	void SendPacket(T& packet, uint16 pktID)
	{
		TSharedPtr<ChunkBuffer> buffer = _netHandler->Serialize(packet, pktID);
		_netHandler->Send(buffer, buffer->GetWriteSize()); // Send something in the job queue
	}

	TSharedPtr<NetHandler> GetNetHandler() { return _netHandler; }

	void SpawnPlayer(uint32 index, FVector loc = FVector::ZeroVector, FRotator rot = FRotator::ZeroRotator);

private:

	TSharedPtr<NetHandler> _netHandler;

	TMap<uint32, ANetCharacter*> _players;


	TSubclassOf<ANetCharacter> _netCharacter;


	FCriticalSection _lock;
};
