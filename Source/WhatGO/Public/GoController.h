// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GoController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class WHATGO_API AGoController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGoController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private: 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* GoControllerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> ClickShake;

	void OnMouseClick();
};
