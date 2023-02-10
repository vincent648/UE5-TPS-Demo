// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/TPSPickupBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/TPSPickupWeapon.h"
#include "Item/TPSPickupUse.h"
#include "Character/TPSPlayerState.h"
#include "Core/TPSGameInstance.h"
#include "Item/TPSItemUse.h"
#include "Character/TPSPlayerController.h"
#include "Components/UIManagementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimWait);

//////////////////////////////////////////////////////////////////////////
// ATPSCharacter

ATPSCharacter::ATPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curvy(TEXT("CurveFloat'/Game/Misc/FC_FreeView.FC_FreeView'"));
	if (Curvy.Object) {
		FCurve = Curvy.Object;
	}

	TimelineEvent.BindUFunction(this, FName(TEXT("AltSetControlRotation")));

	TimelineFinishedEvent.BindUFunction(this, FName(TEXT("AltFinish")));
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	TimelineComp->AddInterpFloat(FCurve, TimelineEvent, FName(TEXT("TimelineFloat")));
	TimelineComp->SetTimelineLength(0.3f);
	TimelineComp->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	TimelineComp->SetTimelineFinishedFunc(TimelineFinishedEvent);

	TPSPlayerState = Cast<ATPSPlayerState> (GetPlayerState());
	TPSGameInstance = Cast<UTPSGameInstance> (GetGameInstance());
	PlayerController = Cast<ATPSPlayerController> (GetController());

	if(TPSPlayerState != nullptr) TPSPlayerState->OnWeaponChange.AddDynamic(this, &ATPSCharacter::EventWeaponChange);
	UnEquip.AddDynamic(this, &ATPSCharacter::EventOnUnEquip);
	OnEquip.AddDynamic(this, &ATPSCharacter::EventOnEquip);
	OnEquipEnd.AddDynamic(this, &ATPSCharacter::EventOnMontageEnd);
	UnEquipEnd.AddDynamic(this, &ATPSCharacter::EventOnMontageEnd);
	ReloadEnd.AddDynamic(this, &ATPSCharacter::EventOnMontageEnd);
	ReloadEnd.AddDynamic(this, &ATPSCharacter::EventOnReloadEnd);
	FireEnd.AddDynamic(this, &ATPSCharacter::EventOnMontageEnd);
	if(TPSPlayerState != nullptr) TPSPlayerState->OnItemsChange.AddDynamic(this, &ATPSCharacter::EventItemsChange);
	
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimDirection = GetAnimDirection();

	if (GEngine == nullptr || GetWorld() == nullptr) return;
	
	if (!IsFreeView)
	{
		FRotator NewRotation = this->GetActorRotation();
		NewRotation.Yaw = GetControlRotation().Yaw;
		this->SetActorRotation(NewRotation);
	}

	TargetPickupItem();
	
	if(IsProning && GetAnimSpeed() != 0.f) PlayerStopFire();
}



//////////////////////////////////////////////////////////////////////////
// Input

void ATPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ATPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATPSCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATPSCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATPSCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::PlayerCrouch);
	PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &ATPSCharacter::PlayerProne);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ATPSCharacter::PlayerWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ATPSCharacter::PlayerStopWalk);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ATPSCharacter::PlayerRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ATPSCharacter::PlayerStopRun);

	PlayerInputComponent->BindAction("Alt", IE_Pressed, this, &ATPSCharacter::PlayerFreeView);
	PlayerInputComponent->BindAction("Alt", IE_Released, this, &ATPSCharacter::PlayerStopFreeView);

	PlayerInputComponent->BindAction("DiscardWeapon", IE_Pressed, this, &ATPSCharacter::PlayerDiscardWeapon);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::PlayerAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSCharacter::PlayerStopAiming);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ATPSCharacter::PickupItem);

	PlayerInputComponent->BindAction("TakeBackGun", IE_Pressed, this, &ATPSCharacter::PlayerTakeBackGun);

	PlayerInputComponent->BindAction("KeyBoard1", IE_Pressed, this, &ATPSCharacter::PlayerKeyBoard1);
	PlayerInputComponent->BindAction("KeyBoard2", IE_Pressed, this, &ATPSCharacter::PlayerKeyBoard2);

	PlayerInputComponent->BindAction("ShootMode", IE_Pressed, this, &ATPSCharacter::PlayerChangeShootMode);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::PlayerFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::PlayerStopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATPSCharacter::PlayerReload);

}

void ATPSCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATPSCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (LimitCameraPitchAngle(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds()))
	{
		AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	}
		
}

void ATPSCharacter::MoveForward(float Value)
{
	MoveForwardValue = Value;
	AnimDirection = GetAnimDirection();
	UpdateWeaponDisplay(CalculateHoldOnSocket());
	if ((Controller != nullptr) && (Value != 0.0f) && EnableMove)
	{
		// find out which way is forward
		FRotator Rotation;
		if (IsFreeView) Rotation = AltPressRotation;
		else Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		UpdateWalkSpeed();
		SmoothIncrease();
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	MoveRightValue = Value;
	AnimDirection = GetAnimDirection();
	UpdateWeaponDisplay(CalculateHoldOnSocket());
	if ( (Controller != nullptr) && (Value != 0.0f) && EnableMove)
	{
		// find out which way is right
		FRotator Rotation;
		if(IsFreeView) Rotation = AltPressRotation;
		else Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		UpdateWalkSpeed();
		SmoothIncrease();
		AddMovementInput(Direction, Value);
	}
}

int32 ATPSCharacter::GetAnimOffsetType() const
{
	if (GetAnimIsInAir() || IsProning) return 0;
	else
	{
		if (IsCrouching)
		{
			if (IsHoldWeapon)
			{
				if (IsAiming) return 6;
				else return 4;
			}
			else return 2;
		}
		else
		{
			if (IsHoldWeapon)
			{
				if (IsAiming) return 5;
				else return 3;
			}
			else return 1;
		}	
	}
}

void ATPSCharacter::SmoothIncrease()
{
	if((GetVelocity().Size() + 5.0f) <= MaxWalkSpeed ) GetCharacterMovement()->MaxWalkSpeed = GetVelocity().Size() + 5.0f;
	else GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ATPSCharacter::UpdateWalkSpeed()
{
	FString MapKey = IsHoldWeapon ? FString(TEXT("true")) : FString(TEXT("false"));
	FString Posture, MoveState;

	if (IsCrouching) Posture = FString(TEXT("crouch"));
	else if(IsProning) Posture = FString(TEXT("prone"));
	else Posture = FString(TEXT("stand"));

	if(IsAiming) MoveState = FString(TEXT("aim"));
	else if(IsWalk) MoveState = FString(TEXT("walk"));
	else if(IsRun) MoveState = FString(TEXT("run"));
	else MoveState = FString(TEXT("jog"));

	MapKey.Append(FString(TEXT("_")));
	MapKey.Append(Posture);
	MapKey.Append(FString(TEXT("_")));
	MapKey.Append(MoveState);
	if (WalkSpeedMap.Find(MapKey) != nullptr)
	{
		if (MoveForwardValue > 0) MaxWalkSpeed = WalkSpeedMap[MapKey].Forward;
		else MaxWalkSpeed = WalkSpeedMap[MapKey].Other;
	}
	else if (TPSPlayerState->GetHoldGun() != nullptr)
	{
		MaxWalkSpeed = TPSPlayerState->GetHoldGun()->Data.WalkSpeedPer * MaxWalkSpeed;
	}
}

void ATPSCharacter::AnimWait(bool IsHoldWeaponValue, FString Current, FString Target)
{
	EnableMove = false;
	CanJump = false;
	FString MapKey = IsHoldWeaponValue ? FString(TEXT("true")) : FString(TEXT("false"));
	MapKey.Append(FString(TEXT("_")));
	MapKey.Append(Current);
	MapKey.Append(FString(TEXT("_")));
	MapKey.Append(Target);

	if(!AnimWaitTimeMap.Contains(MapKey))
	{
		UE_LOG(LogAnimWait,Error,TEXT("Cant Find Wait Time In Map !!!"));
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(AnimWaitHandle, this, &ATPSCharacter::AnimWaitEnd, AnimWaitTimeMap[MapKey], false);
}


void ATPSCharacter::AnimWaitEnd()
{
	EnableMove = true;
	CanJump = true;
	GetWorld()->GetTimerManager().ClearTimer(AnimWaitHandle);
}

void ATPSCharacter::AltSetControlRotation(float Val)
{
	GetController()->SetControlRotation(UKismetMathLibrary::RLerp(AltReleaseRotation, AltPressRotation, Val, true));
}

void ATPSCharacter::PlayerCrouch()
{
	if(IsProning) 
	{
		IsProning = false; 
		IsCrouching = true;
		AnimWait(IsHoldWeapon,FString(TEXT("prone")), FString(TEXT("crouch")));
	}
	else IsCrouching = !IsCrouching;
	LimitCameraPitchAngle(0.0f);
	UpdateWeaponDisplay(CalculateHoldOnSocket());
}

void ATPSCharacter::PlayerProne()
{
	if(IsProning) 
	{
		IsProning = false;
		PlayerStopFire();
		AnimWait(IsHoldWeapon, FString(TEXT("prone")), FString(TEXT("stand")));
	}
	else
	{
		if (IsCrouching || IsAiming)
		{
			IsCrouching = false;
			IsProning = true;
			IsAiming = false;
			PlayerStopFire();
			AnimWait(IsHoldWeapon, FString(TEXT("crouch")), FString(TEXT("prone")));

		}
		else 
		{
			IsProning = true;
			IsAiming = false;
			PlayerStopFire();
			AnimWait(IsHoldWeapon, FString(TEXT("stand")), FString(TEXT("prone")));
		}
	}
	LimitCameraPitchAngle(0.0f);
	UpdateWeaponDisplay(CalculateHoldOnSocket());
}

void ATPSCharacter::PlayerJump()
{
	if (IsProning)
	{
		IsProning = false;
		IsCrouching = true;
		PlayerStopFire();
		AnimWait(IsHoldWeapon, FString(TEXT("prone")), FString(TEXT("jump")));
	}
	else
	{
		if(IsCrouching) IsCrouching = false;
		else if(CanJump) 
		{
			PlayerStopFire();
			ACharacter::Jump();
		}
	}
}

void ATPSCharacter::PlayerFreeView()
{
	IsFreeView = true;
	IsAiming = false;
	PlayerStopFire();
	AltPressRotation = GetCameraBoom()->GetTargetRotation();
}

void ATPSCharacter::PlayerStopFreeView()
{
	AltReleaseRotation = GetCameraBoom()->GetTargetRotation();

	TimelineComp->PlayFromStart();
}

void ATPSCharacter::PlayerDiscardWeapon()
{
	if (!IsProning && !IsPlayingMontage)
	{
		if(TPSPlayerState->GetHoldGun() != nullptr) DiscardWeapon(TPSPlayerState->GetHoldGun());
		else if(TPSPlayerState->GetWeapon1() != nullptr) DiscardWeapon(TPSPlayerState->GetWeapon1());
		else if(TPSPlayerState->GetWeapon2() != nullptr) DiscardWeapon(TPSPlayerState->GetWeapon2());
	}
}

void ATPSCharacter::PlayerAiming()
{
	if(IsHoldWeapon && !IsProning) IsAiming = true;
	UpdateWeaponDisplay(CalculateHoldOnSocket());
}

void ATPSCharacter::PlayerStopAiming()
{
	IsAiming = false;
}

void ATPSCharacter::PlayerTakeBackGun()
{
	if(TPSPlayerState->GetHoldGun() != nullptr && !IsPlayingMontage) 
	{
		IsAiming = false;
		PlayerStopFire();
		PlayerPlayMontage(EMontageType::E_UnEquip);
	}
}

void ATPSCharacter::PlayerKeyBoard1()
{
	if (!IsPlayingMontage)
	{
		ReadyEquipWeapon = TPSPlayerState->GetWeapon1();
		if (ReadyEquipWeapon != nullptr)
		{
			IsAiming = false;
			if (TPSPlayerState->GetHoldGun() != nullptr)
			{
				PlayerStopFire();
				PlayerPlayMontage(EMontageType::E_UnEquip);
			}
			else PlayerPlayMontage(EMontageType::E_Equip);
		}
	}
}

void ATPSCharacter::PlayerKeyBoard2()
{
	if (!IsPlayingMontage)
	{
		ReadyEquipWeapon = TPSPlayerState->GetWeapon2();
		if (ReadyEquipWeapon != nullptr)
		{
			IsAiming = false;
			if (TPSPlayerState->GetHoldGun() != nullptr)
			{
				PlayerStopFire();
				PlayerPlayMontage(EMontageType::E_UnEquip);
			}
			else PlayerPlayMontage(EMontageType::E_Equip);
		}
	}
}

void ATPSCharacter::PlayerFire()
{
	if (TPSPlayerState->GetHoldGun() != nullptr && EnableMove && !IsFreeView 
	&& !GetAnimIsInAir() && ((IsProning && GetAnimSpeed() == 0.f) || (IsAiming && !IsProning)))
	{
		if (!IsPlayingMontage || PlayingMontageType == EMontageType::E_Fire)
		{
			TPSPlayerState->GetHoldGun()->PressFire();

		}
	}
}

void ATPSCharacter::PlayerStopFire()
{
	if(TPSPlayerState->GetHoldGun() != nullptr) TPSPlayerState->GetHoldGun()->ReleaseFire();
}

void ATPSCharacter::PlayerReload()
{
	if (TPSPlayerState->GetHoldGun() != nullptr && EnableMove && !IsFreeView
		&& !GetAnimIsInAir())
	{
		if (!IsPlayingMontage || PlayingMontageType == EMontageType::E_Fire)
		{
			IsAiming = false;
			PlayerStopFire();
			TPSPlayerState->GetHoldGun()->ReloadClip();
		}
	}
}



void ATPSCharacter::PlayerChangeShootMode()
{
	if(TPSPlayerState->GetHoldGun() != nullptr && !IsPlayingMontage) TPSPlayerState->GetHoldGun()->SwitchShootMode();
}

bool ATPSCharacter::LimitCameraPitchAngle(float Val)
{
	float BottomValue, TopValue;
	if (IsProning)
	{
		BottomValue = 350.0f;
		TopValue = 10.0f;
	}
	else if(IsCrouching)
	{
		BottomValue = 290.0f;
		TopValue = 40.0f;
	}
	else
	{
		BottomValue = 290.0f;
		TopValue = 60.0f;
	}

	float NewControllerPitch = GetControlRotation().Pitch + Val * Cast<APlayerController>(GetController())->InputPitchScale_DEPRECATED;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::SanitizeFloat(NewControllerPitch));
	FRotator NewRotaion = GetControlRotation();

	if(NewControllerPitch > BottomValue || NewControllerPitch < TopValue) return true;
	else
	{
		if (abs(NewControllerPitch - TopValue) < abs(NewControllerPitch - BottomValue))
		{
			NewRotaion.Pitch = TopValue;
			GetController()->SetControlRotation(NewRotaion);
		}
		else
		{
			NewRotaion.Pitch = BottomValue;
			GetController()->SetControlRotation(NewRotaion);
		}
		return false;
	}
}

void ATPSCharacter::UpdateWeaponDisplay(FName HoldSocket)
{
	if(TPSPlayerState == nullptr || TPSGameInstance == nullptr || HoldSocket == FName(TEXT("None"))) return;
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
	if (TPSPlayerState->GetHoldGun() != nullptr)
	{
		if (HoldSocket != FName(TEXT("None")))
		{
			TPSPlayerState->GetHoldGun()->SetActorTransform(GetMesh()->GetSocketTransform(HoldSocket, ERelativeTransformSpace::RTS_World));
			TPSPlayerState->GetHoldGun()->AttachToComponent(GetMesh(), AttachmentTransformRules, HoldSocket);
		}
	}
	if (TPSPlayerState->GetWeapon1() != nullptr)
	{
		TPSPlayerState->GetWeapon1()->SetActorTransform(GetMesh()->GetSocketTransform(TPSGameInstance->BackLeftNName, ERelativeTransformSpace::RTS_World));
		TPSPlayerState->GetWeapon1()->AttachToComponent(GetMesh(), AttachmentTransformRules, TPSGameInstance->BackLeftNName);
	}
	if (TPSPlayerState->GetWeapon2() != nullptr)
	{
		TPSPlayerState->GetWeapon2()->SetActorTransform(GetMesh()->GetSocketTransform(TPSGameInstance->BackRightNName, ERelativeTransformSpace::RTS_World));
		TPSPlayerState->GetWeapon2()->AttachToComponent(GetMesh(), AttachmentTransformRules, TPSGameInstance->BackRightNName);
	}
}

void ATPSCharacter::TargetPickupItem()
{
	FHitResult HitResult;
	if(GEngine->GameViewport == nullptr || GEngine->GameViewport->Viewport == nullptr) return;
	FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector WorldLocation, WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(Cast<APlayerController> (GetController()), 
	ViewportCenter, WorldLocation, WorldDirection);
	ATPSPickupBase* ShortestItem = nullptr;
	FVector StartLocation = WorldLocation;
	FVector EndLocation = StartLocation + WorldDirection * 5000.f;
	if (ItemsInRange.Num() > 0)
	{
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, Params);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1, 0, 1.f);
		float ShortestDistance = 10000.f;
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.Location.ToString());
		for (auto& Item : ItemsInRange)
		{
			Item->EnableOutLine(false);
			if (FVector::Distance(HitResult.Location, Item->GetActorLocation()) < ShortestDistance)
			{
				ShortestDistance = FVector::Distance(HitResult.Location, Item->GetActorLocation());
				ShortestItem = Item;
			}
		}

		if(ShortestItem != nullptr) 
		{
			if (ShortestDistance < 65.f)
			{
				ShortestItem->EnableOutLine(true);
				ReadyPickupItem = ShortestItem;
			}
			else ReadyPickupItem = nullptr;
		}
	}
}

FWeaponPosition ATPSCharacter::AutoPosition()
{
	if (TPSPlayerState == nullptr) return FWeaponPosition();

	int32 CurrentAmount = 0;

	if (TPSPlayerState->GetWeapon1() != nullptr) CurrentAmount++;
	if (TPSPlayerState->GetWeapon2() != nullptr) CurrentAmount++;
	if (TPSPlayerState->GetHoldGun() != nullptr) CurrentAmount++;

	if (CurrentAmount == 0) return FWeaponPosition(EWeaponPosition::E_Left, true);

	if (CurrentAmount < 2)
	{
		if (TPSPlayerState->GetHoldGun() != nullptr)
		{
			switch (TPSPlayerState->GetHoldGun()->Position)
			{
			case EWeaponPosition::E_Left: return FWeaponPosition(EWeaponPosition::E_Right, false);

			case EWeaponPosition::E_Right: return FWeaponPosition(EWeaponPosition::E_Left, false);

			}
		}
		else if (TPSPlayerState->GetWeapon1() != nullptr) return FWeaponPosition(EWeaponPosition::E_Right, true);
		else if (TPSPlayerState->GetWeapon1() == nullptr) return FWeaponPosition(EWeaponPosition::E_Left, true);
	}

	if (CurrentAmount >= 2)
	{
		if (TPSPlayerState->GetHoldGun() != nullptr)
		{
			switch (TPSPlayerState->GetHoldGun()->Position)
			{
			case EWeaponPosition::E_Left: return FWeaponPosition(EWeaponPosition::E_Right, true);

			case EWeaponPosition::E_Right: return FWeaponPosition(EWeaponPosition::E_Left, true);
			}
		}
		else return FWeaponPosition(EWeaponPosition::E_Left, false);
	}

	return FWeaponPosition();
}

FWeaponPosition ATPSCharacter::AssignPosition(EWeaponPosition Assign)
{
	if(TPSPlayerState == nullptr) return FWeaponPosition();

	if (TPSPlayerState->GetHoldGun() != nullptr)
	{
		if (TPSPlayerState->GetHoldGun()->Position == Assign) return FWeaponPosition(Assign, true);
	}
	
	return FWeaponPosition(Assign, false);
}

ATPSPickupBase* ATPSCharacter::SpawnPickupItem(ATPSItemBase* Item)
{
	return SpawnPickupByType(Item->ItemType, GetActorLocation(), GetActorRotation(), 0, FName(""), FString(""), 0, Item);
}

void ATPSCharacter::SetPickupItem(ATPSPickupBase* Item)
{
	Item->OnBeginOverlap.AddDynamic(this, &ATPSCharacter::ExecBeginOverlap);
	Item->OnEndOverlap.AddDynamic(this, &ATPSCharacter::ExecEndOverlap);
}

void ATPSCharacter::ExecBeginOverlap(ATPSPickupBase* Item)
{
	this->ItemsInRange.Emplace(Item);
}

void ATPSCharacter::ExecEndOverlap(ATPSPickupBase* Item)
{
	Item->EnableOutLine(false);
	this->ItemsInRange.Remove(Item);
}

ATPSPickupBase* ATPSCharacter::SpawnPickupByType(EItemType Type, FVector Location, FRotator Rotation, int32 Ammo, FName ID, FString SN, int32 Amount, ATPSItemBase* Item)
{
	if(GetWorld() == nullptr) return nullptr;
	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(FQuat(Rotation));
	ATPSPickupBase* PickupItem = nullptr;
	switch (Type)
	{
	case EItemType::E_Weapon:
	{
		ATPSPickupWeapon* SpawnWeapon = GetWorld()->SpawnActorDeferred<ATPSPickupWeapon>(ATPSPickupWeapon::StaticClass(), Transform);
		PickupItem = Cast<ATPSPickupBase> (SpawnWeapon);
		if (Item != nullptr)
		{
			ATPSItemWeapon* Weapon = Cast<ATPSItemWeapon>(Item);
			SpawnWeapon->Ammo = Weapon->Ammo;
		}
		else SpawnWeapon->Ammo = Ammo;
		break;
	}
	case EItemType::E_Health: 
	case EItemType::E_Boost:
	{
		ATPSPickupUse* SpawnUse = GetWorld()->SpawnActorDeferred<ATPSPickupUse>(ATPSPickupUse::StaticClass(), Transform);
		PickupItem = Cast<ATPSPickupBase>(SpawnUse);
		PickupItem->GetMesh()->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
		break;
	}
	default: return nullptr;
	}

	if (Item != nullptr)
	{
		PickupItem->ID = Item->ID;
		PickupItem->SN = Item->SN;
		PickupItem->Amount = Item->Amount;
	}
	else
	{
		PickupItem->ID = ID;
		PickupItem->SN = SN;
		PickupItem->Amount = Amount;
	}

	PickupItem->FinishSpawning(Transform);
	SetPickupItem(PickupItem);
	return PickupItem;
}

ATPSItemWeapon* ATPSCharacter::SpawnItemWeapon(ATPSPickupWeapon* Weapon, FWeaponPosition WeaponPosition)
{
	FTransform Transform;
	ATPSItemWeapon* SpawnWeapon = GetWorld()->SpawnActorDeferred<ATPSItemWeapon>(ATPSItemWeapon::StaticClass(), Transform);
	SpawnWeapon->Ammo = Weapon->Ammo;
	SpawnWeapon->Position = WeaponPosition.Position;
	SpawnWeapon->IsOnHand = WeaponPosition.IsOnHand;
	SpawnWeapon->ID = Weapon->ID;
	SpawnWeapon->SN = Weapon->SN;
	SpawnWeapon->CharacterOwner = this;
	SpawnWeapon->FinishSpawning(Transform);
	return SpawnWeapon;
}


void ATPSCharacter::PickupWeapon(ATPSPickupWeapon* Weapon, bool IsAssign, EWeaponPosition Position)
{	if (TPSPlayerState == nullptr) return;
	FWeaponPosition WeaponPosition;
	ATPSItemWeapon* ReplaceWeapon = nullptr;

	if(IsAssign) WeaponPosition = AssignPosition(Position);
	else WeaponPosition = AutoPosition();

	if (WeaponPosition.IsOnHand)
	{
		if(TPSPlayerState->GetHoldGun() != nullptr) ReplaceWeapon = TPSPlayerState->GetHoldGun();
	}
	else
	{
		switch (WeaponPosition.Position)
		{
			case EWeaponPosition::E_Left:
			{
				if(TPSPlayerState->GetWeapon1() != nullptr) ReplaceWeapon = TPSPlayerState->GetWeapon1();
				break;
			}
			case EWeaponPosition::E_Right:
			{
				if (TPSPlayerState->GetWeapon2() != nullptr) ReplaceWeapon = TPSPlayerState->GetWeapon2();
				break;
			}
		}
	}

	if (ReplaceWeapon != nullptr) 
	{
		WeaponPosition.Position = ReplaceWeapon->Position;
		WeaponPosition.IsOnHand = ReplaceWeapon->IsOnHand;
		DiscardWeapon(ReplaceWeapon);
	}

	ATPSItemWeapon* NewWeapon = SpawnItemWeapon(Weapon, WeaponPosition);

	if (WeaponPosition.IsOnHand) TPSPlayerState->SetHoldGun(NewWeapon);
	else
	{
		switch (WeaponPosition.Position)
		{
		case EWeaponPosition::E_Left: { TPSPlayerState->SetWeapon1(NewWeapon); break; }

		case EWeaponPosition::E_Right: { TPSPlayerState->SetWeapon2(NewWeapon); break; }
		}
	}
}

void ATPSCharacter::DiscardWeapon(ATPSItemWeapon* Weapon)
{
	ATPSPickupBase* Pickup = SpawnPickupItem(Weapon);
	IsAiming = false;
	if(ItemsInRange.Find(Pickup) == -1) ItemsInRange.Emplace(Pickup);

	if(Weapon->IsOnHand) TPSPlayerState->SetHoldGun(nullptr);
	else
	{
		switch (Weapon->Position)
		{
		case EWeaponPosition::E_Left: { TPSPlayerState->SetWeapon1(nullptr); break;}

		case EWeaponPosition::E_Right: { TPSPlayerState->SetWeapon2(nullptr); break;}
		}
	}

	Weapon->Destroy();
}

FName ATPSCharacter::CalculateHoldOnSocket()
{
	FName GunSocket = FName(TEXT("None"));
	if(TPSGameInstance == nullptr || TPSPlayerState == nullptr) return GunSocket;
	if (IsProning)
	{
		if (MoveForwardValue == 0.f && MoveRightValue == 0.f) GunSocket = TPSGameInstance->GunProneIdleName;
		else if(MoveRightValue == 0.f) GunSocket = TPSGameInstance->GunProneFBName;
		else GunSocket = TPSGameInstance->GunProneOtherName;
	}
	else if (IsCrouching && !IsAiming) GunSocket = TPSGameInstance->GunCrouchName;
	else if (IsAiming || IsFiring) GunSocket = TPSGameInstance->GunAimName;
	else if(IsReload) GunSocket = TPSGameInstance->GunReloadName;
	else GunSocket = TPSGameInstance->GunStandName;

	return GunSocket;
}

void ATPSCharacter::EventWeaponChange(ATPSItemWeapon* Weapon, FWeaponPosition WeaponPosition)
{
	UpdateWeaponDisplay(CalculateHoldOnSocket());
	UpdatePlayerGunState();
}

void ATPSCharacter::PickupItem()
{
	if (ReadyPickupItem != nullptr && !IsPlayingMontage)
	{
		switch (ReadyPickupItem->ItemType)
		{
			case EItemType::E_Weapon :
			{
				PickupWeapon(Cast<ATPSPickupWeapon>(ReadyPickupItem), false, EWeaponPosition::E_Left);
				ItemsInRange.Remove(ReadyPickupItem);
				ReadyPickupItem->Destroy();
				ReadyPickupItem = nullptr;
				break;
			}
			case EItemType::E_Health:
			case  EItemType::E_Boost:
			{
				if (PickupUse(Cast<ATPSPickupUse>(ReadyPickupItem)))
				{
					ItemsInRange.Remove(ReadyPickupItem);
					ReadyPickupItem->Destroy();
					ReadyPickupItem = nullptr;
				}
				break;
			}
			default: return;
		}
	}
}

void ATPSCharacter::UpdatePlayerGunState()
{
	if(TPSPlayerState->GetHoldGun() != nullptr) IsHoldWeapon = true;
	else IsHoldWeapon = false;
}

bool ATPSCharacter::PickupUse(ATPSPickupUse* Item)
{
	if (TPSPlayerState == nullptr) return false;

	switch (Item->ItemType)
	{
		case EItemType::E_Health: 
		case EItemType::E_Boost: 
		{
			ATPSItemUse* SpawnItem = SpawnItemUse(Item);
			if (SpawnItem != nullptr)
			{
				TPSPlayerState->AddItem(SpawnItem);
				ReadyPickupItem->Destroy();
				return true;
			}
			return false;
		}
		default: return false;
	}
	return false;
}

void ATPSCharacter::DiscardUse(ATPSItemUse* Use)
{
	if(Use == nullptr) return;
	ATPSPickupBase* SpawnItem = SpawnPickupItem(Use);
	if (SpawnItem != nullptr)
	{
		TPSPlayerState->RemoveItem(Use);
		if (ItemsInRange.Find(SpawnItem) == -1) ItemsInRange.Emplace(SpawnItem);
		Use->Destroy();
	}
}

ATPSItemUse* ATPSCharacter::SpawnItemUse(ATPSPickupUse* Item)
{
	FTransform Transform;
	ATPSItemUse* SpawnItem = GetWorld()->SpawnActorDeferred<ATPSItemUse>(ATPSItemUse::StaticClass(), Transform);
	SpawnItem->ID = Item->ID;
	SpawnItem->SN = Item->SN;
	SpawnItem->Amount = Item->Amount;
	SpawnItem->FinishSpawning(Transform);
	return SpawnItem;
}

void ATPSCharacter::EventItemsChange(ATPSItemBase* Item, bool IsAdd)
{
	
}

void ATPSCharacter::EventOnUnEquip()
{	
	IsHoldWeapon = false;
	IsAiming = false;
	TPSPlayerState->GetHoldGun()->IsOnHand = false;

	switch (TPSPlayerState->GetHoldGun()->Position)
	{
		case EWeaponPosition::E_Left: 
		{
			TPSPlayerState->SetWeapon1(TPSPlayerState->GetHoldGun());
			break;
		}
		case  EWeaponPosition:: E_Right:
		{
			TPSPlayerState->SetWeapon2(TPSPlayerState->GetHoldGun());
			break;
		}
	}
	TPSPlayerState->SetHoldGun(nullptr);

	if(ReadyEquipWeapon != nullptr) PlayerPlayMontage(EMontageType::E_Equip);
}

void ATPSCharacter::EventOnEquip()
{
	IsHoldWeapon = true;
	IsAiming = false;

	TPSPlayerState->SetHoldGun(ReadyEquipWeapon);
	ReadyEquipWeapon->IsOnHand = true;

	switch (ReadyEquipWeapon->Position)
	{
	case EWeaponPosition::E_Left:
	{
		TPSPlayerState->SetWeapon1(nullptr);
		break;
	}
	case  EWeaponPosition::E_Right:
	{
		TPSPlayerState->SetWeapon2(nullptr);
		break;
	}
	}

	ReadyEquipWeapon = nullptr;
}

void ATPSCharacter::EventOnReloadEnd()
{
	if (TPSPlayerState->GetHoldGun() != nullptr && PlayingMontageType == EMontageType::E_Reload)
	{
		TPSPlayerState->GetHoldGun()->FillClip();
	}
}

void ATPSCharacter::DamagePlayer(float Damage)
{
	Health = (Health - Damage) < 0 ? 0.f : Health - Damage;
	if(Health <= 0) 
	{
		IsDead = true;
		PlayerDead();
	}
}

void ATPSCharacter::UseHealthorBoost(ATPSItemUse* Use)
{
	switch (Use->ItemType)
	{
		case EItemType::E_Health:
		{
			float NewHealth = (Health + Use->Data.Value) > 100 ? 100.f : Health + Use->Data.Value;
			PlayerPlayMontage(EMontageType::E_Use, 1.0f, Use->Data.UseTime, true, NewHealth);
			break;
		}
		case EItemType::E_Boost:
		{
			float Heal = Use->Data.Value / 10.f;
			PlayerPlayMontage(EMontageType::E_Use, 1.0f, Use->Data.UseTime, false, Heal);
			break;
		}
		default: return;
	}
	Use->Amount --;
	if (Use->Amount <= 0)
	{
		TPSPlayerState->RemoveItem(Use);
	}
}

void ATPSCharacter::AddHealth(float Heal)
{
	Health = (Health + Heal) > 100 ? 100.f : Health + Heal;
	HealthCount++;
	if(HealthCount == 10) 
	{
		GetWorld()->GetTimerManager().ClearTimer(HealPlayer);
		HealthCount = 0;
	}
}
