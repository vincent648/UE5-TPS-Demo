// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TPSPickupBase.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"

ATPSPickupBase::ATPSPickupBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")), true);


	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	Box->SetupAttachment(RootComponent);
	Box->InitBoxExtent(FVector(220.f, 220.f, 100.f));
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OutLineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutLine"));
	OutLineMesh->SetupAttachment(Mesh);
	OutLineMesh->SetCollisionProfileName(FName(TEXT("NoCollision")), true);


}

void ATPSPickupBase::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ATPSPickupBase::OverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &ATPSPickupBase::OverlapEnd);

	//武器掉落至地面
	FHitResult HitResult;
	FVector EndLocation = GetActorLocation();
	EndLocation.Z -= 1000.0f;
	FCollisionQueryParams Params;
	TArray<AActor*> Ignore;
	Ignore.Emplace(this);
	Ignore.Emplace(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	Params.AddIgnoredActors(Ignore);

	GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), EndLocation, ECollisionChannel::ECC_WorldStatic, Params);

	this->SetActorLocation(HitResult.Location);
}

void ATPSPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSPickupBase::InitPickup(EItemType TypeIn, FText NameIn, FText UI_PrefixIn, UStaticMesh* StaticMeshIn)
{
	Init(TypeIn, NameIn);
	UI_Prefix = UI_PrefixIn;
	Mesh->SetStaticMesh(StaticMeshIn);
}

void ATPSPickupBase::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CharacterRef == Cast<ATPSCharacter>(OtherActor)) OnBeginOverlap.Broadcast(this);
}

void ATPSPickupBase::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CharacterRef == Cast<ATPSCharacter>(OtherActor)) OnEndOverlap.Broadcast(this);
}

void ATPSPickupBase::EnableOutLine(bool Enable)
{
	Mesh->SetRenderCustomDepth(Enable);
}

int32 ATPSPickupBase::GetWeight() const
{
	 return 0;
}
