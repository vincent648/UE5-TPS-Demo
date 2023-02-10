// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "DataTableStruct.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FItemWeapon : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UTexture2D* BigIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UTexture2D* Thumbnail;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		FName UseAmmoID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		int32 ClipSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		int32 Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float FiringInterval;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float ReplaceClipTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float ReplaceBulletTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool AutoMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		USkeletalMesh* SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UStaticMesh* DefaultMag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UStaticMesh* DefaultSight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		UStaticMesh* DefaultButtStock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		USoundBase* FireSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		USoundBase* FireSoundSuppressor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool CanSight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool CanMuzzle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool CanGrip;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool CanClip;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		bool CanButtStock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMin_Stillness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMax_Stillness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMin_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMax_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMin_AimStillness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMax_AimStillness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMin_AimMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float CrossOffsetMax_AimMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float VerticalOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float HorizontalOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float InitVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float Velocity100M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float Velocity300M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float Velocity500M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeapon")
		float WalkSpeedPer;
};

USTRUCT(BlueprintType)
struct FItemAmmo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemAmmo")
		FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemAmmo")
		UStaticMesh* PickupMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemAmmo")
		int32 PickupAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemAmmo")
		UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemAmmo")
		int32 Weight;
};

UENUM(BlueprintType)
enum class EWeaponAccType : uint8
{
	E_Sight       UMETA(DisplayName = "Sight"),
	E_Muzzle      UMETA(DisplayName = "Muzzle"),
	E_Foregrip    UMETA(DisplayName = "Foregrip"),
	E_Mag         UMETA(DisplayName = "Mag"),
	E_ButtStock   UMETA(DisplayName = "ButtStock")
};

USTRUCT(BlueprintType)
struct FItemWeaponAcc : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		EWeaponAccType Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		TArray<FText> WeaponDs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		int32 Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		UStaticMesh* StaticMesh2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		int32 SightMultiple;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		bool IsSuppressor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		bool IsNotFlash;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		float ClipRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		int32 ClipCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		float VerticalAdjPer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		float HorizontalAdjPer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemWeaponAcc")
		FName SocketName;
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	E_Weapon       UMETA(DisplayName = "Weapon"),
	E_Accessories  UMETA(DisplayName = "Accessories"),
	E_Ammo         UMETA(DisplayName = "Ammo"),
	E_Health       UMETA(DisplayName = "Health"),
	E_Boost        UMETA(DisplayName = "Boost"),
	E_Helmet       UMETA(DisplayName = "Helmet"),
	E_Vest         UMETA(DisplayName = "Vest"),
	E_Backpack     UMETA(DisplayName = "Backpack"),
	E_Fashion      UMETA(DisplayName = "Fashion")
};

USTRUCT(BlueprintType)
struct FItemEquipment : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		EItemType Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		UTexture2D* Thumbnails;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemEquipment")
		int32 Capacity;
};

UENUM(BlueprintType)
enum class EItemUse : uint8
{
	E_Health       UMETA(DisplayName = "Health"),
	E_Boost        UMETA(DisplayName = "Boost")
};

USTRUCT(BlueprintType)
struct FItemUse : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		EItemUse Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		int32 Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		float UseTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		float Value;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemUse")
		float Limit;
};


class TPS_API DataTableStruct
{
public:
	DataTableStruct();
	~DataTableStruct();
};
