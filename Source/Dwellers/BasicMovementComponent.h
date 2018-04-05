// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "Tiles.h"
#include "BasicMovementComponent.generated.h"

/// <summary>
/// Point used for pathfinding which contains the point's location and the tile's speed.
/// </summary>
class DWELLERS_API FMovementPoint
{
public:
	FVector location;
	float speed;

	FMovementPoint::FMovementPoint(FVector Loc, float Spd)
	{
		location = Loc;
		speed = Spd;
	}
};

/// <summary>
/// Simple movement component which can follow a defined Path
/// </summary>
UCLASS()
class DWELLERS_API UBasicMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

	UBasicMovementComponent(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Whether the Actor should be destroyed when it reaches the End of its Path.
	/// </summary>
	bool bDestroyOnEnd;

	/// <summary>
	/// The owning Actor
	/// </summary>
	class AEntity* OwningActor;

public:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/// <summary>
	/// Add an array of locations used for pathfinding.
	/// Clears any previous paths.
	/// </summary>
	/// <param Name="tiles">The Path to follow</param>
	void AddPoints(TArray<TTile*> Tiles);

	/// <summary>
	/// Sets whether the Actor should be destroyed when they reach the End of the Path.
	/// </summary>
	/// <param Name="bDestroy">Destroy Actor</param>
	void DestroyOnEnd(bool bDestroy);
	
	/// <summary>
	/// The Path to follow.
	/// </summary>
	TArray<FMovementPoint*> Points;
};
