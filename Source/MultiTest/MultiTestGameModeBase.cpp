// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiTestGameModeBase.h"
#include "MyGameInstance.h"
#include "NetHandler.h"
#include "MyPlayerController.h"


AMultiTestGameModeBase::AMultiTestGameModeBase()
{
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMultiTestGameModeBase::ProcessJobQueue()
{
	if (_gameInstance && _netHandler)
	{
		TFunction<void()> func = []() {};
		while(_netHandler->DequeSpawnJob(func))
		{
			func();
		}
	}
}

void AMultiTestGameModeBase::StartPlay()
{
	Super::StartPlay();

	_gameInstance = Cast<UMyGameInstance>(GetGameInstance());
	_netHandler = _gameInstance->GetNetHandler();

	GetWorldTimerManager().SetTimer(netTimer, FTimerDelegate::CreateLambda(
		[&]() 
		{
			ProcessJobQueue();
		}
	), 2.f, true);
}
