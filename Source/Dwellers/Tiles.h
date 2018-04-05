// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEncapsulator.h"
#include "TileObject.h"
#include "FunctionLibrary.h"

/// <summary>
/// The category a tile is in
/// </summary>
enum ETileType
{
	Ground,
	WaterEdge,
	Water,
	Cliff,
};

/// <summary>
/// Structure containing tile locations
/// </summary>
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

/// <summary>
/// Tile structure used to hold tile information
/// </summary>
class DWELLERS_API TTile
{

public:
	FTileLoc * location;
	float Height;

	int ChunkNum = -1;

	ETileType TileType;

	class FTileObject* object = nullptr;

	TTile::TTile() {}

	TTile::TTile(FTileLoc* loc, float hght)
	{
		location = loc;
		Height = hght;
	}

	/// <summary>
	/// Get the Tile's location, represented as a standard 3D vector
	/// </summary>
	FVector GetTileLocationAsVector();

	/// <summary>
	/// Remove the object on the tile, calling it's removal functions aswell
	/// </summary>
	void RemoveObject();

	/// <summary>
	/// Whether this tile can be walked over by a movement actor
	/// </summary>
	/// <returns></returns>
	virtual bool TTile::IsTraversable() { return true; }
	
	/// <summary>
	/// Get the original color of the tile
	/// </summary>
	/// <returns></returns>
	virtual FColor TTile::GetColor() { return FColor(); }

	/// <summary>
	/// Get the cost to traverse this tile, takes in to account the tile object
	/// </summary>
	/// <returns></returns>
	virtual float TTile::GetCost() { return 1.0f; }
};

/// <summary>
/// A tile representing a tile that is between at least two tiles of different heights
/// </summary>
class DWELLERS_API TCliffTile : public TTile
{
	bool IsStairs = false;

public:
	TCliffTile::TCliffTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		TileType = ETileType::Cliff;
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

/// <summary>
/// A tile representing a tile that is between one water tile and one land tile
/// </summary>
class DWELLERS_API TWaterEdgeTile : public TTile
{
	bool IsBridge = false;

public:

	TWaterEdgeTile::TWaterEdgeTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		TileType = ETileType::WaterEdge;
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

/// <summary>
/// A tile representing a normal ground tile, which can have foliage and be built on
/// </summary>
class DWELLERS_API TGroundTile : public TTile
{

public:
	float grassDensity = 0;

	TGroundTile::TGroundTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		TileType = ETileType::Ground;
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

/// <summary>
/// A tile representing a water tile
/// </summary>
class DWELLERS_API TWaterTile : public TTile
{

public:
	TWaterTile::TWaterTile(FTileLoc* loc, float hght) : TTile(loc, hght)
	{
		TileType = ETileType::Water;
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



