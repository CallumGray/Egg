// Fill out your copyright notice in the Description page of Project Settings.


#include "EggCrack.h"

// Sets default values
AEggCrack::AEggCrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;
	
	EggTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EggTop"));
	EggTop->SetupAttachment(SceneRoot);
	EggTop->SetSimulatePhysics(true);

	EggBot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EggBot"));
	EggBot->SetupAttachment(SceneRoot);
	EggBot->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AEggCrack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEggCrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEggCrack::CrackOpen(FVector WorldLocation, FVector WorldImpulse,FVector CrackVelocity)
{
	EggTop->SetPhysicsLinearVelocity(CrackVelocity);
	EggBot->SetPhysicsLinearVelocity(CrackVelocity);
	
	FVector TopDirection = GetActorUpVector();
	FVector BotDirection = -GetActorUpVector();

	EggTop->AddImpulse(TopDirection * WorldImpulse.Size() * CrackOppositionScale);
	EggBot->AddImpulse(BotDirection * WorldImpulse.Size() * CrackOppositionScale);

	WorldImpulse.Normalize();
	
	EggTop->AddImpulse(WorldImpulse * WorldCollisionScale);
	EggBot->AddImpulse(WorldImpulse * WorldCollisionScale);
}

