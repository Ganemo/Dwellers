// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GodCamera.h"
#include "PlayerAction.h"
#include "GodCameraController.generated.h"

/**
 * 
 */
UCLASS()
class DWELLERS_API AGodCameraController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGodCameraController();
	
	AGodCamera* ControllingPlayer;

	PlayerAction* CurrentAction = nullptr;

	virtual void BeginPlay() override;
	virtual void Possess(APawn* plyr) override;
	virtual void SetupInputComponent() override;


	void Zoom(float axisval);
	void Move_Right(float axisval);
	void Move_Forward(float axisval);
	void Rotate(float axisval);

	void LeftClick_Down();
	void LeftClick_Up();
};
