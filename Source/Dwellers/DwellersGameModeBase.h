// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DirectionalLight.h"
#include "DwellersGameModeBase.generated.h"

/// <summary>
/// Game mode for the basic game.
/// Spawns a directional light to use for a day/night cycle
/// </summary>
UCLASS()
class DWELLERS_API ADwellersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADwellersGameModeBase();

	void BeginPlay() override;
	
	/// <summary>
	/// Light used for day/night cycles
	/// </summary>
	ADirectionalLight* Light;
};
