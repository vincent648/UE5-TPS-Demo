// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSItemBase.h"
#include "Character/TPSCharacter.h"
#include "Character/TPSPlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATPSItemBase::ATPSItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void ATPSItemBase::BeginPlay()
{
	Super::BeginPlay();

	ATPSCharacter* CharacterTemp =  Cast<ATPSCharacter> (UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if(CharacterTemp != nullptr) CharacterRef = CharacterTemp;

	ATPSPlayerController* ControllerTemp = Cast<ATPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(ControllerTemp != nullptr) ControllerRef = ControllerTemp;
	
}

// Called every frame
void ATPSItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSItemBase::Init(EItemType ItemTypeIn, FText NameIn)
{
	ItemType = ItemTypeIn;
	Name = NameIn;
}

int32 ATPSItemBase::GetWeight() const
{
	return 0;
}

void ATPSItemBase::UpdateAmount(bool IsAdd, bool IsReduce, int32 AmountIn)
{
	if(IsAdd) Amount += AmountIn;
	else if(IsReduce) Amount -= AmountIn;
	else Amount = AmountIn;
}

