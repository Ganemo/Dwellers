// Fill out your copyright notice in the Description page of Project Settings.

#include "GodCameraController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameEncapsulator.h"
#include "WorldMap.h"
#include "WorldChunk.h"
#include "TileObject.h"
#include "Kismet/GameplayStatics.h"

const FName TraceTag("PlayerClickQuery");

AGodCameraController::AGodCameraController()
{
}

void AGodCameraController::BeginPlay()
{
	this->bShowMouseCursor = true;
	this->bEnableClickEvents = true;
	this->bEnableMouseOverEvents = true;

	//Make Current player action to making a road on click
	CurrentAction = new PlayerAction_MakeRoad();
}

void AGodCameraController::Possess(APawn* Player)
{
	ControllingPlayer = Cast<AGodCamera>(Player);

	//Make the player camera the main camera
	this->SetViewTarget(Player);
	this->AttachToPawn(Player);
}

void AGodCameraController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind input dispatches
	this->InputComponent->BindAxis("Zoom", this, &AGodCameraController::Zoom);
	this->InputComponent->BindAxis("Move_Right", this, &AGodCameraController::Move_Right);
	this->InputComponent->BindAxis("Move_Forward", this, &AGodCameraController::Move_Forward);
	this->InputComponent->BindAxis("Rotate", this, &AGodCameraController::Rotate);

	this->InputComponent->BindAction("LeftClick", EInputEvent::IE_Pressed, this, &AGodCameraController::LeftClick_Down);
	this->InputComponent->BindAction("LeftClick", EInputEvent::IE_Released, this, &AGodCameraController::LeftClick_Up);
}

void AGodCameraController::Tick(float DT)
{
	DoHitTrace();
	DrawDebugBoxOnTile();
	UpdatePlayerAction();
}

void AGodCameraController::Zoom(float AxisVal)
{
	//Make sure the player being controlled is valid
	if (ControllingPlayer != nullptr)
	{
		//Increase or decrease the camera distance
		ControllingPlayer->SpringArm->TargetArmLength += 50 * AxisVal;

		//Make sure the arm length is not negative
		if(ControllingPlayer->SpringArm->TargetArmLength < 0)
			ControllingPlayer->SpringArm->TargetArmLength = 0;
	}
}
void AGodCameraController::Move_Right(float AxisVal)
{
	//Make sure the player being controlled is valid
	if (ControllingPlayer != nullptr)
	{
		//Move the camera left or right
		ControllingPlayer->AddActorLocalOffset(FVector(0, AxisVal * (15 * (ControllingPlayer->SpringArm->TargetArmLength / 3000)), 0));
	}
}
void AGodCameraController::Move_Forward(float AxisVal)
{
	//Make sure the player being controlled is valid
	if (ControllingPlayer != nullptr)
	{
		//Move the the camera forward or back
		ControllingPlayer->AddActorLocalOffset(FVector(AxisVal * (15 * (ControllingPlayer->SpringArm->TargetArmLength / 3000)), 0, 0));
	}
}
void AGodCameraController::Rotate(float AxisVal)
{
	//Make sure the player being controlled is valid
	if (ControllingPlayer != nullptr)
	{
		//Rotate the player around, only changing the yaw
		ControllingPlayer->AddActorWorldRotation(FRotator(0, AxisVal * 1, 0));
	}
}

void AGodCameraController::LeftClick_Down()
{
	//Fire the Current action's click down event
	if (CurrentTile != nullptr)
	{
		CurrentAction->ClickDown(CurrentTile);
	}
}

void AGodCameraController::LeftClick_Up()
{
	//Fire the Current action's click up event
	if (CurrentTile != nullptr)
	{
		CurrentAction->ClickUp(CurrentTile);
	}
}

void AGodCameraController::DoHitTrace()
{
	FVector Location;
	FVector Direction;

	//Get the mouse location in world space
	if (DeprojectMousePositionToWorld(Location, Direction))
	{
		//initialize the trace params
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = false;
		RV_TraceParams.bTraceAsyncScene = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		//Tag used for debug
		RV_TraceParams.TraceTag = TraceTag;

		//Do the line trace
		GetWorld()->LineTraceSingleByChannel(
			RV_Hit,        
			Location,    
			Location + (Direction * 20000), 
			ECC_GameTraceChannel2,
			RV_TraceParams
		);

		//Check if the hit is a blocking hit
		if (RV_Hit.bBlockingHit)
		{
			//Get the tile at the Current location
			CurrentTile = GameEncapsulator::GetGame()->Map->GetTileAtLocation(
				FVector(
					RV_Hit.ImpactPoint.X + GameEncapsulator::GetGame()->Map->CellSize / 2,
					RV_Hit.ImpactPoint.Y + GameEncapsulator::GetGame()->Map->CellSize / 2,
					0)
			);
		}
	}
}

void AGodCameraController::DrawDebugBoxOnTile()
{
	//Makes sure the tile is valid
	if (CurrentTile != nullptr)
	{
		//Draw the debug tile
		DrawDebugBox(
			GetWorld(),
			FVector(
				CurrentTile->location->x * GameEncapsulator::GetGame()->Map->CellSize,
				CurrentTile->location->y * GameEncapsulator::GetGame()->Map->CellSize,
				CurrentTile->Height * GameEncapsulator::GetGame()->Map->CliffHeight * GameEncapsulator::GetGame()->Map->CellSize),
			FVector(GameEncapsulator::GetGame()->Map->CellSize / 2, GameEncapsulator::GetGame()->Map->CellSize / 2, 1),
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}
}

void AGodCameraController::UpdatePlayerAction()
{
	//Update the Current tile's tick event
	if (CurrentTile != nullptr)
	{
		CurrentAction->ClickTick(CurrentTile);
	}
}