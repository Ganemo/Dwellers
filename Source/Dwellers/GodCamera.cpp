// Fill out your copyright notice in the Description page of Project Settings.

#include "GodCamera.h"
#include "Components/InputComponent.h"

AGodCamera::AGodCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring");
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 3000;
	SpringArm->SetWorldRotation(FRotator(-45, 0, 0));
	SpringArm->ProbeChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void AGodCamera::BeginPlay()
{
	Super::BeginPlay();
}

void AGodCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGodCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

