// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "ThreadHandler.h"
#include "NetHandler.h"
#include "Buffers.h"
#include "PacketProcessor.h"
#include "NetCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FileLoader.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"

UMyGameInstance::UMyGameInstance()
{
	_netCharacter = UFileLoader::FindClass<ANetCharacter>(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (!_netCharacter)
	{
		_netCharacter = ANetCharacter::StaticClass();
	}
}

void UMyGameInstance::Init()
{
	_netHandler = MakeShared<NetHandler>(L"127.0.0.1",this);
	_netHandler->SetMyPlayerIndex(-1);

	ThreadHandler::GetInstance()->LaunchThread(
		[=]()
		{
			if (_netHandler && _netHandler->GetConnectionState())
			{
				_netHandler->Receive(); // Recv
			}
		}
		,L"Connector"
		,[=]()
		{
			//init
			_netHandler->Connect();
			Protocol::C_LOGIN login;
			SendPacket(login, PKT_C_LOGIN);
		});

	
	ThreadHandler::GetInstance()->LaunchThread(
		[=]()
		{
			if (_netHandler && _netHandler->GetConnectionState())
			{
				_netHandler->Receive(); // Recv
			}
		}
		, L"Receiver"
		, []() {});
	

	for (int i = 0; i < 2; i++)
	{
		ThreadHandler::GetInstance()->LaunchThread(
			[&]()
			{
				{
					FScopeLock lock(&_lock);
					if (_netHandler && _netHandler->GetConnectionState())
					{
						TFunction<void()> job = []() {};
						if (_netHandler->_jobQueue.Dequeue(job))
						{
							lock.Unlock();
							job();
						}
					}
				}
				FPlatformProcess::Sleep(0.05f);
			}
			, L"WorkerThread"
			, []() {});
	}

}

void UMyGameInstance::Shutdown()
{
	ThreadHandler::GetInstance()->JoinThreads();
}


void UMyGameInstance::SpawnPlayer(uint32 index,FVector loc,FRotator rot)
{	
	ANetCharacter* character = nullptr;
	if (!_players.Find(index))
	{
		character = Cast<ANetCharacter>(GetWorld()->SpawnActor(_netCharacter));
		if (!character)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Character spawn failed"));
			return;
		}
		
		AController* newController = GetWorld()->SpawnActor<AController>(AMyPlayerController::StaticClass(),loc,rot);
		character->PossessedBy(newController);
		character->SetIndex(index);
		_players.Add(index,character);
	}


	if (character && _netHandler && _netHandler->_myPlayerIndex == index)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Control Spawned Character"));
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(character);
	}


}

