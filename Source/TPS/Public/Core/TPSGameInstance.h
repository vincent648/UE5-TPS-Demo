// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backpack")
	int32 DefaultCapacity = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunStandName = FName(TEXT("Socket_Gun_Stand"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunAimName = FName(TEXT("Socket_Gun_Aim"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunReloadName = FName(TEXT("Socket_Gun_Reload"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunProneIdleName = FName(TEXT("Socket_Gun_ProneIdle"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunProneFBName = FName(TEXT("Socket_Gun_ProneFB"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunProneOtherName = FName(TEXT("Socket_Gun_ProneOther"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName GunCrouchName = FName(TEXT("Socket_Gun_Crouch"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName BackLeftNName = FName(TEXT("Socket_BackLeft_Normal"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName BackLeftBName = FName(TEXT("Socket_BackLeft_Backpack"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName BackRightBName = FName(TEXT("Socket_BackRight_Back"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SocketName")
	FName BackRightNName = FName(TEXT("Socket_BackRight_Normal"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ReloadTime")
	float ReloadClipTimeStand = 2.17f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ReloadTime")
	float ReloadClipTimeProne = 2.4f;
};
