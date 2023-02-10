// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSAICharacter.generated.h"

UCLASS()
class TPS_API ATPSAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health = 100.f;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void DamageAI(float Damage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool IsDead;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AI")
	void AIDead();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AI")
	void PlayAIMontage(EMontageType Type, float ReloadTime = 1.0f);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AI")
	void BeingDamage();

};
