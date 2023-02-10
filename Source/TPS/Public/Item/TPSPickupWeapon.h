// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSPickupBase.h"
#include "TPSPickupWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPickupWeapon : public ATPSPickupBase
{
	GENERATED_BODY()

public:

	ATPSPickupWeapon();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupWeapon")
	class UDataTable* DTItemWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupWeapon")
	struct FItemWeapon Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupWeapon", meta = (ExposeOnSpawn = "true"))
	int32 Ammo;
	
};
