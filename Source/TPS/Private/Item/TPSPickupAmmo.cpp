// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSPickupAmmo.h"
#include "Types/DataTableStruct.h"

ATPSPickupAmmo::ATPSPickupAmmo()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Ammo.DT_Item_Ammo'"));
	if (DataTable.Succeeded()) {
		DTItemAmmo = DataTable.Object;
	}
}

void ATPSPickupAmmo::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	FName UI_Prefix_Name = FName(TEXT("PICK UP"));
	if (DTItemAmmo)
	{
		FItemAmmo* Ammo = DTItemAmmo->FindRow<FItemAmmo>(this->ID, ContextStr, true);
		if (Ammo != nullptr)
		{
			Data = *Ammo;
			if (Amount == 0) Amount = Data.PickupAmmo;
			SingleWeight = Data.Weight;
			InitPickup(EItemType::E_Ammo, Data.Name, FText::FromName(UI_Prefix_Name), Data.PickupMesh);
		}
	}
}

void ATPSPickupAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ATPSPickupAmmo::GetWeight() const
{
	return SingleWeight * Amount;
}
