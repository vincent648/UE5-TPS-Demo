// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSItemBase.h"
#include "TPSItemAmmo.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSItemAmmo : public ATPSItemBase
{
	GENERATED_BODY()

public:

	ATPSItemAmmo();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	class UDataTable* DTItemAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	struct FItemAmmo Data;

	virtual int32 GetWeight() const override;
};
