// Fill out your copyright notice in the Description page of Project Settings.

#include "Goal.h"

#include "EggPawn.h"
#include "EggPlayerController.h"
#include "Components/SphereComponent.h"

// Sets default values
AGoal::AGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();

	if(SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&AGoal::GoalHit);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("BLEH"));
	}

	
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoal::GoalHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEggPawn* Player = Cast<AEggPawn>(OtherActor);

	if(Player)
	{
		AEggPlayerController* PlayerController = Cast<AEggPlayerController>(Player->GetController());

		if(PlayerController)
		{
			PlayerController->LevelComplete();
			Destroy();
		}
	}
}

