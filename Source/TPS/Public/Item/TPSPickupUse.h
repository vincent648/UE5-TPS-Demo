// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSPickupBase.h"
#include "TPSPickupUse.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPickupUse : public ATPSPickupBase
{
	GENERATED_BODY()

public:

	ATPSPickupUse();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "PickupUse")
	int32 SingleWeight;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupUse")
	class UDataTable* DTItemUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickupUse")
	struct FItemUse Data;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PickupUse")
	EItemType GetItemTypeByUse();

	virtual int32 GetWeight() const override;
	
};
