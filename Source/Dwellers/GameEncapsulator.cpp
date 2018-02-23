// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEncapsulator.h"
#include "WorldMap.h"

GameEncapsulator* GameEncapsulator::gameencapsulator = nullptr;

GameEncapsulator::GameEncapsulator()
{
}

GameEncapsulator::~GameEncapsulator()
{
	delete map;
}

void GameEncapsulator::CreateGame(int mapsize, int chunksize, int cellsize, float cliffheight)
{
	if (gameencapsulator != nullptr)
		delete gameencapsulator;

	gameencapsulator = new GameEncapsulator();

	gameencapsulator->map = new WorldMap();

	gameencapsulator->map->mapsize = mapsize;
	gameencapsulator->map->chunksize = chunksize;
	gameencapsulator->map->cellsize = cellsize;
	gameencapsulator->map->cliffheight = cliffheight;

	gameencapsulator->map->tiles = new TTile**[mapsize];
	for (int x = 0; x < mapsize; ++x)
	{
		gameencapsulator->map->tiles[x] = new TTile*[mapsize];
		for (int y = 0; y < mapsize; ++y)
		{
			gameencapsulator->map->tiles[x][y] = nullptr;
		}
	}
}

GameEncapsulator* GameEncapsulator::GetGame()
{
	return gameencapsulator;
}