// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn/TPSSpawnPickupBase.h"
#include "Components/BoxComponent.h"
#include "Item/TPSPickupBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TPSCharacter.h"

// Sets default values
ATPSSpawnPickupBase::ATPSSpawnPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent> (TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent> (TEXT("Collision"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(50.f,100.f,50.f),true);

}

// Called when the game starts or when spawned
void ATPSSpawnPickupBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATPSSpawnPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ATPSPickupBase* ATPSSpawnPickupBase::Spawn()
{
	if(Player != nullptr)
		return Player->SpawnPickupByType(Type, GetActorLocation(), GetActorRotation(), Ammo, ID, SN, Amount, nullptr);
	else return nullptr;
}

