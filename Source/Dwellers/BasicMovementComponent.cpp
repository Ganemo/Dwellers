// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicMovementComponent.h"
#include "Entity.h"
#include "Dwellers.h"

UBasicMovementComponent::UBasicMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Need to update the movement always
	bUpdateOnlyIfRendered = false;
	bWantsInitializeComponent = true;
	//Should the Actor be destroyed when it reaches the End of the Path
	bDestroyOnEnd = false;
}

void UBasicMovementComponent::BeginPlay()
{
	OwningActor = Cast<AEntity>(GetOwner());
}

void UBasicMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (OwningActor == nullptr)
		return;

	//If there are still points to follow
	if (Points.Num() > 0)
	{
		//Remove a point if the Actor is at its location
		if (OwningActor->GetActorLocation().Equals(Points[0]->location))
			Points.RemoveAt(0);

		//Follow the next point
		if (Points.Num() > 0)
		{
			//Get the direction toward the next point
			FVector NewVel = Points[0]->location - OwningActor->GetActorLocation();
			NewVel.Normalize();

			//Get the distance to the next point
			float DistanceToNextPoint = FVector::Dist(Points[0]->location, OwningActor->GetActorLocation());
			//Get the speed of the Actor over the tile
			float Speed = OwningActor->Speed + (1 - Points[0]->speed)*OwningActor->Speed;

			//If the distance to the next point is less than the distance to move with speed
			//Move the remaining distance to the location.
			if (DistanceToNextPoint < Speed)
				NewVel *= DistanceToNextPoint;
			else
				NewVel *= Speed;

			//Set the velocity and update it 
			this->Velocity = NewVel;
			this->UpdateComponentVelocity();

			//Move the Actor
			OwningActor->AddActorLocalOffset(NewVel);
		}
	}
	//Destroy owner if End Path is reached and it should be destroyed
	else if (bDestroyOnEnd)
	{
		OwningActor->Destroy();
	}
}

void UBasicMovementComponent::AddPoints(TArray<TTile*> Tiles)
{
	UE_LOG(LogMyGame, Warning, TEXT("Adding Points For %s"), *this->GetName());

	//Clear all previous Path points
	Points.Empty();

	//Get tile locations and costs and stor them as FMovementPoints into the Path
	for (int x = 0; x < Tiles.Num(); ++x)
		Points.Add(new FMovementPoint(Tiles[x]->GetTileLocationAsVector(), Tiles[x]->GetCost()));
}

void UBasicMovementComponent::DestroyOnEnd(bool Set)
{
	bDestroyOnEnd = Set;
}