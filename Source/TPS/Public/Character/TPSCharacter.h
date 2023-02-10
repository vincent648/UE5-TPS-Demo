// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "Character/TPSPlayerState.h"
#include "TPSCharacter.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAnimWait, Log, All);

class ATPSPickupBase;
class ATPSPlayerState;
class UTPSGameInstance;
class ATPSItemUse;
class ATPSPlayerController;

USTRUCT(BlueprintType)
struct FWalkSpeed
{
	GENERATED_BODY()

	UPROPERTY()
	float Forward;
	UPROPERTY()
	float Other;

	FWalkSpeed() {}
	FWalkSpeed(float InForward, float InOther)
	{
		Forward = InForward;
		Other = InOther;
	}
};

//************************ 目前没使用 ***************************************
UENUM(BlueprintType)
enum class EPlayerPose : uint8
{
	E_Stand        UMETA(DisplayName = "Stand"),
	E_Crouch       UMETA(DisplayName = "Crouch"),
	E_Prone        UMETA(DisplayName = "Prone")
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	E_Dead         UMETA(DisplayName = "Dead"),
	E_HoldWeapon   UMETA(DisplayName = "HoldWeapon"),
	E_Aim          UMETA(DisplayName = "Aim"),
	E_Fire         UMETA(DisplayName = "Fire"),
	E_Reload       UMETA(DisplayName = "Reload"),
	E_Equip        UMETA(DisplayName = "Equip")
};
//***************************************************************************

UENUM(BlueprintType)
enum class EMontageType : uint8
{
	E_Equip        UMETA(DisplayName = "Equip"),
	E_UnEquip      UMETA(DisplayName = "UnEquip"),
	E_Reload       UMETA(DisplayName = "Reload"),
	E_Fire         UMETA(DisplayName = "Fire "),
	E_Use          UMETA(DisplayName = "Use")
};

UCLASS(config=Game)
class ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	//**************** 角色动画蒙太奇委托 *****************
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquip);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipEnd);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnEquip);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnEquipEnd);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReload);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadEnd);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireEnd);
	//******************************************************

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ATPSCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	virtual void Tick(float DeltaTime) override;


protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	virtual void BeginPlay() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	//*********************** 角色动画混合空间和瞄准偏移 ********************************************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Pose")
	bool IsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Pose")
	bool IsProning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsHoldWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "On The Air")
	bool IsOnTheAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "On The Air")
	float ForwardValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsPlayingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AnimDirection;

	//********************** 角色动画相关计算 *********************************
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	float GetAnimSpeed() const { return GetVelocity().Size(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	float GetAnimDirection() const
	{
		return UKismetAnimationLibrary::CalculateDirection(GetVelocity(), this->GetActorRotation());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	float GetAnimYaw() const
	{
		if(!IsFreeView) return 0.0f;
		else return UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	float GetAnimPitch() const 
	{
		return UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	bool GetAnimIsInAir() const { return GetMovementComponent()->IsFalling(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	float GetAnimSidewayValue() const 
	{ 
		return UKismetMathLibrary::LessLess_VectorRotator(GetVelocity(),this->GetActorRotation()).Y;
	}
	//************************************************************************

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Calculate")
	int32 GetAnimOffsetType() const;
	//******************************************************************************************************************

	//******************************** 角色动画蒙太奇 *********************************************
	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FOnEquip OnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FOnEquipEnd OnEquipEnd;

	UFUNCTION(BlueprintCallable,  Category = "Equip")
	void CallEventOnEquip() { if(OnEquip.IsBound()) OnEquip.Broadcast(); }

	UFUNCTION(BlueprintCallable,  Category = "Equip")
	void CallEventOnEquipEnd() { if (OnEquipEnd.IsBound()) OnEquipEnd.Broadcast(); }

	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FUnEquip UnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FUnEquipEnd UnEquipEnd;

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void CallEventUnEquip() { if (UnEquip.IsBound()) UnEquip.Broadcast(); }

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void CallEventUnEquipEnd() { if (UnEquipEnd.IsBound()) UnEquipEnd.Broadcast(); }

	UPROPERTY(BlueprintAssignable, Category = "Reload")
	FReload Reload;

	UPROPERTY(BlueprintAssignable, Category = "Reload")
	FReloadEnd ReloadEnd;

	UFUNCTION(BlueprintCallable, Category = "Reload")
	void CallEventReload() { if (Reload.IsBound()) Reload.Broadcast(); }

	UFUNCTION(BlueprintCallable, Category = "Reload")
	void CallEventReloadEnd() { if (ReloadEnd.IsBound()) ReloadEnd.Broadcast(); }

	UPROPERTY(BlueprintAssignable, Category = "Fire")
	FFireEnd FireEnd;

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void CallEventFireEnd() { if (FireEnd.IsBound()) FireEnd.Broadcast(); }
	//**************************************************************************************

	//****************************** 角色移动 *********************************************
	void SmoothIncrease();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float MaxWalkSpeed = 625.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Movement")
	TMap<FString, FWalkSpeed> WalkSpeedMap
	{
		{FString(TEXT("false_stand_walk")), FWalkSpeed(175.0f,158.0f)},
		{FString(TEXT("false_stand_jog")), FWalkSpeed(500.0f,333.0f)},
		{FString(TEXT("false_stand_run")), FWalkSpeed(625.0f,333.0f)},
		{FString(TEXT("true_stand_walk")), FWalkSpeed(170.0f,150.0f)},
		{FString(TEXT("true_stand_jog")), FWalkSpeed(490.0f,300.0f)},
		{FString(TEXT("true_stand_run")), FWalkSpeed(600.0f,300.0f)},
		{FString(TEXT("true_stand_aim")), FWalkSpeed(180.0f,151.0f)},
		{FString(TEXT("false_crouch_walk")), FWalkSpeed(135.0f,119.0f)},
		{FString(TEXT("false_crouch_jog")), FWalkSpeed(333.0f,238.0f)},
		{FString(TEXT("false_crouch_run")), FWalkSpeed(476.0f,238.0f)},
		{FString(TEXT("true_crouch_walk")), FWalkSpeed(130.0f,100.0f)},
		{FString(TEXT("true_crouch_jog")), FWalkSpeed(320.0f,220.0f)},
		{FString(TEXT("true_crouch_run")), FWalkSpeed(460.0f,220.0f)},
		{FString(TEXT("true_crouch_aim")), FWalkSpeed(138.0f,90.0f)},
		{FString(TEXT("false_prone_walk")), FWalkSpeed(49.0f,30.0f)},
		{FString(TEXT("false_prone_jog")), FWalkSpeed(119.0f,100.0f)},
		{FString(TEXT("false_prone_run")), FWalkSpeed(119.0f,100.0f)},
		{FString(TEXT("true_prone_walk")), FWalkSpeed(40.0f,25.0f)},
		{FString(TEXT("true_prone_jog")), FWalkSpeed(110.0f,90.0f)},
		{FString(TEXT("true_prone_run")), FWalkSpeed(110.0f,90.0f)}
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	bool IsWalk;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	bool IsRun;

	void UpdateWalkSpeed();

	float MoveForwardValue;
	float MoveRightValue;
	//**********************************************************************************

	//**************************** 动画切换等待 ****************************************
	//等待时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Wait")
	TMap<FString, float> AnimWaitTimeMap
	{
		{FString(TEXT("false_prone_crouch")), 1.1f},
		{FString(TEXT("true_prone_crouch")), 1.6f},
		{FString(TEXT("false_prone_stand")), 1.2f},
		{FString(TEXT("true_prone_stand")), 2.7f},
		{FString(TEXT("false_crouch_prone")), 1.0f},
		{FString(TEXT("true_crouch_prone")), 1.5f},
		{FString(TEXT("false_stand_prone")), 1.2f},
		{FString(TEXT("true_stand_prone")), 2.4f},
		{FString(TEXT("false_prone_jump")), 1.5f},
		{FString(TEXT("true_prone_jump")), 1.8f}
	};

	void AnimWait(bool IsHoldWeaponValue, FString Current, FString Target);

	bool EnableMove = true;
	bool CanJump = true;

	FTimerHandle AnimWaitHandle;

	void AnimWaitEnd();

	//摄像机角度限制
	bool LimitCameraPitchAngle(float Val);
	//******************************************************************************

	//*********************** 自由视角 **********************************
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UTimelineComponent* TimelineComp;

	FOnTimelineFloat TimelineEvent;
	FOnTimelineEventStatic TimelineFinishedEvent;

	UPROPERTY()
	UCurveFloat* FCurve;

	UFUNCTION()
	void AltSetControlRotation(float Val);

	bool IsFreeView;

	UFUNCTION()
	void AltFinish() { IsFreeView = false; }

	FRotator AltPressRotation;
	FRotator AltReleaseRotation;
	//**************************************************************

//******************** 角色输入函数 ***********************
public:
	
	void PlayerCrouch();
	void PlayerProne();
	void PlayerJump();

	void PlayerWalk() { IsWalk = true; }
	void PlayerStopWalk() { IsWalk = false; }
	void PlayerRun() { IsRun = true; }
	void PlayerStopRun() { IsRun = false; }

	void PlayerFreeView();
	void PlayerStopFreeView();

	void PlayerDiscardWeapon();

	void PlayerAiming();
	void PlayerStopAiming();

	UFUNCTION(BlueprintCallable,  Category = "PlayerInput")
	void PlayerTakeBackGun();
	UFUNCTION(BlueprintCallable,  Category = "PlayerInput")
	void PlayerKeyBoard1();
	UFUNCTION(BlueprintCallable,  Category = "PlayerInput")
	void PlayerKeyBoard2();

	void PlayerChangeShootMode();
	
	void PlayerFire();
	void PlayerStopFire();

	void PlayerReload();
	//*******************************************************

public:
	
	//***************************** 拾取系统 ******************************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TArray<ATPSPickupBase*> ItemsInRange;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void UpdateWeaponDisplay(FName HoldSocket);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void TargetPickupItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	ATPSPickupBase* ReadyPickupItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ATPSPlayerState* TPSPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInstance")
	UTPSGameInstance* TPSGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	ATPSPlayerController* PlayerController;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FWeaponPosition AutoPosition();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FWeaponPosition AssignPosition(EWeaponPosition Assign);
	
	//根据持有物品生成可拾取物品（丢弃物品）
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	ATPSPickupBase* SpawnPickupItem(ATPSItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetPickupItem(ATPSPickupBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void ExecBeginOverlap(ATPSPickupBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void ExecEndOverlap(ATPSPickupBase* Item);

	//根据物品类型生成可拾取物品
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	ATPSPickupBase* SpawnPickupByType(EItemType Type, FVector Location, FRotator Rotation, 
	int32 Ammo, FName ID, FString SN, int32 Amount, ATPSItemBase* Item);

	//拾取武器
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickupWeapon(ATPSPickupWeapon* Weapon, bool IsAssign, EWeaponPosition Position);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	ATPSItemWeapon* SpawnItemWeapon(ATPSPickupWeapon* Weapon, FWeaponPosition WeaponPosition);

	//丢弃武器
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void DiscardWeapon(ATPSItemWeapon* Weapon);

	//获取当前动画对应的武器插槽名字
	UFUNCTION(BlueprintCallable, BlueprintPure,  Category = "Pickup")
	FName CalculateHoldOnSocket();

	UFUNCTION()
	void EventWeaponChange(ATPSItemWeapon* Weapon, FWeaponPosition WeaponPosition);

	//拾取物品
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickupItem();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void UpdatePlayerGunState();

	//拾取道具
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool PickupUse(ATPSPickupUse* Item);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void DiscardUse(ATPSItemUse* Use);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	ATPSItemUse* SpawnItemUse(ATPSPickupUse* Item);

	UFUNCTION()
	void EventItemsChange(ATPSItemBase* Item, bool IsAdd);
	//*****************************************************************************************

	//*************************** 角色动画蒙太奇 ********************************************
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AnimationMontage")
	void PlayerPlayMontage(EMontageType Type, float ReloadTime = 1.0f, float UseTime = 1.0f, bool IsHealth = true, float NewHealth = 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationMontage")
	EMontageType PlayingMontageType;

	UFUNCTION(BlueprintCallable, Category = "AnimationMontage")
	void EventOnUnEquip();

	UFUNCTION(BlueprintCallable, Category = "AnimationMontage")
	void EventOnEquip();

	UFUNCTION(BlueprintCallable, Category = "AnimationMontage")
	void EventOnMontageEnd() {IsPlayingMontage = false; }

	UFUNCTION(BlueprintCallable, Category = "AnimationMontage")
	void EventOnReloadEnd();

	//************************* 武器切换 **************************************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSwitch")
	ATPSItemWeapon* ReadyEquipWeapon;

	//******************* 血量 ******************************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = 100.f;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void DamagePlayer(float Damage);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void UseHealthorBoost(ATPSItemUse* Use);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void BoostStart(float AddHeal)
	{
		TimerDelegate.BindUFunction(this, FName("AddHealth"), AddHeal);
		GetWorld()->GetTimerManager().SetTimer(HealPlayer, TimerDelegate, 1.f, true);
	}

	FTimerHandle HealPlayer;
	FTimerDelegate TimerDelegate;
	int32 HealthCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Heal);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dead")
	void PlayerDead();
};



