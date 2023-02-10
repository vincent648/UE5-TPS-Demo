// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TPSItemBase.h"
#include "TPSPickupBase.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPickupBase : public ATPSItemBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginOverlap, ATPSPickupBase*, PickupObject);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndOverlap, ATPSPickupBase*, PickupObject);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* OutLineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;

public:

	ATPSPickupBase();

protected:

	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText UI_Prefix;

public:

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnBeginOverlap OnBeginOverlap;

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnEndOverlap OnEndOverlap;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void InitPickup(EItemType TypeIn, FText NameIn, FText UI_PrefixIn, UStaticMesh* StaticMeshIn);

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void EnableOutLine(bool Enable);

	virtual int32 GetWeight() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pickup")
	UStaticMeshComponent* GetMesh() {return Mesh;}
};
