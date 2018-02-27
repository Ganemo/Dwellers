// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "Tiles.h"
#include "BasicMovementComponent.generated.h"

class DWELLERS_API FMovementPoint
{
public:
	FVector location;
	float speed;

	FMovementPoint::FMovementPoint(FVector loc, float spd)
	{
		location = loc;
		speed = spd;
	}
};

/**
 * 
 */
UCLASS()
class DWELLERS_API UBasicMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

	UBasicMovementComponent(const FObjectInitializer& ObjectInitializer);

	bool bDestroyOnEnd;

public:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void AddPoints(TArray<TTile*> tiles);
	void DestroyOnEnd(bool set);
	
	TArray<FMovementPoint*> points;
};
