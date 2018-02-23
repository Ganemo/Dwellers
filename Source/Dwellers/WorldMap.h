// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldChunk.h"
#include "Tiles.h"
#include "TileObject.h"

/**
 * 
 */
class DWELLERS_API WorldMap
{
public:
	WorldMap();
	~WorldMap();

	class TTile*** tiles;

	TTile* GetTile(int x, int y);
	TTile* GetTileAtLocation(FVector loc);
	bool IsTileInChunk(TTile* tile);

	int mapsize = 0;
	int chunksize = 0;

	int cellsize = 0;
	float cliffheight = 0;

	TArray<TMap<TTile*, TTile*>> heightchunks;

	TArray<AWorldChunk*> chunks;
	TMap<int, int> waterchunkheight;

	AWorldChunk* FindChunkWithTile(TTile* tile);
	void MakeTileRoad(TTile* tile);

private:

	int GetPointFromReference(int xpos, int ypos, int xdiff, int ydiff);
};
