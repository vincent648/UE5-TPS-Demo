// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPSPlayerState.h"

ATPSPlayerState::ATPSPlayerState()
{

}

void ATPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSPlayerState::SetWeapon1(ATPSItemWeapon* Weapon)
{
	Weapon1 = Weapon;
	OnWeaponChange.Broadcast(Weapon1, FWeaponPosition(EWeaponPosition::E_Right, false));
}

void ATPSPlayerState::SetWeapon2(ATPSItemWeapon* Weapon)
{
	Weapon2 = Weapon;
	OnWeaponChange.Broadcast(Weapon2, FWeaponPosition(EWeaponPosition::E_Right, false));
}

void ATPSPlayerState::SetHoldGun(ATPSItemWeapon* Weapon)
{
	HoldGun = Weapon;
	if (HoldGun != nullptr) OnWeaponChange.Broadcast(HoldGun, FWeaponPosition(HoldGun->Position, true));
	else OnWeaponChange.Broadcast(HoldGun, FWeaponPosition(EWeaponPosition::E_Left, true));
}

void ATPSPlayerState::SetAmmo556(int32 Ammo556In)
{
	Ammo556 = Ammo556In;
	OnAmmoChange.Broadcast();
}

void ATPSPlayerState::SetHealthPoint(float HealthPointIn)
{
	HealthPoint = HealthPointIn;
	OnHealthPointChange.Broadcast(HealthPoint);
}

void ATPSPlayerState::AddItem(ATPSItemBase* Item)
{
	if (Item->ItemType == EItemType::E_Boost || Item->ItemType == EItemType::E_Health)
	{
		for (auto& BackpackItem : ItemsInBackpack)
		{
			if (BackpackItem->ItemType == Item->ItemType && BackpackItem->ID == Item->ID)
			{
				BackpackItem->Amount += Item->Amount;
				OnItemsChange.Broadcast(BackpackItem, true);
				return;
			}
		}
	}

	ItemsInBackpack.Emplace(Item);
	OnItemsChange.Broadcast(Item, true);
}

void ATPSPlayerState::RemoveItem(ATPSItemBase* Item)
{
	ItemsInBackpack.Remove(Item);
	OnItemsChange.Broadcast(Item, false);
}
