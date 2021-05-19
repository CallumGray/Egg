// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EggPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EGG_API AEggPlayerController : public APlayerController
{
	GENERATED_BODY()

	private:
	UPROPERTY(EditDefaultsOnly,Category="Widgets")
	TSubclassOf<class UUserWidget> LevelCompleteClass;

	UPROPERTY(EditAnywhere,Category="Sound")
	USoundBase* CompleteSound;
	
	public:
	void LevelComplete();
};
