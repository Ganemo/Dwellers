// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimalHotSpot.h"
#include "ConstructorHelpers.h"
#include "Dwellers.h"
#include "Array.h"
#include "TileMovementFunctions.h"
#include "GameEncapsulator.h"
#include "TileObject.h"
#include "Engine/World.h"


const FName TraceTag("DeerLocationQuery");

AAnimalHotSpot::AAnimalHotSpot()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = FMath::RandRange(10,20);

	static ConstructorHelpers::FObjectFinder<UBlueprint> AnimalBP(TEXT("Blueprint'/Game/Blueprint/Entities/Deer/Doe.Doe'"));
	BP = AnimalBP.Object;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AAnimalHotSpot::BeginPlay()
{
	Super::BeginPlay();
	
	//Init spawn parameters
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	//Get the location of the hotspot
	TTile* Base = GameEncapsulator::GetGame()->Map->GetTileAtLocation(this->GetActorLocation());

	//Random number of animals
	int RandNum = FMath::RandRange(2, 5);
	for (int x = 0; x < RandNum; x++)
	{
		//Get a random location a max of 4 tiles away
		FVector Loc = this->GetActorLocation() + FVector(100 * FMath::RandRange(-4, 4), 100 * FMath::RandRange(-4, 4), 0);

		//Get the tile at that location
		TTile* Tile = GameEncapsulator::GetGame()->Map->GetTileAtLocation(Loc);

		//Make sure the tile is valid
		if (Tile == nullptr || !Tile->IsTraversable() || Base->ChunkNum != Tile->ChunkNum)
			Loc = this->GetActorLocation();

		//Spawn the animal and add it to owned animals array
		AActor* act = GetWorld()->SpawnActor(BP->GeneratedClass, new FVector(Loc), new FRotator(0, 0, 0), params);
		if (AEntity* ent = Cast<AEntity>(act))
		{
			OwningAnimals.Add(ent);
		}
	}
}

void AAnimalHotSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

