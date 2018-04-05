// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "BasicMovementComponent.h"
#include "GodCamera.h"
#include "Entity.generated.h"

/// <summary>
/// The basic entity for all simulated actors.
/// Has a capsule component, skeletal mesh, and Path movement component.
/// </summary>
UCLASS()
class DWELLERS_API AEntity : public AActor
{
	GENERATED_BODY()
	
public:
	AEntity();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// The Current player character
	/// </summary>
	static AGodCamera* Player;

	/// <summary>
	/// The Name of the object
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	FString Name;

	/// <summary>
	/// The health of the Actor (used for disasters and combat)
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	int Health = 10;

	/// <summary>
	/// The speed of movement for the Actor
	/// </summary>
	UPROPERTY(BlueprintReadWrite)
	int Speed = 10;

	/// <summary>
	/// The collision component
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UCapsuleComponent* Collision;

	/// <summary>
	/// 
	/// The skeletal mesh component
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMesh;

	/// <summary>
	/// The Path movement component
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UBasicMovementComponent* MovementComponent;

private:
	//Update animation or rotation if close enough to player
	void SetUpdateBasedOnDistanceFromPlayer(float DT);
};
