// Fill out your copyright notice in the Description page of Project Settings.

#include "DwellersGameModeBase.h"
#include "GodCamera.h"
#include "Engine/World.h"
#include "Components/DirectionalLightComponent.h"
#include "GodCameraController.h"


ADwellersGameModeBase::ADwellersGameModeBase()
{
	this->DefaultPawnClass = AGodCamera::StaticClass();
	this->PlayerControllerClass = AGodCameraController::StaticClass();
}

void ADwellersGameModeBase::BeginPlay()
{
	light = (ADirectionalLight*)GetWorld()->SpawnActor(ADirectionalLight::StaticClass());
	light->GetLightComponent()->bUseRayTracedDistanceFieldShadows = true;
	light->SetMobility(EComponentMobility::Movable);
}