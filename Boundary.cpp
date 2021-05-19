// Fill out your copyright notice in the Description page of Project Settings.


#include "Boundary.h"

#include "EggPawn.h"
#include "Components/BoxComponent.h"

// Sets default values
ABoundary::ABoundary()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = BoxComponent;
}

// Called when the game starts or when spawned
void ABoundary::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this,&ABoundary::HitResponse);
}

void ABoundary::HitResponse(AActor* OverlappedActor, AActor* OtherActor)
{
	AEggPawn* Egg = Cast<AEggPawn>(OtherActor);

	if(Egg)
	{
		Egg->Respawn();
	}
}

