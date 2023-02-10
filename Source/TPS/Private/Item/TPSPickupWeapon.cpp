// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSPickupWeapon.h"
#include "Types/DataTableStruct.h"

ATPSPickupWeapon::ATPSPickupWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Weapon.DT_Item_Weapon'"));
	if (DataTable.Succeeded()) {
		DTItemWeapon = DataTable.Object;
	}
}

void ATPSPickupWeapon::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	FName UI_Prefix_Name = FName(TEXT("EQUIP"));
	if (DTItemWeapon)
	{
		FItemWeapon* Weapon = DTItemWeapon->FindRow<FItemWeapon>(this->ID, ContextStr, true);
		if (Weapon != nullptr)
		{
			Data = *Weapon;
			InitPickup(EItemType::E_Weapon, Data.Name, FText::FromName(UI_Prefix_Name), Data.StaticMesh);
		}
	}
}

void ATPSPickupWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
