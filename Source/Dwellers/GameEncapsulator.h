// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DWELLERS_API GameEncapsulator
{
public:
	GameEncapsulator();
	~GameEncapsulator();

	static void CreateGame(int mapsize, int chunksize, int cellsize, float cellheight);
	static GameEncapsulator* GetGame();

	class WorldMap* map;

	int mincull = 10000;
	int maxcull = 15000;

private:
	static GameEncapsulator* gameencapsulator;

};
