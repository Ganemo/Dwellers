// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity.h"
#include "Kismet/GameplayStatics.h"

AGodCamera* AEntity::Player = nullptr;

// Sets default values
AEntity::AEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	/*MovementComponent->bPauseOnImpact = false;
	MovementComponent->BehaviourType = EInterpToBehaviourType::OneShot;*/
	MovementComponent->bTickBeforeOwner = false;
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player == nullptr || !Player->IsValidLowLevel())
	{
		Player = Cast<AGodCamera>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
	
	if (Player != nullptr && Player->Camera != nullptr)
	{
		float dist = FVector::Distance(this->GetActorLocation(), Player->Camera->GetComponentLocation());
		if (dist <= 8000)
		{
			SkeletalMesh->SetComponentTickEnabled(true);
		}
		else
		{
			SkeletalMesh->SetComponentTickEnabled(false);
		}

		if (dist <= 15000)
		{
			FVector vel = this->GetVelocity();
			if (vel.Size() > 0)
			{
				FRotator rot = (vel.Rotation()) + FRotator(0, -90, 0);
				rot.Pitch = 0;
				rot.Roll = 0;

				SkeletalMesh->SetWorldRotation(FMath::RInterpTo(SkeletalMesh->GetComponentRotation(), rot, DeltaTime, vel.Size()/2));
			}
		}
	}
	
}

