// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/DataTableStruct.h"
#include "TPSItemBase.generated.h"

UCLASS()
class TPS_API ATPSItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = "true"))
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = "true"))
	FString SN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = "true"))
	int32 Amount;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	class ATPSCharacter* CharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	class ATPSPlayerController* ControllerRef;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	void Init(EItemType ItemTypeIn, FText NameIn);

	virtual int32 GetWeight() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UpdateAmount(bool IsAdd, bool IsReduce, int32 AmountIn);

};
