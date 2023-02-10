// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSItemWeapon.h"
#include "Types/DataTableStruct.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MatineeCameraShake.h"
#include "Character/TPSCharacter.h"
#include "Character/TPSPlayerController.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "AI/TPSAICharacter.h"

ATPSItemWeapon::ATPSItemWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent> (TEXT("SkeletalMesh"));
	Mesh->SetupAttachment(RootComponent);

	Mag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagMesh"));
	Mag->SetupAttachment(Mesh);
	Mag->SetCollisionProfileName(FName(TEXT("NoCollision")), true);

	Muule = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MuuleMesh"));
	Muule->SetupAttachment(Mesh);
	Muule->SetCollisionProfileName(FName(TEXT("NoCollision")), true);

	Sight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SightMesh"));
	Sight->SetupAttachment(Mesh);
	Sight->SetCollisionProfileName(FName(TEXT("NoCollision")), true);

	Foregrip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ForegripMesh"));
	Foregrip->SetupAttachment(Mesh);
	Foregrip->SetCollisionProfileName(FName(TEXT("NoCollision")), true);

	Buttstock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtstockMesh"));
	Buttstock->SetupAttachment(Mesh);
	Buttstock->SetCollisionProfileName(FName(TEXT("NoCollision")), true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Mesh);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFlash"));
	Particle->SetupAttachment(Mesh);
	Particle->bAutoActivate = false;
	Particle->SetRelativeScale3D(FVector(0.3f,0.3f,0.3f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("ParticleSystem'/Game/Particles/Particles/P_Weapon_Fired.P_Weapon_Fired'"));
	if (ParticleSystem.Succeeded()) {
		Particle->SetTemplate(ParticleSystem.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Misc/DT_Item_Weapon.DT_Item_Weapon'"));
	if (DataTable.Succeeded()) {
		DTItemWeapon = DataTable.Object;
	}

	AutoCameraShake = LoadClass<UMatineeCameraShake>(GetWorld(), TEXT("/Game/Blueprint/Camera/AutoCameraShake.AutoCameraShake_C"));
	SingleCameraShake = LoadClass<UMatineeCameraShake>(GetWorld(), TEXT("/Game/Blueprint/Camera/SingleCameraShake.SingleCameraShake_C"));

// 	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
// 
// 	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curvy(TEXT("CurveFloat'/Game/Misc/FC_CameraShake.FC_CameraShake'"));
// 	if (Curvy.Object) {
// 		FCurve = Curvy.Object;
// 	}

	/*TimelineEvent.BindUFunction(this, FName(TEXT("AddControllerInput")));*/

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Others/Materials/M_BulletHole.M_BulletHole'"));
	if (Material.Succeeded()) {
		BulletHole = Material.Object;
	}
	
	this->SetActorEnableCollision(false);
}

void ATPSItemWeapon::BeginPlay()
{
	Super::BeginPlay();

	FString ContextStr;
	if (DTItemWeapon)
	{
		FItemWeapon* Weapon = DTItemWeapon->FindRow<FItemWeapon>(ID, ContextStr, true);
		if (Weapon != nullptr)
		{
			Data = *Weapon;
			Init(EItemType::E_Weapon, Data.Name);
			Mesh->SetSkeletalMesh(Data.SkeletalMesh, true);
			InitDefaultWeapon();
		}
	}

	Player = Cast<ATPSCharacter> (UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	PlayerController = Cast<ATPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	if(!IsOwnByPlayer()) AI = Cast<ATPSAICharacter> (CharacterOwner);
}

void ATPSItemWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSItemWeapon::InitDefaultWeapon()
{
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepWorld, true);
	AttachmentTransformRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachmentTransformRules.RotationRule = EAttachmentRule::KeepWorld;
	AttachmentTransformRules.ScaleRule = EAttachmentRule::KeepRelative;

	if (Data.DefaultMag != nullptr)
	{
		Mag->SetStaticMesh(Data.DefaultMag);
		Mag->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Mag")));
	}

	Muule->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Muzzle")));

	if (Data.DefaultSight != nullptr)
	{
		Sight->SetStaticMesh(Data.DefaultSight);
		Sight->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Sight_Default")));
	}

	if (Data.CanGrip)
	{
		Foregrip->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Foregrip")));
	}

	if (Data.DefaultButtStock != nullptr)
	{
		Buttstock->SetStaticMesh(Data.DefaultButtStock);
		Buttstock->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Buttstock")));
	}

	Particle->AttachToComponent(Mesh, AttachmentTransformRules, FName(TEXT("Socket_Muzzle")));

	if(Data.AutoMode) IsAuto = true;
}

void ATPSItemWeapon::PlayFireSound()
{
	Audio->SetSound(Data.FireSound);
	Audio->bCanPlayMultipleInstances = true;
	Audio->Play();
}

void ATPSItemWeapon::PlayFireFlash()
{
	Particle->SetActive(true,true);
}

void ATPSItemWeapon::SwitchShootMode()
{
	if(Data.AutoMode) IsAuto = !IsAuto;
}

void ATPSItemWeapon::PressFire()
{
	if (IsAuto)
	{
		GetWorld()->GetTimerManager().SetTimer(FireHandle, this, &ATPSItemWeapon::Fire, Data.FiringInterval, true);
	}
	else
	{
		Fire();
	}
}

void ATPSItemWeapon::ReleaseFire()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(FireHandle)) GetWorld()->GetTimerManager().ClearTimer(FireHandle);

}

void ATPSItemWeapon::Fire()
{
	if (Ammo > 0)
	{
		
		if (IsOwnByPlayer())
		{
			if (Player->IsDead) return;
		}
		else
		{
			if (AI->IsDead) return;
		}
		
		PlayFireSound();
		PlayFireFlash();
		Ammo --;
		FireLineTrace();

		if (IsOwnByPlayer())
		{
			PlayCameraShake();
			Player->PlayerPlayMontage(EMontageType::E_Fire);
		}
		else if(!AI->IsDead)
		{
			AI->PlayAIMontage(EMontageType::E_Fire);
		}
			

		if(Ammo == 0 && IsOwnByPlayer())
		{
			ReleaseFire();
			ReloadClip();
		}
	}
}

void ATPSItemWeapon::ReloadClip()
{
	if (Ammo < Data.ClipSize)
	{
		if (IsOwnByPlayer())
		{
			Player->PlayerPlayMontage(EMontageType::E_Reload, Data.ReplaceClipTime);
		}
		else if(!AI->IsDead)
		{
			AI->PlayAIMontage(EMontageType::E_Reload, Data.ReplaceClipTime);
		}
		ReleaseFire();
	}
}

void ATPSItemWeapon::PlayCameraShake()
{
	if(Data.AutoMode) UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->StartCameraShake(AutoCameraShake, 1.f, ECameraShakePlaySpace::CameraLocal);
	else UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(SingleCameraShake, 1.f, ECameraShakePlaySpace::CameraLocal);
	
// 	AddYaw = Data.HorizontalOffset * (FMath::RandBool() ? -1 : 1);
// 	AddPitch = Data.VerticalOffset;
// 	TimelineLength = Data.FiringInterval > 0 ? Data.FiringInterval * 0.9f : 0.1f;
// 	TimelineComp->SetTimelineLength(TimelineLength);
// 	TimelineComp->PlayFromStart();
}

// void ATPSItemWeapon::AddControllerInput(float Val)
// {
// 	Percent = Val / TimelineLength;
// 	PlayerController->AddPitchInput(AddPitch * (Percent - AddedPer));
// 	PlayerController->AddYawInput(AddYaw * (Percent - AddedPer));
// 	AddedPer = Percent;
// }

void ATPSItemWeapon::FireLineTrace()
{
	FHitResult HitResult;
	FVector StartLocation, EndLocation;
	if (IsOwnByPlayer())
	{
		FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
		FVector WorldLocation, WorldDirection;
		UGameplayStatics::DeprojectScreenToWorld(Cast<APlayerController>(PlayerController),
			ViewportCenter, WorldLocation, WorldDirection);
		ATPSPickupBase* ShortestItem = nullptr;
		StartLocation = WorldLocation;
		EndLocation = StartLocation + WorldDirection * 5000.f;
	}
	else
	{
		StartLocation = this->Muule->GetComponentLocation();
		EndLocation = Player->GetActorLocation() + (Player->GetActorLocation() - StartLocation) * 1000.f;
	}
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CharacterOwner);
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, CollisionObjectQueryParams, Params);
	if (HitResult.GetActor() != nullptr)
	{
		if (Cast<ATPSCharacter>(HitResult.GetActor()) != nullptr)
		{
			Cast<ATPSCharacter>(HitResult.GetActor())->DamagePlayer(Data.Damage / 10.f);
		}
		else if (Cast<ATPSAICharacter>(HitResult.GetActor()) != nullptr)
		{
			if(!IsOwnByPlayer()) return; // AI打AI
			Cast<ATPSAICharacter>(HitResult.GetActor())->DamageAI(Data.Damage);
		}
		else
		{
			UDecalComponent* DecalBulletHole = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHole,
				FVector(6.f, 6.f, 6.f), HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), 10.f);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.ImpactNormal.Rotation().ToString());
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);
			DecalBulletHole->SetFadeScreenSize(0.001f);
			DecalBulletHole->FadeInStartDelay = 30.f;
			DecalBulletHole->FadeDuration = 3.f;
		}
	}
}

bool ATPSItemWeapon::IsOwnByPlayer()
{
	if(Cast<ATPSCharacter>(CharacterOwner) != nullptr) return true;
	else return false;
}
