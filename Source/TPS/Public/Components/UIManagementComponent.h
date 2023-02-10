// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/StackStateMachineInterface.h"
#include "UIManagementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStackStateMachine, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogUIManagementComponent, Log, All);

class UParentUI;

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class TPS_API UUIManagementComponent : public UActorComponent, public IStackStateMachineInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUIManagementComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStackPushOrPop, TScriptInterface<IStackStateMachineInterface>, State);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterNewState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitOldState);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PushState(TScriptInterface<IStackStateMachineInterface> NewState, bool Hide);

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopState();

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopStates(int32 Num);

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopAllStates();

	virtual void EnterNewState(EStackOperation StackOperation) override;
	virtual void ExitOldState(EStackOperation StackOperation, bool Hide) override;

	// UFUNCTION(BlueprintImplementableEvent,Category="StackStateMachine")
	// void BP_EnterNewState(EStackOperation StackOperation);

	// UFUNCTION(BlueprintImplementableEvent,Category="StackStateMachine")
	// void BP_ExitOldState(EStackOperation StackOperation);

	// UFUNCTION(BlueprintImplementableEvent,Category="StackStateMachine")
	// void BP_StatePushed(const TScriptInterface<IStackStateMachineInterface>& State);

	// UFUNCTION(BlueprintImplementableEvent,Category="StackStateMachine")
	// void BP_StatePopped(const TScriptInterface<IStackStateMachineInterface>& State);
	
	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnEnterNewState OnEnterNewState;

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnExitOldState OnExitOldState;

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnStackPushOrPop OnStackPush;

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnStackPushOrPop OnStackPop;

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackStateMachineInterface> GetCurrentState();

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackStateMachineInterface> GetUnderState();

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackStateMachineInterface> GetState(int32 Index);

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	int32 StateNum();

protected:
	
	virtual void StatePushed(TScriptInterface<IStackStateMachineInterface> State);
	
	virtual void StatePopped(TScriptInterface<IStackStateMachineInterface> State);

	TArray<TScriptInterface<IStackStateMachineInterface>> States;

public:

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	UParentUI* GetUI(FName UIName);

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	int32 GetUIIndex(FName UIName);

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	void PushUI(FName UINmae, bool Hide);

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	UParentUI* GetCurrentUIInstance();

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	UParentUI* GetUIInstance(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UIManagementComponent")
	TMap<FName, TSubclassOf<UParentUI>> UINameAndClass;

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	UParentUI* CreateUI(FName UIName);

	UPROPERTY(BlueprintReadWrite, Category = "UIManagementComponent")
	TMap<FName, UParentUI*> UINameAndInstances;

	UPROPERTY(BlueprintReadWrite, Category = "UIManagementComponent")
	TMap<FName, UParentUI*> UINameAndInstancesNotInUse;

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	FName GetUIName(UParentUI* UI);

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	void OpenUI(FName UIName, bool Hide);

	UFUNCTION(BlueprintCallable, Category = "UIManagementComponent")
	void CloseCurrentUI();
		
};
