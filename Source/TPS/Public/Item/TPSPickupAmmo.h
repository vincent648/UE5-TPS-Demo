// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSPickupBase.h"
#include "TPSPickupAmmo.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPickupAmmo : public ATPSPickupBase
{
	GENERATED_BODY()

public:

	ATPSPickupAmmo();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "PickupAmmo")
	int32 SingleWeight;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupAmmo")
	class UDataTable* DTItemAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupAmmo")
	struct FItemAmmo Data;

	virtual int32 GetWeight() const override;
};
