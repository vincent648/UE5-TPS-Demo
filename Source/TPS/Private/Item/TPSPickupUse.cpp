// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSPickupUse.h"
#include "Types/DataTableStruct.h"

ATPSPickupUse::ATPSPickupUse()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Use.DT_Item_Use'"));
	if (DataTable.Succeeded()) {
		DTItemUse = DataTable.Object;
	}
}

void ATPSPickupUse::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	FName UI_Prefix_Name = FName(TEXT("PICK UP"));
	if (DTItemUse)
	{
		FItemUse* Use = DTItemUse->FindRow<FItemUse>(this->ID, ContextStr, true);
		if (Use != nullptr)
		{
			Data = *Use;
			SingleWeight = Data.Weight;
			InitPickup(GetItemTypeByUse(), Data.Name, FText::FromName(UI_Prefix_Name), Data.StaticMesh);
		}
	}
}

void ATPSPickupUse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EItemType ATPSPickupUse::GetItemTypeByUse()
{
	if(Data.Type == EItemUse::E_Health) return EItemType::E_Health;
	else return EItemType::E_Boost;
}

int32 ATPSPickupUse::GetWeight() const
{
	return SingleWeight * Amount;
}
