// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GoGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WHATGO_API UGoGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	UPROPERTY(BlueprintReadWrite)
	bool bShowMenu = true;

protected:
	virtual void OnPreLoadMap(const FString& MapName);
	virtual void OnPostLoadMap(UWorld* LoadedWorld);
};
