// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Entity.h"
#include "AnimalHotSpot.generated.h"

/// <summary>
/// A spawnable Actor which spawns animals.
/// </summary>
UCLASS()
class DWELLERS_API AAnimalHotSpot : public AActor
{
	GENERATED_BODY()
	
public:	
	AAnimalHotSpot();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UBlueprint* BP;
	TArray<AEntity*> OwningAnimals;
};
