// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WhatGO/WhatGO.h"
#include "GoGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartGame);

class AGoBoard;
/**
 * 
 */
UCLASS()
class WHATGO_API AGoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    // Sets default values for this gamemode's properties
    AGoGameMode();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

	void FallStone(EStoneColor Color);
	void SetFallStone(bool bFall) { bFallStone = bFall; }
	bool IsFallStone() const { return bFallStone; }

	void SetBoard(AGoBoard* InBoard) { Board = InBoard; }
	AGoBoard* GetBoard() const { return Board.Get(); }

	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	void SetLid(AActor* InLid) { Lid = InLid; }
	AActor* GetLid() const { return Lid.Get(); }

	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	void SetDesk(AActor* InDesk) { Desk = InDesk; }
	AActor* GetDesk() const { return Desk.Get(); }

	EStoneColor GetPlayerColor() const { return PlayerColor; }

	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	bool IsNeedMenu() const { return bNeedMenu; }
	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	void SetIsNeedMenu(bool bNeed) { bNeedMenu = bNeed; }

	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	int GetScore() const { return Score; }
	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	void AddScore() { Score++; }

	UFUNCTION(BlueprintCallable, Category = "GoGameMode")
	void RestartGame();

	UPROPERTY(BlueprintAssignable, Category = "GoGameMode")
	FOnRestartGame OnRestart;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> HammerSound;

private:
	bool bFallStone = false;

	TObjectPtr<AGoBoard> Board = nullptr;
	TObjectPtr<AActor> Lid = nullptr;
	TObjectPtr<AActor> Desk = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "GoGameMode")
	EStoneColor PlayerColor = EStoneColor::Black;

	bool bNeedMenu = true;

	int Score = 0;
};
