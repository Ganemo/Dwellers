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
{/*
	AGodCamera* pl = GetWorld()->SpawnActor<AGodCamera>();
	Possess(pl);*/

	this->bShowMouseCursor = true;
	this->bEnableClickEvents = true;
	this->bEnableMouseOverEvents = true;

	CurrentAction = new PlayerAction_MakeRoad();
}

void AGodCameraController::Possess(APawn* plyr)
{
	ControllingPlayer = Cast<AGodCamera>(plyr);
	this->SetViewTarget(plyr);
	this->AttachToPawn(plyr);
}

void AGodCameraController::SetupInputComponent()
{
	Super::SetupInputComponent();

	this->InputComponent->BindAxis("Zoom", this, &AGodCameraController::Zoom);
	this->InputComponent->BindAxis("Move_Right", this, &AGodCameraController::Move_Right);
	this->InputComponent->BindAxis("Move_Forward", this, &AGodCameraController::Move_Forward);
	this->InputComponent->BindAxis("Rotate", this, &AGodCameraController::Rotate);

	this->InputComponent->BindAction("LeftClick", EInputEvent::IE_Pressed, this, &AGodCameraController::LeftClick_Down);
	this->InputComponent->BindAction("LeftClick", EInputEvent::IE_Released, this, &AGodCameraController::LeftClick_Up);
}

void AGodCameraController::Zoom(float axisvalue)
{
	if (ControllingPlayer != nullptr)
	{
		ControllingPlayer->SpringArm->TargetArmLength += 50 * axisvalue;

		if(ControllingPlayer->SpringArm->TargetArmLength < 0)
			ControllingPlayer->SpringArm->TargetArmLength = 0;
	}
}
void AGodCameraController::Move_Right(float axisval)
{
	if (ControllingPlayer != nullptr)
	{
		ControllingPlayer->AddActorLocalOffset(FVector(0, axisval * (15 * (ControllingPlayer->SpringArm->TargetArmLength / 3000)), 0));
	}
}
void AGodCameraController::Move_Forward(float axisval)
{
	if (ControllingPlayer != nullptr)
	{
		ControllingPlayer->AddActorLocalOffset(FVector(axisval * (15 * (ControllingPlayer->SpringArm->TargetArmLength / 3000)), 0, 0));
	}
}
void AGodCameraController::Rotate(float axisval)
{
	if (ControllingPlayer != nullptr)
		ControllingPlayer->AddActorWorldRotation(FRotator(0, axisval * 1, 0));
}

void AGodCameraController::LeftClick_Down()
{
	FVector loc;
	FVector dir;

	if (DeprojectMousePositionToWorld(loc, dir))
	{
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = false;
		RV_TraceParams.bTraceAsyncScene = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.TraceTag = TraceTag;

		//Re-initialize hit info
		FHitResult RV_Hit(ForceInit);

		//call GetWorld() from within an actor extending class
		GetWorld()->LineTraceSingleByChannel(
			RV_Hit,        //result
			loc,    //start
			loc + (dir * 20000), //end
			ECC_GameTraceChannel2, //collision channel
			RV_TraceParams
		);

		/*DrawDebugLine(
			GetWorld(),
			loc,
			loc + (dir * 20000),
			FColor(255, 0, 0),
			false, 5, 0,
			12.333
		);*/

		if (RV_Hit.bBlockingHit)
		{
			/*UE_LOG(LogTemp, Log, TEXT("Click Hit!"), RV_Hit.ImpactPoint.X, RV_Hit.ImpactPoint.Y);
			UE_LOG(LogTemp, Log, TEXT("\tLocation:\t%f\t%f"), RV_Hit.ImpactPoint.X, RV_Hit.ImpactPoint.Y);
			UE_LOG(LogTemp, Log, TEXT("\tActor Hit:\t%s"), *RV_Hit.Actor->GetName());*/

			TTile* til = GameEncapsulator::GetGame()->map->GetTileAtLocation(FVector(RV_Hit.ImpactPoint.X + GameEncapsulator::GetGame()->map->cellsize / 2, RV_Hit.ImpactPoint.Y + GameEncapsulator::GetGame()->map->cellsize / 2, 0));
			if (til != nullptr)
			{
				/*UE_LOG(LogTemp, Log, TEXT("\tTile At:\t%f\t%f"), til->location->x, til->location->y);

				DrawDebugBox(
					GetWorld(),
					FVector(
						til->location->x * GameEncapsulator::GetGame()->cellsize,
						til->location->y * GameEncapsulator::GetGame()->cellsize,
						til->height * GameEncapsulator::GetGame()->cliffheight * GameEncapsulator::GetGame()->cellsize),
					FVector(GameEncapsulator::GetGame()->cellsize / 2, GameEncapsulator::GetGame()->cellsize / 2, 1),
					FColor(255, 0, 0),
					false, 5, 0,
					12.333
				);

				AWorldChunk* chnk = GameEncapsulator::GetGame()->FindChunkWithTile(til);
				if (chnk != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("\tIn Chunk:\t%s"), *chnk->GetName());
				}*/

				//GameEncapsulator::GetGame()->map->MakeTileRoad(til);
				CurrentAction->ClickDown(til);
			}

		}
	}

}

void AGodCameraController::LeftClick_Up()
{
	FVector loc;
	FVector dir;

	if (DeprojectMousePositionToWorld(loc, dir))
	{
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = false;
		RV_TraceParams.bTraceAsyncScene = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.TraceTag = TraceTag;

		//Re-initialize hit info
		FHitResult RV_Hit(ForceInit);

		//call GetWorld() from within an actor extending class
		GetWorld()->LineTraceSingleByChannel(
			RV_Hit,        //result
			loc,    //start
			loc + (dir * 20000), //end
			ECC_GameTraceChannel2, //collision channel
			RV_TraceParams
		);


		if (RV_Hit.bBlockingHit)
		{

			TTile* til = GameEncapsulator::GetGame()->map->GetTileAtLocation(FVector(RV_Hit.ImpactPoint.X + GameEncapsulator::GetGame()->map->cellsize / 2, RV_Hit.ImpactPoint.Y + GameEncapsulator::GetGame()->map->cellsize / 2, 0));
			if (til != nullptr)
			{
				CurrentAction->ClickUp(til);
				//GameEncapsulator::GetGame()->map->MakeTileRoad(til);
			}

		}
	}

}