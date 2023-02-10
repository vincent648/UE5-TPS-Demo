// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UIManagementComponent.h"
#include "UI/ParentUI.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogStackStateMachine);
DEFINE_LOG_CATEGORY(LogUIManagementComponent);

// Sets default values for this component's properties
UUIManagementComponent::UUIManagementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUIManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUIManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUIManagementComponent::PushState(TScriptInterface<IStackStateMachineInterface> NewState, bool Hide)
{
	UParentUI* UI = Cast<UParentUI>(NewState.GetObject());
	if(UINameAndInstancesNotInUse.Find(GetUIName(UI)) != nullptr) UINameAndInstancesNotInUse.Remove(GetUIName(UI));
	
	if (StateNum() > 0)
	{
		auto CurrentState = GetCurrentState();
		if (CurrentState != nullptr)
		{
			CurrentState->ExitOldState(Push, Hide);
			ExitOldState(Push, Hide);
		}
	}

	States.Push(NewState);
	NewState->EnterNewState(Push);

	EnterNewState(Push);
	StatePushed(NewState);
}

void UUIManagementComponent::PopState()
{
	if(StateNum() == 0) return;

	auto OldState = GetCurrentState();
	if (OldState != nullptr)
	{
		OldState->ExitOldState(Pop, true);
		ExitOldState(Pop, true);
		
		UParentUI* UI = Cast<UParentUI> (OldState.GetObject());
		UINameAndInstancesNotInUse.Add(GetUIName(UI), UI);
	}

	States.Pop();

	if (StateNum() > 0)
	{
		GetCurrentState()->EnterNewState(Pop);
		EnterNewState(Pop);
	}

	StatePopped(OldState);
}

void UUIManagementComponent::PopStates(int32 Num)
{
	while (Num > 0)
	{
		PopState();
		Num--;
	}
}

void UUIManagementComponent::PopAllStates()
{
	if(StateNum() > 0) PopStates(StateNum());
}

void UUIManagementComponent::EnterNewState(EStackOperation StackOperation)
{
	if (OnEnterNewState.IsBound())
	{
		OnEnterNewState.Broadcast();
	}
	//BP_EnterNewState(StackOperation);
}

void UUIManagementComponent::ExitOldState(EStackOperation StackOperation, bool Hide)
{
	if (OnExitOldState.IsBound())
	{
		OnExitOldState.Broadcast();
	}
	// BP_ExitOldState(StackOperation);
}

TScriptInterface<IStackStateMachineInterface> UUIManagementComponent::GetCurrentState()
{
	return States[StateNum() - 1];
}

TScriptInterface<IStackStateMachineInterface> UUIManagementComponent::GetUnderState()
{
	return States[StateNum() - 2];
}

TScriptInterface<IStackStateMachineInterface> UUIManagementComponent::GetState(int32 Index)
{
	return States[Index];
}

int32 UUIManagementComponent::StateNum()
{
	return States.Num();
}

void UUIManagementComponent::StatePushed(TScriptInterface<IStackStateMachineInterface> State)
{
	UE_LOG(LogStackStateMachine, Warning, TEXT("Stack Push UI: %s"), *State.GetObject()->GetName());

	if (OnStackPush.IsBound())
	{
		OnStackPush.Broadcast(State);
	}
	// BP_StatePushed(State);
}

void UUIManagementComponent::StatePopped(TScriptInterface<IStackStateMachineInterface> State)
{
	UE_LOG(LogStackStateMachine, Warning, TEXT("Stack Pop UI: %s"), *State.GetObject()->GetName());

	if (OnStackPop.IsBound())
	{
		OnStackPop.Broadcast(State);
	}
	// BP_StatePopped(State);
}



UParentUI* UUIManagementComponent::GetUI(FName UIName)
{
	UParentUI** UI = UINameAndInstances.Find(UIName);
	if (UI != nullptr)
	{
		return *UI;
	}

	UE_LOG(LogUIManagementComponent, Warning, TEXT("UI not exist: %s"), *UIName.ToString());

	return nullptr;
}

void UUIManagementComponent::PushUI(FName UIName, bool Hide)
{
	auto UI = GetUI(UIName);

	if (UI == nullptr) return;

	TScriptInterface<IStackStateMachineInterface> Interface(UI);

	PushState(UI, Hide);
}


UParentUI* UUIManagementComponent::GetCurrentUIInstance()
{
	if (StateNum() > 0)
	{
		UParentUI* UIInstance = Cast<UParentUI>(GetCurrentState().GetObject());
		if (UIInstance != nullptr)
		{
			return UIInstance;
		}
	}
	return nullptr;
}

UParentUI* UUIManagementComponent::GetUIInstance(int32 Index)
{
	if (StateNum() > 0)
	{
		UParentUI* UIInstance = Cast<UParentUI>(GetState(Index).GetObject());
		if (UIInstance != nullptr)
		{
			return UIInstance;
		}
	}
	return nullptr;
}

int32 UUIManagementComponent::GetUIIndex(FName UIName)
{
	auto UI = GetUI(UIName);

	if (UI == nullptr) return -1;

	TScriptInterface<IStackStateMachineInterface> Interface(UI);

	return States.Find(UI);
}

FName UUIManagementComponent::GetUIName(UParentUI* UI)
{
	return *UINameAndInstances.FindKey(UI);
}


UParentUI* UUIManagementComponent::CreateUI(FName UIName)
{
	auto PC = UGameplayStatics::GetPlayerController(GetOwner(), 0);

	TSubclassOf<UParentUI>* UI = UINameAndClass.Find(UIName);
	if (UI == nullptr) return nullptr;

	if (UINameAndInstancesNotInUse.Find(UIName) == nullptr)
	{
		auto Widget = CreateWidget<UParentUI>(PC, *UI, UIName);
		if (Widget == nullptr) { return nullptr; }

		UE_LOG(LogUIManagementComponent, Warning, TEXT("Create New UI: %s"), *Widget->GetName());

		UINameAndInstances.Add(UIName, Widget);

		Widget->Init();

		return Widget;
	}

	UParentUI* Widget = GetUI(UIName);
	UE_LOG(LogUIManagementComponent, Warning, TEXT("Exist UI: %s Begin Init"), *Widget->GetName());

	Widget->Init();
	Widget->SetVisibility(ESlateVisibility::Visible);

	return Widget;
}

void UUIManagementComponent::OpenUI(FName UIName, bool Hide)
{
	UParentUI* UI = CreateUI(UIName);

	if (UI != nullptr)
	{
		PushState(UI, Hide);
	}
	else
	{
		UE_LOG(LogUIManagementComponent, Error, TEXT("Open UI Error!"));
	}
}

void UUIManagementComponent::CloseCurrentUI()
{
	if (StateNum() == 0) return;
	PopState();
}