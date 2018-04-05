// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEncapsulator.h"
#include "WorldMap.h"

GameEncapsulator* GameEncapsulator::gameencapsulator = nullptr;

GameEncapsulator::GameEncapsulator()
{
}

GameEncapsulator::~GameEncapsulator()
{
	delete Map;
}

void GameEncapsulator::CreateGame(int Mapsize, int Chunksize, int Cellsize, float Cellheight)
{
	//Destroy old game state if it exists
	if (gameencapsulator != nullptr)
		delete gameencapsulator;

	//Create the new singleton instance
	gameencapsulator = new GameEncapsulator();

	//Create the new world map
	gameencapsulator->Map = new WorldMap();

	//Initialize the map properties
	gameencapsulator->Map->MapSize = Mapsize;
	gameencapsulator->Map->ChunkSize = Chunksize;
	gameencapsulator->Map->CellSize = Cellsize;
	gameencapsulator->Map->CliffHeight = Cellheight;

	//Initialize the map's tiles to null
	gameencapsulator->Map->Tiles = new TTile**[Mapsize];
	for (int x = 0; x < Mapsize; ++x)
	{
		gameencapsulator->Map->Tiles[x] = new TTile*[Mapsize];
		for (int y = 0; y < Mapsize; ++y)
		{
			gameencapsulator->Map->Tiles[x][y] = nullptr;
		}
	}
}

GameEncapsulator* GameEncapsulator::GetGame()
{
	return gameencapsulator;
}