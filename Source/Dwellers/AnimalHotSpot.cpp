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

// Sets default values
AAnimalHotSpot::AAnimalHotSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = FMath::RandRange(10,20);

	static ConstructorHelpers::FObjectFinder<UBlueprint> animalbp(TEXT("Blueprint'/Game/Blueprint/Entities/Deer/Doe.Doe'"));
	bp = animalbp.Object;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

}

// Called when the game starts or when spawned
void AAnimalHotSpot::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogMyGame, Warning, TEXT("Animal Loc %f-%f"), this->GetActorLocation().X, this->GetActorLocation().Y);
	
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	TTile* base = GameEncapsulator::GetGame()->map->GetTileAtLocation(this->GetActorLocation());

	int randcount = FMath::RandRange(2, 5);
	for (int x = 0; x < randcount; x++)
	{
		FVector loc = this->GetActorLocation() + FVector(100 * FMath::RandRange(-4, 4), 100 * FMath::RandRange(-4, 4), 0);

		TTile* ti = GameEncapsulator::GetGame()->map->GetTileAtLocation(loc);

		if (ti == nullptr || !ti->IsTraversable() || base->chunknum != ti->chunknum)
			loc = this->GetActorLocation();

		AActor* act = GetWorld()->SpawnActor(bp->GeneratedClass, new FVector(loc), new FRotator(0, 0, 0), params);
		if (AEntity* ent = Cast<AEntity>(act))
		{
			OwningAnimals.Add(ent);
		}
	}


}

// Called every frame
void AAnimalHotSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UTileMovementFunctions::AsyncMoveActorTo(OwningAnimals[animaltotick], this->GetActorLocation() + GameEncapsulator::GetGame()->map->cellsize * FVector(FMath::RandRange(-5, 5), FMath::RandRange(-5, 5), 0));
	//this->SetActorTickInterval(FMath::RandRange(10, 20));

	animaltotick++;
	if (animaltotick >= OwningAnimals.Num())
		animaltotick = 0;

}

