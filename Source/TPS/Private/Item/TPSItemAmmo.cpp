// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSItemAmmo.h"
#include "Types/DataTableStruct.h"

ATPSItemAmmo::ATPSItemAmmo()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Ammo.DT_Item_Ammo'"));
	if (DataTable.Succeeded()) {
		DTItemAmmo = DataTable.Object;
	}
}

void ATPSItemAmmo::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	if (DTItemAmmo)
	{
		FItemAmmo* Ammo = DTItemAmmo->FindRow<FItemAmmo>(ID, ContextStr, true);
		if (Ammo != nullptr)
		{
			Data = *Ammo;
			Init(EItemType::E_Ammo, Data.Name);
		}
	}
}

void ATPSItemAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ATPSItemAmmo::GetWeight() const
{
	return Amount * Data.Weight;
}
