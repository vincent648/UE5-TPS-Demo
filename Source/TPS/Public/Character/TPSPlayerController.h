// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ATPSPlayerController();

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UUIManagementComponent* UIComponent;

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UIComponent")
	class UUIManagementComponent* GetUIComponent() const { return UIComponent; }

	UFUNCTION(BlueprintCallable, Category = "Input")
	void PlayerShowInventory();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void PlayerEsc();

	virtual void SetupInputComponent() override;
};
