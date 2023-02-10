// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSItemBase.h"
#include "TPSItemUse.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSItemUse : public ATPSItemBase
{
	GENERATED_BODY()

public:

	ATPSItemUse();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Use")
	float UseTime;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Use")
	class UDataTable* DTItemUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Use")
	struct FItemUse Data;

	virtual int32 GetWeight() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Use")
	EItemType GetItemTypeByUse();
};
