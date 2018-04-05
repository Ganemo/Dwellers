// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity.h"
#include "Kismet/GameplayStatics.h"

AGodCamera* AEntity::Player = nullptr;

AEntity::AEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .01f;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Collision = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SkeletalMesh->bAbsoluteRotation = false;
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SkeletalMesh->SetComponentTickEnabled(false);
	SkeletalMesh->SetComponentTickInterval(0);

	MovementComponent = CreateDefaultSubobject<UBasicMovementComponent>("Movement");
	MovementComponent->bAutoActivate = true;
	MovementComponent->bWantsInitializeComponent = true;
	MovementComponent->bTickBeforeOwner = false;
}

void AEntity::BeginPlay()
{
	Super::BeginPlay();
}

void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetUpdateBasedOnDistanceFromPlayer(DeltaTime);
}

void AEntity::SetUpdateBasedOnDistanceFromPlayer(float DT)
{
	//Get the player or make sure it's valid
	if (Player == nullptr || !Player->IsValidLowLevel())
	{
		Player = Cast<AGodCamera>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	//Ensure that the player an it's camera are still valid
	if (Player != nullptr && Player->Camera != nullptr)
	{
		//Get the distance from this Actor to the player
		float Dist = FVector::Distance(this->GetActorLocation(), Player->Camera->GetComponentLocation());

		//Update animations if close enough
		if (Dist <= 8000)
		{
			SkeletalMesh->SetComponentTickEnabled(true);
		}
		else
		{
			SkeletalMesh->SetComponentTickEnabled(false);
		}

		//Rotate mesh in the direction of motion if close enough
		if (Dist <= 15000)
		{
			//Get this Actor's Current velocity
			FVector CurrentVelocity = this->GetVelocity();
			//Make sure it's not stopped
			if (CurrentVelocity.Size() > 0)
			{
				//Find the new rotation, in the direction of motion.
				FRotator NewRotation = (CurrentVelocity.Rotation()) + FRotator(0, -90, 0);
				//Only rotate the yaw
				NewRotation.Pitch = 0;
				NewRotation.Roll = 0;

				//Interpolate to the new rotation
				SkeletalMesh->SetWorldRotation(FMath::RInterpTo(SkeletalMesh->GetComponentRotation(), NewRotation, DT, CurrentVelocity.Size() / 2));
			}
		}
	}
}
