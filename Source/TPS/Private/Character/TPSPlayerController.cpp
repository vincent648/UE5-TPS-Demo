// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPSPlayerController.h"
#include "Components/UIManagementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TPSCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ATPSPlayerController::ATPSPlayerController()
{
	UIComponent = CreateDefaultSubobject<UUIManagementComponent>(TEXT("UIComponent"));
}

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("ShowInventory", IE_Pressed, this, &ATPSPlayerController::PlayerShowInventory);

	InputComponent->BindAction("Esc", IE_Pressed, this, &ATPSPlayerController::PlayerEsc);
}

void ATPSPlayerController::PlayerShowInventory()
{
	if (GetUIComponent()->StateNum() == 1)
	{
		GetUIComponent()->OpenUI(FName(TEXT("Backpack")), true);
		bShowMouseCursor = true;
		GetPawn()->DisableInput(this); 
	}
	else if (GetUIComponent()->StateNum() == 2)
	{
		GetUIComponent()->CloseCurrentUI();
		bShowMouseCursor = false;
		GetPawn()->EnableInput(this);
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
}

void ATPSPlayerController::PlayerEsc()
{
	if (GetUIComponent()->StateNum() == 2)
	{
		GetUIComponent()->CloseCurrentUI();
		bShowMouseCursor = false;
		GetPawn()->EnableInput(this);
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
	else if (GetUIComponent()->StateNum() > 2) GetUIComponent()->CloseCurrentUI();
	else UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}