// Fill out your copyright notice in the Description page of Project Settings.

#include "EggPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AEggPlayerController::LevelComplete()
{
	if(LevelCompleteClass)
	{
		UUserWidget* LevelCompleteScreen = CreateWidget(this,LevelCompleteClass);
		if(LevelCompleteScreen)LevelCompleteScreen->AddToViewport();
	}

	if(CompleteSound)UGameplayStatics::PlaySound2D(GetWorld(),CompleteSound);
}
