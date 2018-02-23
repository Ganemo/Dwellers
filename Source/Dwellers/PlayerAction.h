// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles.h"
#include "TileObject.h"
#include "TileMovementFunctions.h"

/**
 * 
 */
class DWELLERS_API PlayerAction
{
public:
	PlayerAction::PlayerAction() {}
	virtual PlayerAction::~PlayerAction() {}

	virtual void PlayerAction::ClickDown(TTile* hittile) {}
	virtual void PlayerAction::ClickTick(TTile* hittile) {}
	virtual void PlayerAction::ClickUp(TTile* hittile) {}
};

class DWELLERS_API PlayerAction_MakeRoad : public PlayerAction
{
private:
	TTile* firsttile;

public:
	PlayerAction_MakeRoad::~PlayerAction_MakeRoad() override {}

	void PlayerAction_MakeRoad::ClickDown(TTile* hittile) override
	{
		firsttile = hittile;
	}

	void PlayerAction_MakeRoad::ClickUp(TTile* hittile) override
	{
		if (firsttile == hittile)
		{
			GameEncapsulator::GetGame()->map->MakeTileRoad(firsttile);
		}
		else
		{
			TArray<TTile*> tiles = UTileMovementFunctions::GetTilePath_NoWeight(firsttile, hittile, true);

			for (int x = 0; x < tiles.Num(); ++x)
			{
				GameEncapsulator::GetGame()->map->MakeTileRoad(tiles[x]);
			}
		}

		firsttile = nullptr;
	}

};
