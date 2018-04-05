// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GodCamera.h"
#include "PlayerAction.h"
#include "GodCameraController.generated.h"

/// <summary>
/// The controller used for player actions
/// </summary>
UCLASS()
class DWELLERS_API AGodCameraController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGodCameraController();
	
	/// <summary>
	/// The player being controlled
	/// </summary>
	AGodCamera* ControllingPlayer;

	/// <summary>
	/// The next action used for click events
	/// </summary>
	PlayerAction* CurrentAction = nullptr;

	virtual void BeginPlay() override;
	virtual void Possess(APawn* Player) override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DT) override;

	/// <summary>
	/// Change the zoom level
	/// </summary>
	/// <param Name="AxisVal">The amount to zoom</param>
	void Zoom(float AxisVal);

	/// <summary>
	/// Move the player right or left
	/// </summary>
	/// <param Name="AxisVal">The input value</param>
	void Move_Right(float AxisVal);

	/// <summary>
	/// Move the player forward or back
	/// </summary>
	/// <param Name="AxisVal">The input value</param>
	void Move_Forward(float AxisVal);

	/// <summary>
	/// Rotate the player right or left
	/// </summary>
	/// <param Name="AxisVal">The input value</param>
	void Rotate(float AxisVal);

	/// <summary>
	/// Function called when player clicks left click
	/// Fires the Current Action's click down function
	/// </summary>
	void LeftClick_Down();
	/// <summary>
	/// Function called when player lets go of left click
	/// Fires the Current Action's click up function
	/// </summary>
	void LeftClick_Up();

private:
	FHitResult RV_Hit;
	TTile* CurrentTile;

	/// <summary>
	/// Do a hit trace from the mouse position towards the map
	/// </summary>
	void DoHitTrace();

	/// <summary>
	/// Draw a debug box on the currently hovered tile
	/// </summary>
	void DrawDebugBoxOnTile();

	/// <summary>
	/// Update the Current player action
	/// </summary>
	void UpdatePlayerAction();
};
