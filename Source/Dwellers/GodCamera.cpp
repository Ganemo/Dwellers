// Fill out your copyright notice in the Description page of Project Settings.

#include "GodCamera.h"
#include "Components/InputComponent.h"

// Sets default values
AGodCamera::AGodCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring");
	SpringArm->SetupAttachment(RootComponent);
	//SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	//Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);

	SpringArm->TargetArmLength = 3000;
	SpringArm->SetWorldRotation(FRotator(-45, 0, 0));
	SpringArm->ProbeChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

// Called when the game starts or when spawned
void AGodCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGodCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGodCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

