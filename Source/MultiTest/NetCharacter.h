// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetHandler.h"
#include "NetCharacter.generated.h"

class UMyGameInstance;
class NetHandler;

UCLASS()
class MULTITEST_API ANetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SyncMove();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InterpolateNetCharacter(float DeltaTime);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetIndex(uint32 playerIndex) { _playerIndex = playerIndex; }


private:
	UPROPERTY()
	UMyGameInstance* _instance;

	TSharedPtr<NetHandler> _netHandler;

	FTimerHandle _netHandle;

	FTimerHandle _fixHandle;

	uint32 _playerIndex = -1;

	PlayerInfo _playerInfo;

	FVector _netLoc;

	FRotator _netRot;

	FVector _previousLoc;

	FRotator _previousRot;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool _netCharacter = false;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool _netIsMoving = false;

	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double _netSpeed = 0.f;

	bool _isMoving = false;

};
