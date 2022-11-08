// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FileLoader.generated.h"

/**
 * 
 */
UCLASS()
class MULTITEST_API UFileLoader : public UObject
{
	GENERATED_BODY()
	
public:
	template<typename T>
	static T* FindObject(const TCHAR* paths)
	{
		static ConstructorHelpers::FObjectFinder<T> object(paths);
		if (object.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT(" Succeeded to find this object"));
			return object.Object;
		}
		UE_LOG(LogTemp, Warning, TEXT(" Failed to find this object"));
		return nullptr;
	}


	template<typename T>
	static TSubclassOf<T> FindClass(const TCHAR* paths)
	{
		static ConstructorHelpers::FClassFinder<T> object(paths);
		if (object.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT(" Succeeded to find this object"));
			return object.Class;
		}
		UE_LOG(LogTemp, Warning,TEXT(" Failed to find this class"));
		return nullptr;
	}



};
