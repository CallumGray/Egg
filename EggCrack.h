// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EggCrack.generated.h"

UCLASS()
class EGG_API AEggCrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEggCrack();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* EggTop;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* EggBot;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CrackOpen(FVector WorldLocation, FVector Impulse,FVector CrackVelocity);

	//How fast each half of the egg fly away from the impact point
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crack")
	float WorldCollisionScale = 0.5f;

	//How fast each half of the egg fly away from each other
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crack")
	float CrackOppositionScale = 0.5f;

};
