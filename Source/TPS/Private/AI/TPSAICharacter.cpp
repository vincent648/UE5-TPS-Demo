// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TPSAICharacter.h"

// Sets default values
ATPSAICharacter::ATPSAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATPSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSAICharacter::DamageAI(float Damage)
{
	Health = (Health - Damage) <= 0 ? 0.f : Health - Damage;
	this->BeingDamage();
	if(Health <= 0) 
	{
		IsDead = true;
		AIDead();
	}
}

