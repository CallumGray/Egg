// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementSequence.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
UMovementSequence::UMovementSequence()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMovementSequence::BeginPlay()
{
	Super::BeginPlay();

	MeshActor = Cast<AStaticMeshActor>(GetOwner());
	if(!MeshActor)
	{
		UE_LOG(LogTemp,Error,TEXT("NOT A MESH ACTOR!!"));
		return;
	}

	MeshActor->SetMobility(EComponentMobility::Movable);
	MeshActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
		
	//Get initial location in world space
	FirstLocation = GetOwner()->GetActorLocation();
	
	//Set world space for second location
	SecondLocation = FirstLocation + SecondLocation;
	
	if(LocationTimerLength > 0.f)
	{
		MovementSpeed = FVector::Dist(FirstLocation,SecondLocation);
		GetWorld()->GetTimerManager().SetTimer(
		LocationTimer,this,&UMovementSequence::SwitchLocationTarget,LocationTimerLength,true);
	}
	else
	{
		MovementSpeed = 0.f;
	}

	Direction = SecondLocation-FirstLocation;
	Direction.Normalize();

	Axis = MeshActor->GetActorRotation().RotateVector(Axis);
}

// Called every frame
void UMovementSequence::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(MeshActor)
	{
		MeshActor->AddActorWorldOffset(Direction*MovementSpeed*DeltaTime);
		MeshActor->AddActorWorldRotation(FQuat(Axis,RotateSpeedRadians*DeltaTime));
	}
}

void UMovementSequence::SwitchLocationTarget()
{
	Direction *= -1.f;
}

