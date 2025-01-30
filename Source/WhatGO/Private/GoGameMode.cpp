// Fill out your copyright notice in the Description page of Project Settings.


#include "GoGameMode.h"
#include "GoBoard.h"

// Sets default values
AGoGameMode::AGoGameMode()
{
    // Set this gamemode to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void AGoGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGoGameMode::FallStone(EStoneColor Color)
{
	if (Color == EStoneColor::Black)
	{
		Board->FallStone(EStoneColor::White);
	}
	else
	{
		Board->FallStone(EStoneColor::Black);
	}
}

void AGoGameMode::RestartGame()
{
	Board->Reset();
	Score = 0;
	OnRestart.Broadcast();
}
