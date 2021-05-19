// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementSequence.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EGG_API UMovementSequence : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementSequence();

	//Location offset from position in local space
	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="Location")
	FVector SecondLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="Location")
	float LocationTimerLength = 1.f;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="Rotation")
	FVector Axis = FVector::ForwardVector;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="Rotation")
	float RotateSpeedRadians = 0.f;

private:

	//For moving location
	UFUNCTION()void SwitchLocationTarget();
	FTimerHandle LocationTimer;
	FVector FirstLocation;
	FVector Direction;
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere,Category="Owner")
	class AStaticMeshActor* MeshActor;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
