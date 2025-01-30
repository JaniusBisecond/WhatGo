// Fill out your copyright notice in the Description page of Project Settings.


#include "GoController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "GoBoard.h"
#include "Kismet/GameplayStatics.h"
#include "GoGameMode.h"

AGoController::AGoController()
{
	bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AGoController::BeginPlay()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(this))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(GoControllerMappingContext, 1);
        }
    }
}

void AGoController::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGoController::OnMouseClick);
    }

}

void AGoController::OnMouseClick()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    AGoGameMode* GameMode = Cast<AGoGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;
    if (GameMode->IsFallStone())
    {
        GameMode->FallStone(GameMode->GetPlayerColor());
		GameMode->AddScore();
    }
    else if (HitResult.bBlockingHit)
    {
        if (AGoBoard* HitActor = Cast<AGoBoard>(HitResult.GetActor()))
        {
			FVector ClickedLocation = HitResult.ImpactPoint;
			HitActor->MappingLocationToBoardAndPlaceStone(ClickedLocation, GameMode->GetPlayerColor());
        }
        else if (HitResult.GetActor() == GameMode->GetDesk())
        {
			ClientStartCameraShake(ClickShake);
            GameMode->GetBoard()->Hammer();
            UGameplayStatics::PlaySoundAtLocation(this, GameMode->HammerSound, GetPawn()->GetActorLocation());
        }
    }
}