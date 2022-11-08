// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiTestGameModeBase.generated.h"


class UMyGameInstance;
class NetHandler;
/**
 * 
 */
UCLASS()
class MULTITEST_API AMultiTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AMultiTestGameModeBase();

	UFUNCTION()
	void ProcessJobQueue();

	virtual void StartPlay();

private:

	UMyGameInstance* _gameInstance;

	TSharedPtr<NetHandler> _netHandler;

	FTimerHandle netTimer;

};
