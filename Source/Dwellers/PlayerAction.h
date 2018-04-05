// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles.h"
#include "TileObject.h"
#include "TileMovementFunctions.h"

/// <summary>
/// An action that can be fired from player mouse clicks,
/// Pure abstract class, should not be instantiated
/// </summary>
class DWELLERS_API PlayerAction
{
public:
	PlayerAction::PlayerAction() {}
	virtual PlayerAction::~PlayerAction() {}

	/// <summary>
	/// Function fired by clicking down
	/// </summary>
	/// <param Name="HitTile">The tile hit when this function is called</param>
	virtual void PlayerAction::ClickDown(TTile* HitTile) = 0;

	/// <summary>
	/// Function fired every frame
	/// </summary>
	/// <param Name="HitTile">The tile hit when this function is called</param>
	virtual void PlayerAction::ClickTick(TTile* HitTile) = 0;

	/// <summary>
	/// Function fired by clicking up
	/// </summary>
	/// <param Name="HitTile">The tile hit when this function is called</param>
	virtual void PlayerAction::ClickUp(TTile* HitTile) = 0;
};

/// <summary>
/// Makes a road between two tiles
/// </summary>
class DWELLERS_API PlayerAction_MakeRoad : public PlayerAction
{
private:
	TTile* FirstTile;

public:
	PlayerAction_MakeRoad::~PlayerAction_MakeRoad() override {}

	/// <summary>
	/// Store the first tile hit when clicked
	/// </summary>
	/// <param Name="HitTile">The tile hit</param>
	void PlayerAction_MakeRoad::ClickDown(TTile* HitTile) override
	{
		FirstTile = HitTile;
	}

	void PlayerAction_MakeRoad::ClickTick(TTile* HitTile) override {}

	/// <summary>
	/// Make a road between tiles
	/// </summary>
	/// <param Name="HitTile">The tile hit</param>
	void PlayerAction_MakeRoad::ClickUp(TTile* HitTile) override
	{
		//If the same tile is hit again, make that tile a road tile
		if (FirstTile == HitTile)
			GameEncapsulator::GetGame()->Map->MakeTileRoad(FirstTile);
		else
		{
			//Get a Path between tile locations
			TArray<TTile*> Tiles = UTileMovementFunctions::GetTilePath_NoWeight(FirstTile, HitTile, true);

			//Made all the tiles in the Path a road
			for (int x = 0; x < Tiles.Num(); ++x)
			{
				GameEncapsulator::GetGame()->Map->MakeTileRoad(Tiles[x]);
			}
		}

		//reset for the next call of this action
		FirstTile = nullptr;
	}
};
