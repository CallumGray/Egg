// Fill out your copyright notice in the Description page of Project Settings.

#include "EggPawn.h"

#include "DrawDebugHelpers.h"
#include "EggCrack.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


AEggPawn::AEggPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EggMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EggMesh"));
	RootComponent = EggMesh;
	EggMesh->SetSimulatePhysics(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AEggPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(),APlayerStart::StaticClass()));
	OnActorHit.AddDynamic(this, &AEggPawn::HitResponse);

	Respawn();
}

void AEggPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEggPawn::HitResponse(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//Save on expensive sqrt
	float TotalImpulseSquared = NormalImpulse.SizeSquared();

	if(TotalImpulseSquared > BaseImpulse*BaseImpulse)
	{
		float TotalImpulse = FMath::Sqrt(TotalImpulseSquared);
		float Damage = BaseDamage*TotalImpulse/BaseImpulse;
		Health -= Damage;

		if(Health < 0.f)
		{
			// DIE !!
			if(CrackSound)UGameplayStatics::PlaySoundAtLocation(GetWorld(),CrackSound,GetActorLocation());
			if(DeathSound)UGameplayStatics::PlaySoundAtLocation(GetWorld(),DeathSound,GetActorLocation());
			SpawnEggSplat(Hit.Location,Hit.ImpactNormal);

			FVector PostCrackVelocity = GetVelocity();
			SetEnabled(false);
			SpawnEggCrack(Hit.Location,NormalImpulse,PostCrackVelocity);

			GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AEggPawn::Respawn, DeathDelay, false);
		}
		else
		{
			// Crack
			if(CrackSound)UGameplayStatics::PlaySoundAtLocation(GetWorld(),CrackSound,GetActorLocation());
			SpawnCrackDecal(Hit.Location,Hit.ImpactNormal);
		}
	}
}

void AEggPawn::SetEnabled(bool bEnabled)
{
	SetActorTickEnabled(bEnabled);
	SetActorEnableCollision(bEnabled);
	SetActorHiddenInGame(!bEnabled);
	EggMesh->SetSimulatePhysics(bEnabled);
}

//Input and Movement
void AEggPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX",this,&AEggPawn::MoveHorizontal);
	PlayerInputComponent->BindAxis("MoveY",this,&AEggPawn::MoveVertical);
	PlayerInputComponent->BindAxis("CameraX",this,&AEggPawn::MoveCameraHorizontal);
	PlayerInputComponent->BindAction("Jump",EInputEvent::IE_Pressed,this,&AEggPawn::Jump);
}

void AEggPawn::MoveHorizontal(float InputX)
{
	if(!EggMesh->IsSimulatingPhysics())return;
	if(FMath::IsNearlyZero(InputX))return;

	//Only generate torque if touching ground
	if(IsTouchingGround())
	{
		FVector FlattenedCameraForward = Camera->GetForwardVector()*FVector(1.f,1.f,0.f);
		FlattenedCameraForward.Normalize();
		EggMesh->AddTorqueInRadians(InputX*TorqueMagnitude*-FlattenedCameraForward,NAME_None,true);		
	}
	else
	{
		//Drift
		EggMesh->AddForce(InputX*Camera->GetRightVector()*DriftMagnitude,NAME_None,true);
	}
}

void AEggPawn::MoveVertical(float InputY)
{
	if(!EggMesh->IsSimulatingPhysics())return;
	if(FMath::IsNearlyZero(InputY))return;

	//Only generate torque if touching ground
	if(IsTouchingGround())
	{
		EggMesh->AddTorqueInRadians(InputY*TorqueMagnitude*Camera->GetRightVector(),NAME_None,true);
	}
	else
	{
		//Drift
		FVector FlattenedCameraForward = Camera->GetForwardVector()*FVector(1.f,1.f,0.f);
		FlattenedCameraForward.Normalize();
		EggMesh->AddForce(InputY*FlattenedCameraForward*DriftMagnitude,NAME_None,true);
	}
}

void AEggPawn::MoveCameraHorizontal(float InputX)
{
	SpringArm->AddRelativeRotation(
		FRotator(0.f,CameraRotationSpeedDegrees*InputX*GetWorld()->GetDeltaSeconds(),0.f));
}

void AEggPawn::Jump()
{
	if(bDrawGroundSphere)
	{
		const FVector Start = GetActorLocation();
		const FVector End = GetActorLocation()+FVector::DownVector*GroundTraceLength;
		
		//Draw start sphere
		DrawDebugSphere(GetWorld(),Start,GroundSphereRadius,32,FColor::Red,false,5.f);
		//Draw end sphere
		DrawDebugSphere(GetWorld(),End,GroundSphereRadius,32,FColor::Blue,false,5.f);

		//Print velocity too
		UE_LOG(LogTemp,Warning,TEXT("%s"),*GetVelocity().ToString());
	}
	
	if(EggMesh->IsSimulatingPhysics() && bJumpReady && GetVelocity().Z > FallSpeed && IsTouchingGround())
	{
		//Jump
		EggMesh->AddImpulse(FVector::UpVector*JumpMagnitude,NAME_None,true);

		//Disable jump inputs temporarily so jump impulses don't get stacked
		bJumpReady = false;
		GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &AEggPawn::JumpReady, JumpReadyDelay, false);
	}
}

void AEggPawn::JumpReady()
{
	bJumpReady = true;
}

bool AEggPawn::IsTouchingGround()
{
	//Trace below the egg to see if ground is there
	FHitResult Hit;
	const FVector Start = GetActorLocation();
	const FVector End = GetActorLocation()+FVector::DownVector*GroundTraceLength;
	const ECollisionChannel CollisionChannel = ECollisionChannel::ECC_Pawn;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	return GetWorld()->SweepSingleByChannel(
		Hit,Start,End,FQuat::Identity,CollisionChannel,FCollisionShape::MakeSphere(GroundSphereRadius),Params);
}

//Visualising damage
void AEggPawn::SpawnCrackDecal(FVector HitWorldLocation,FVector HitWorldNormal)
{
	//Spawn the decal
	if(DecalMaterial)
	{
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAttached(DecalMaterial,DecalSize,EggMesh,
			NAME_None,HitWorldLocation,(-HitWorldNormal).Rotation(),
			EAttachLocation::KeepWorldPosition,0);
		
		if(Decal)
		{
			Decals.Add(Decal);
			
			if(Decals.Num() > MaxDecals)
			{
				RemoveDecal();
			}
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Decal failed"));
		}
	}
}

void AEggPawn::RemoveDecal()
{
	//Removes the decals which are the closest to others
	
	float SmallestDistanceSquared = TNumericLimits<float>::Max();
	float SmallestIndex = 0;
	
	for(int32 i = 0;i<Decals.Num();i++)
	{
		FVector iLocation = Decals[i]->GetRelativeLocation();
		
		for(int32 j=0;j<Decals.Num();j++)
		{
			if(i < j)
			{
				FVector jLocation = Decals[j]->GetRelativeLocation();

				float DistanceSquared = FVector::DistSquared(iLocation,jLocation);
				
				if(DistanceSquared < SmallestDistanceSquared)
				{
					SmallestDistanceSquared = DistanceSquared;
					SmallestIndex = i;
				}
			}
		}
	}

	Decals[SmallestIndex]->DestroyComponent();
	Decals.RemoveAt(SmallestIndex);
}

void AEggPawn::SpawnEggSplat(FVector HitWorldLocation,FVector HitWorldNormal)
{
	if(EggSplatClass && !IsValid(EggSplat))
	{
		EggSplat = GetWorld()->SpawnActor<AActor>(EggSplatClass,HitWorldLocation+HitWorldNormal,HitWorldNormal.Rotation());
	}
}

void AEggPawn::SpawnEggCrack(FVector HitWorldLocation, FVector HitWorldImpulse,FVector CrackVelocity)
{
	if(EggCrackClass && !IsValid(EggCrack))
	{
		EggCrack = GetWorld()->SpawnActor<AEggCrack>(EggCrackClass,GetActorLocation(),GetActorRotation());
		EggCrack->CrackOpen(HitWorldLocation,HitWorldImpulse,CrackVelocity);
	}
}

//Respawning
void AEggPawn::Respawn()
{
	//Reset Health
	Health = StartingHealth;

	//Remove Decals
	for(UDecalComponent* Decal : Decals)Decal->DestroyComponent();
	Decals.Empty();

	//Remove death objects
	if(IsValid(EggCrack))EggCrack->Destroy();
	if(IsValid(EggSplat))EggSplat->Destroy();

	if(PlayerStart)
	{
		SetEnabled(true);
		SetActorLocation(PlayerStart->GetActorLocation(),false,nullptr,ETeleportType::ResetPhysics);
		EggMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		EggMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		EggMesh->SetRelativeRotation(FRotator::ZeroRotator);
		
		//Player start sets control rotation, which doesn't get mirrored since root simulates physics. 
		//Since root simulates physics, it doesn't get rotated and so its child SpringArm is manually set
		SetActorRotation(GetControlRotation());
		SpringArm->SetRelativeRotation(
			FRotator(SpringArm->GetRelativeRotation().Pitch,GetActorRotation().Yaw,SpringArm->GetRelativeRotation().Roll));
	}
}