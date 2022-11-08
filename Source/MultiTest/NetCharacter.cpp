// Fill out your copyright notice in the Description page of Project Settings.


#include "NetCharacter.h"
#include "MyGameInstance.h"
#include "PacketProcessor.h"
#include "FileLoader.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"



// Sets default values
ANetCharacter::ANetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMesh* mannequinnMesh = LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn"));

	if (mannequinnMesh)
	{
		GetMesh()->SetSkeletalMesh(mannequinnMesh);
		UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh Found!"));
	}
	
}

// Called when the game starts or when spawned
void ANetCharacter::BeginPlay()
{
	Super::BeginPlay();
	_instance = Cast<UMyGameInstance>(GetGameInstance());
	_netHandler = _instance->GetNetHandler();
	
	_playerInfo = _instance->GetNetHandler()->GetPlayerInfo(_playerIndex);
	SetActorLocation(_playerInfo.GetLocation());
	SetActorRotation(_playerInfo.GetRotation());


	GetWorldTimerManager().SetTimer(_netHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			if (_instance && _playerIndex != -1 && _netHandler)
			{
				SyncMove();
			}
			else
			{
				if (!_instance)
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("No Instance"));

				if (!_netHandler)
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("No NetHandler"));
			}
		}
	), 0.125f, true);


	GetWorldTimerManager().SetTimer(_fixHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			if (_playerInfo.id != _netHandler->GetMyPlayerIndex())
			{
				SetActorLocation(_playerInfo.GetLocation());
			}
		}
	), 10.f, true);
}

void ANetCharacter::SyncMove()
{
	_playerInfo =_instance->GetNetHandler()->GetPlayerInfo(_playerIndex);
	if (_playerInfo.id != _netHandler->GetMyPlayerIndex())
	{	
		_netCharacter = true;
		FVector loc(_playerInfo.posX, _playerInfo.posY, _playerInfo.posZ);
		FRotator rot(_playerInfo.rotY, _playerInfo.rotZ, _playerInfo.rotX);
		_previousLoc = loc;
		_previousRot = rot;
		_netIsMoving = _playerInfo.isMoving;
		_netSpeed = _playerInfo.speed;
	}
	else
	{
		_netCharacter = false;
		if (GetCharacterMovement()->GetCurrentAcceleration().Length() > 0.f || GetCharacterMovement()->IsFalling())
		{
			Protocol::C_MOVE movePkt;
			movePkt.set_playerid(_playerIndex);
			FVector loc = GetActorLocation();

			movePkt.set_x(loc.X); movePkt.set_y(loc.Y); movePkt.set_z(loc.Z);
			FRotator rot = GetActorRotation();
			movePkt.set_xrot(rot.Roll); movePkt.set_yrot(rot.Pitch); movePkt.set_zrot(rot.Yaw);
			movePkt.set_speed(GetCharacterMovement()->GetCurrentAcceleration().Length());
			movePkt.set_moving(true);

			_netHandler->PushJob([=]() {_instance->SendPacket(movePkt, PKT_C_MOVE); });
			_isMoving = true;

		}
		else if(_isMoving == true)
		{
			Protocol::C_MOVE movePkt;
			movePkt.set_playerid(_playerIndex);
			FVector loc = GetActorLocation();

			movePkt.set_x(loc.X); movePkt.set_y(loc.Y); movePkt.set_z(loc.Z);
			FRotator rot = GetActorRotation();
			movePkt.set_xrot(rot.Roll); movePkt.set_yrot(rot.Pitch); movePkt.set_zrot(rot.Yaw);
			movePkt.set_speed(GetCharacterMovement()->GetCurrentAcceleration().Length());
			movePkt.set_moving(false);

			_netHandler->PushJob([=]() {_instance->SendPacket(movePkt, PKT_C_MOVE); });
			_isMoving = false;
		}

	}

	
	GetCharacterMovement()->IsFalling();

}

// Called every frame
void ANetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (_playerInfo.id != _netHandler->GetMyPlayerIndex())
	{
		InterpolateNetCharacter(DeltaTime);
	}
}

void ANetCharacter::InterpolateNetCharacter(float DeltaTime)
{
	FVector TargetLoc = _playerInfo.GetLocation();
	FVector currentLoc = GetActorLocation();	
	_netLoc = FMath::VInterpTo(currentLoc, TargetLoc, DeltaTime, 2.5f);
	FRotator currentRot = GetActorRotation();
	_netRot = FMath::RInterpTo(currentRot, _playerInfo.GetRotation(), DeltaTime, 2.5f);
	SetActorLocation(_netLoc);
	SetActorRotation(_netRot);

}

// Called to bind functionality to input
void ANetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

