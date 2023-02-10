// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSItemUse.h"
#include "Types/DataTableStruct.h"

ATPSItemUse::ATPSItemUse()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Use.DT_Item_Use'"));
	if (DataTable.Succeeded()) {
		DTItemUse = DataTable.Object;
	}
	this->SetActorEnableCollision(false);
}

void ATPSItemUse::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	if (DTItemUse)
	{
		FItemUse* Use = DTItemUse->FindRow<FItemUse>(ID, ContextStr, true);
		if (Use != nullptr)
		{
			Data = *Use;
			Init(GetItemTypeByUse(), Data.Name);
			UseTime = Data.UseTime;
		}
	}
}

void ATPSItemUse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ATPSItemUse::GetWeight() const
{
	return Amount * Data.Weight;
}

EItemType ATPSItemUse::GetItemTypeByUse()
{
	if (Data.Type == EItemUse::E_Health) return EItemType::E_Health;
	else return EItemType::E_Boost;
}
