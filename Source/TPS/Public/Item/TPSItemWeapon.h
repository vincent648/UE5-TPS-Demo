// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSItemBase.h"
#include "Components/TimelineComponent.h"
#include "TPSItemWeapon.generated.h"

/**
 * 
 */

 class ATPSCharacter;
 class UMatineeCameraShake;
 class ATPSPlayerController;
 class UMaterial;
 class ATPSAICharacter;

UENUM(BlueprintType)
enum class EWeaponPosition : uint8
{
	E_Left       UMETA(DisplayName = "Left"),
	E_Right      UMETA(DisplayName = "Right")
};

UCLASS()
class TPS_API ATPSItemWeapon : public ATPSItemBase
{
	GENERATED_BODY()

public:

	ATPSItemWeapon();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Muule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Foregrip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Buttstock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Audio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* Particle;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ExposeOnSpawn = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ExposeOnSpawn = "true"))
	EWeaponPosition Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ExposeOnSpawn = "true"))
	bool IsOnHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* DTItemWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	struct FItemWeapon Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool IsAuto;

	//初始化武器配件
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InitDefaultWeapon();

	//******************** 开火 ****************************************
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayFireSound();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayFireFlash();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchShootMode();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PressFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReleaseFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	FTimerHandle FireHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	ATPSCharacter* Player;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetAmmoAmount() { return Ammo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadClip();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FillClip() {Ammo = Data.ClipSize; }

	UFUNCTION(BlueprintCallable, Category = "CameraShake")
	void PlayCameraShake();

	TSubclassOf<UMatineeCameraShake> AutoCameraShake;
	TSubclassOf<UMatineeCameraShake> SingleCameraShake;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponTimeline")
// 	UTimelineComponent* TimelineComp;
// 
// 	FOnTimelineFloat TimelineEvent;
// 
// 	UPROPERTY()
// 	UCurveFloat* FCurve;
// 
// 	float Percent;
// 	float AddYaw;
// 	float AddPitch;
// 	float AddedPer;
// 
// 	UPROPERTY()
// 	float TimelineLength;

// 	UFUNCTION()
// 	void AddControllerInput(float Val);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	ATPSPlayerController* PlayerController;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FTransform GetGunMuzzleTransfrom() {return Muule->GetComponentTransform();}

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireLineTrace();

	UMaterial* BulletHole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ExposeOnSpawn = "true"))
	ACharacter* CharacterOwner;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	bool IsOwnByPlayer();

	ATPSAICharacter* AI;
};
