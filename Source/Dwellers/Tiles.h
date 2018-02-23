// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEncapsulator.h"
#include "TileObject.h"
#include "FunctionLibrary.h"

/**
 * 
 */
enum ETileType
{
	Ground,
	WaterEdge,
	Water,
	Cliff,
};

class DWELLERS_API FTileLoc
{

public:
	int x;
	int y;

	FTileLoc::FTileLoc(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

class DWELLERS_API TTile
{

public:
	FTileLoc * location;
	float height;

	int chunknum = -1;

	ETileType type;

	class FTileObject* object = nullptr;

	TTile::TTile() {}

	TTile::TTile(FTileLoc* loc, float hght)
	{
		location = loc;
		height = hght;
	}

	void TTile::RemoveObject();

	virtual bool TTile::IsTraversable() { return true; }
	virtual FColor TTile::GetColor() { return FColor(); }
	virtual float TTile::GetCost() { return 1.0f; }
};

class DWELLERS_API TCliffTile : public TTile
{
	bool IsStairs = false;

public:
	TCliffTile::TCliffTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		type = ETileType::Cliff;
	}

	bool TCliffTile::IsTraversable() override
	{
		return IsStairs;
	}

	FColor TCliffTile::GetColor() override
	{
		return FColor(255, 0, 0, 0);
	}
};

class DWELLERS_API TWaterEdgeTile : public TTile
{
	bool IsBridge = false;

public:

	TWaterEdgeTile::TWaterEdgeTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		type = ETileType::WaterEdge;
	}

	bool TWaterEdgeTile::IsTraversable() override
	{
		return IsBridge;
	}

	FColor TWaterEdgeTile::GetColor() override
	{
		return FColor(0, 0, 255, 0);
	}
};

class DWELLERS_API TGroundTile : public TTile
{

public:
	float grassDensity = 0;

	TGroundTile::TGroundTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		type = ETileType::Ground;
	}

	bool TGroundTile::IsTraversable() override
	{
		return GetCost() > 0;
	}

	FColor TGroundTile::GetColor() override
	{
		return FColor(0, grassDensity * 254, 0, 0);
	}

	float TGroundTile::GetCost() override;
};

class DWELLERS_API TWaterTile : public TTile
{

public:
	TWaterTile::TWaterTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		type = ETileType::Water;
	}

	bool TWaterTile::IsTraversable() override
	{
		return true;
	}

	FColor TWaterTile::GetColor() override
	{
		return FColor(0, 0, 255, 0);
	}
};



