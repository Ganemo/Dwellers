// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicMovementComponent.h"
#include "Entity.h"
#include "Dwellers.h"

UBasicMovementComponent::UBasicMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUpdateOnlyIfRendered = false;

	bWantsInitializeComponent = true;

	bDestroyOnEnd = false;
}

void UBasicMovementComponent::BeginPlay()
{
	UE_LOG(LogMyGame, Warning, TEXT("Movement For %s Added"), *this->GetName());

}

void UBasicMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	AEntity* act = Cast<AEntity>(GetOwner());

	if (act == nullptr)
		return;

	if (points.Num() > 0)
	{
		if (act->GetActorLocation().Equals(points[0]->location))
			points.RemoveAt(0);

		if (points.Num() > 0)
		{
			FVector vec = points[0]->location - act->GetActorLocation();
			vec.Normalize();

			float dist = FVector::Dist(points[0]->location, act->GetActorLocation());
			float spd = act->Speed + (1 - points[0]->speed)*act->Speed;

			if (dist < spd)
			{
				vec *= dist;
			}
			else
			{
				vec *= spd;
			}

			this->Velocity = vec;
			this->UpdateComponentVelocity();

			act->AddActorLocalOffset(vec);
		}
	}
	else if (bDestroyOnEnd)
	{
		act->Destroy();
	}
}

void UBasicMovementComponent::AddPoints(TArray<TTile*> tiles)
{

	UE_LOG(LogMyGame, Warning, TEXT("Adding Points For %s"), *this->GetName());

	points.Empty();

	for (int x = 0; x < tiles.Num(); ++x)
		points.Add(new FMovementPoint(tiles[x]->GetTileLocationAsVector(), tiles[x]->GetCost()));
}

void UBasicMovementComponent::DestroyOnEnd(bool set)
{
	bDestroyOnEnd = set;
}