// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DirectionalLight.h"
#include "DwellersGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DWELLERS_API ADwellersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:
	ADwellersGameModeBase();

	void BeginPlay() override;
	
	ADirectionalLight* light;
};
