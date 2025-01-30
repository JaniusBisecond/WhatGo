// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhatGO/WhatGO.h"
#include "GoBoard.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIsFallStone, bool, bFallStone);

class AGoGameMode;

UCLASS()
class WHATGO_API AGoBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void StoneFollowMouse();

    // Initialize the board
    void InitializeBoard();

    // Print the board (for debugging)
    void PrintBoard();

    // Place a stone on the board
    bool PlaceStone(int X, int Y, EStoneColor Color);

    // Capture stones if necessary
    void CaptureStones(int X, int Y, EStoneColor Color);

    // Remove stones from the board
    int RemoveStones(int X, int Y, EStoneColor Color);

    // Count liberties of a group of stones
    int CountLiberties(int X, int Y, EStoneColor Color);

    // AI Computer move
    void ComputerMove(EStoneColor Color);

    // Check if a move is valid
    bool IsForbiddenPoint(int X, int Y, EStoneColor Color);

	// Mapping world location to board array
    void MappingLocationToBoardAndPlaceStone(FVector Location, EStoneColor Color);

    // Board representation
    EStoneColor Board[19][19];

	// 空中落下棋子
    void FallStone(EStoneColor Color);

    // 捶桌子
    void Hammer();

    // 重置棋盘棋子
    void Reset();

protected:
    UPROPERTY(EditAnywhere, Category = "Stone")
	TSubclassOf<AActor> WhiteStoneClass;

    UPROPERTY(EditAnywhere, Category = "Stone")
    TSubclassOf<AActor> BlackStoneClass;

private:
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
    float BoardXLenght = 97.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
    float BoardYLenght = 97.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
    float XOffset = 0.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
    float YOffset = 0.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
    float StoneSetHeight = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
	float StoneFallHeight = 20.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
	float FollowDegree = 10.f;
    UPROPERTY(EditDefaultsOnly, Category = "BoardSetting")
	float ScanInterval = 2.0f;
	float ScanTimer = 0.0f;
    void ScanStones(float DeltaTime);

    float XUint = 0.f;
	float YUint = 0.f;

	TObjectPtr<AGoGameMode> GameMode = nullptr;
	
    void SpawnStone(int X, int Y, EStoneColor Color);
	TMap<FIntPoint, TObjectPtr<AActor>> StoneMap;

    void GenerateFloatingStone(EStoneColor Color);
    TObjectPtr<AActor> FollowedStone;
    int RemovedStones = 0;

	FVector GetFloatingStoneLocation();

	//所有生成过的棋子，确保重置时能够清除
	TArray<TObjectPtr<AActor>> AllSpawnedStones;

    //前5个随机落子
    int ComputerMoveCount = 5;

    void testPreset();
};
