// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/// <summary>
/// Singleton instance containing all game information
/// </summary>
class DWELLERS_API GameEncapsulator
{
public:
	GameEncapsulator();
	~GameEncapsulator();

	/// <summary>
	/// Creates a map with parameters
	/// </summary>
	/// <param Name="Mapsize">The width and Height of the map</param>
	/// <param Name="Chunksize">The Size of a Chunk</param>
	/// <param Name="Cellsize">The Size of a cell</param>
	/// <param Name="Cellheight">The scale for one unit upwards between cells</param>
	static void CreateGame(int Mapsize, int Chunksize, int Cellsize, float Cellheight);
	
	/// <summary>
	/// Get the Singleton instance of the game
	/// </summary>
	/// <returns>The game instane</returns>
	static GameEncapsulator* GetGame();

	/// <summary>
	/// The map used for world spawning and navigation
	/// </summary>
	class WorldMap* Map;

	//Culling distances
	int MinCull = 10000;
	int MaxCull = 15000;

private:
	//The game instance
	static GameEncapsulator* gameencapsulator;

};
