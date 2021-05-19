// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EggPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class EGG_API AEggPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEggPawn();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* EggMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UCameraComponent* Camera;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Sound")
	USoundBase* CrackSound;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Sound")
	USoundBase* DeathSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float TorqueMagnitude = 12.f*PI;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float JumpMagnitude = 1000.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float DriftMagnitude = 200.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ground")
	float GroundSphereRadius = 100.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ground")
	float GroundTraceLength = 10.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ground")
	bool bDrawGroundSphere = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Camera")
	float CameraRotationSpeedDegrees = 180.f;

	//Impulse required to cause damage
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HealthDamage")
	float BaseImpulse = 75000.f;

	//Damage = BaseDamage*Impulse/BaseImpulse
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HealthDamage")
	float BaseDamage = 0.3f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="HealthDamage")
	float StartingHealth = 1.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="HealthDamage")
	float Health = 1.f;

	UFUNCTION()
	void HitResponse(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Decals")
	TArray<UDecalComponent*> Decals;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Decals")
	FVector DecalSize = FVector::OneVector;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Decals")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Decals")
	int32 MaxDecals = 8;

	//Actor to spawn when the character dies (Mesh of Egg white and yolk)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HealthDamage")
	TSubclassOf<AActor> EggSplatClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HealthDamage")
	TSubclassOf<AActor> EggCrackClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float JumpReadyDelay = 0.25f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HealthDamage")
	float DeathDelay = 2.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float FallSpeed = -400.f;
	
	void Respawn();
	
private:

	UPROPERTY()
	AActor* EggSplat;

	UPROPERTY()
	class AEggCrack* EggCrack;
	
	UPROPERTY()
	class APlayerStart* PlayerStart;
	
	FTimerHandle JumpTimerHandle;

	FTimerHandle DeathTimerHandle;
	
	//Player Input
	void MoveHorizontal(float InputX);
	void MoveVertical(float InputY);
	void MoveCameraHorizontal(float InputX);
	void Jump();
	void JumpReady();
	
	//Decals for egg cracking	
	void RemoveDecal();
	void SpawnCrackDecal(FVector HitWorldLocation,FVector HitWorldNormal);

	//Additional movement functions
	bool IsTouchingGround();
	bool bJumpReady = true;

	//State control
	void SpawnEggSplat(FVector HitWorldLocation,FVector HitWorldNormal);
	void SpawnEggCrack(FVector HitWorldLocation,FVector HitWorldImpulse,FVector CrackVelocity);
	void SetEnabled(bool bEnabled);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
