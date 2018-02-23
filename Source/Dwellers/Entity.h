// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "GodCamera.h"
#include "Entity.generated.h"

UCLASS()
class DWELLERS_API AEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntity();

	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	int Health = 10;
	UPROPERTY(BlueprintReadWrite)
	int Speed = 10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UCapsuleComponent* Collision;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInterpToMovementComponent* MovementComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static AGodCamera* Player;
	
};
