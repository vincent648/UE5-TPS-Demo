// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StackStateMachineInterface.generated.h"

UENUM(BlueprintType)
enum EStackOperation
{
	Push	UMETA(DisplayName = "Push"),
	Pop		UMETA(DisplayName = "Pop")
};



// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UStackStateMachineInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPS_API IStackStateMachineInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EnterNewState(EStackOperation StackOperation) = 0;
	
	virtual void ExitOldState(EStackOperation StackOperation, bool Hide) = 0;
};
