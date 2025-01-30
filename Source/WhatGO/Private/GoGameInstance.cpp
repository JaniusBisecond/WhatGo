// Fill out your copyright notice in the Description page of Project Settings.


#include "GoGameInstance.h"
#include "MoviePlayer.h"
#include "GameFramework/GameUserSettings.h"


void UGoGameInstance::Init()
{
	Super::Init();

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	UserSettings->SetFrameRateLimit(60); // 设置最大帧率为60

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMap);
}

void UGoGameInstance::OnPreLoadMap(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UGoGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();
}
