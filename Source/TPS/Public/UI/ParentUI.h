// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/StackStateMachineInterface.h"
#include "ParentUI.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class TPS_API UParentUI : public UUserWidget, public IStackStateMachineInterface
{
	GENERATED_BODY()

public:

	virtual void EnterNewState(EStackOperation StackOperation) override;
	virtual void ExitOldState(EStackOperation StackOperation, bool Hide) override;

	UFUNCTION(BlueprintNativeEvent, Category = "StackStateMachine", meta = (DisplayName = "EnterNewState"))
	void BP_EnterNewState(EStackOperation StackOperation);

	UFUNCTION(BlueprintNativeEvent, Category = "StackStateMachine", meta = (DisplayName = "ExitOldState"))
	void BP_ExitOldState(EStackOperation StackOperation, bool Hide);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
	void Init();
	
};
