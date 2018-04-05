// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathNavigation.h"

class TTile;
/**
 * 
 */
class DWELLERS_API WorldMap
{
public:
	WorldMap();
	~WorldMap();

	/// <summary>
	/// 2D array of pointers to tiles, representing the tile map
	/// </summary>
	class TTile*** Tiles;

	/// <summary>
	/// Get the tile at the indexes
	/// </summary>
	/// <param name="x">X location</param>
	/// <param name="y">Y location</param>
	/// <returns>The tile at the indexes</returns>
	TTile* GetTile(int x, int y);

	/// <summary>
	/// Get the tile at the world location
	/// </summary>
	/// <param name="Loc">The world location</param>
	/// <returns>The tile at the location</returns>
	TTile* GetTileAtLocation(FVector Loc);

	/// <summary>
	/// Check if the tile is inside a chunk
	/// </summary>
	/// <param name="Tile">The tile</param>
	/// <returns>Whether it is in a chunk</returns>
	bool IsTileInChunk(TTile* Tile);

	/// <summary>
	/// The width and height of the map
	/// </summary>
	int MapSize = 0;

	/// <summary>
	/// The width and height of each chunk
	/// </summary>
	int ChunkSize = 0;

	/// <summary>
	/// The width and height of each cell
	/// </summary>
	int CellSize = 0;
	
	/// <summary>
	/// The scale of the height for each cliff
	/// </summary>
	float CliffHeight = 0;

	/// <summary>
	/// The tile height chunks for navigation
	/// </summary>
	TArray<TMap<TTile*, TTile*>> HeightChunks;

	/// <summary>
	/// All of the chunks
	/// </summary>
	TArray<class AWorldChunk*> Chunks;

	/// <summary>
	/// The height of each water chunk
	/// </summary>
	TMap<int, int> WaterChunkHeight;

	/// <summary>
	/// Get the chunk that contains a tile
	/// </summary>
	/// <param name="Tile">The tile</param>
	/// <returns>The chunk containing the tile</returns>
	AWorldChunk* FindChunkWithTile(TTile* Tile);

	/// <summary>
	/// The handler used for navigation
	/// </summary>
	FPathNavigationHandler PathHandler;

	/// <summary>
	/// Turn a tile into a road tile
	/// </summary>
	/// <param name="Tile">The tile</param>
	void MakeTileRoad(TTile* Tile);
};
