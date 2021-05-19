// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boundary.generated.h"

UCLASS()
class EGG_API ABoundary : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoundary();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class UBoxComponent* BoxComponent;
	
	UFUNCTION()
	void HitResponse(AActor* OverlappedActor, AActor* OtherActor);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
