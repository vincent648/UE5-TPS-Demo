// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Item/TPSItemWeapon.h"
#include "Item/TPSItemBase.h"
#include "TPSPlayerState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponPosition
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponPosition Position;
	UPROPERTY()
	bool IsOnHand;

	FWeaponPosition(){}
	FWeaponPosition(EWeaponPosition InPosition, bool InIsOnHand)
	{
		Position = InPosition;
		IsOnHand = InIsOnHand;
	}
};

UCLASS()
class TPS_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponChange, ATPSItemWeapon*, Weapon, FWeaponPosition, GunPosition);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoChange);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPointChange, float, Value);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemsChange, ATPSItemBase*, ChangeItem, bool, IsAdd);

public:

	ATPSPlayerState();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	ATPSItemWeapon* Weapon1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	ATPSItemWeapon* Weapon2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	ATPSItemWeapon* HoldGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	int32 Ammo556;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	int32 Ammo762;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TArray<ATPSItemBase*> ItemsInBackpack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float HealthPoint;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	ATPSItemWeapon* GetWeapon1() { return Weapon1; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	ATPSItemWeapon* GetWeapon2() { return Weapon2; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	ATPSItemWeapon* GetHoldGun() { return HoldGun; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	int32 GetAmmo556() { return Ammo556; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	int32 GetAmmo762() { return Ammo762; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	float GetHealthPoint() { return HealthPoint; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	TArray<ATPSItemBase*> GetItems() { return ItemsInBackpack; }

	UPROPERTY(BlueprintAssignable, Category = "Items")
	FOnWeaponChange OnWeaponChange;

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetWeapon1(ATPSItemWeapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetWeapon2(ATPSItemWeapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetHoldGun(ATPSItemWeapon* Weapon);

	UPROPERTY(BlueprintAssignable, Category = "Items")
	FOnAmmoChange OnAmmoChange;

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetAmmo556(int32 Ammo556In);

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnHealthPointChange OnHealthPointChange;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetHealthPoint(float HealthPointIn);

	UPROPERTY(BlueprintAssignable, Category = "Items")
	FOnItemsChange OnItemsChange;

	UFUNCTION(BlueprintCallable, Category = "Items")
	void AddItem(ATPSItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void RemoveItem(ATPSItemBase* Item);
};
