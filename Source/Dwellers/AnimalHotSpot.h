// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Entity.h"
#include "AnimalHotSpot.generated.h"

UCLASS()
class DWELLERS_API AAnimalHotSpot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimalHotSpot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UBlueprint* bp;
	
	int animaltotick = 0;
	TArray<AEntity*> OwningAnimals;

	FCollisionQueryParams RV_TraceParams;
};
