// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GodCamera.generated.h"

UCLASS()
class DWELLERS_API AGodCamera : public APawn
{
	GENERATED_BODY()

public:
	AGodCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// <summary>
	/// The player camera
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	/// <summary>
	/// The springarm for the camera
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USpringArmComponent* SpringArm;
};
