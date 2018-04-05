// Fill out your copyright notice in the Description page of Project Settings.

#include "TileMovementFunctions.h"
#include "Algo/Reverse.h"
#include "TileObject.h"
#include "Async/TaskGraphInterfaces.h"

/// <summary>
/// Get the tile at a world location
/// </summary>
/// <param Name="Location">The FVector location</param>
/// <param Name="CellSize">The Size of a cell on the map</param>
/// <param Name="Tiles">The 2d array of tiles</param>
/// <param Name="MapSize">The length and width of the Tiles array</param>
/// <returns>The Tile</returns>
TTile* GetTileAtLocation(FVector Location, int CellSize, TTile*** Tiles, int MapSize)
{
	//Convert the location to the tile indexes in Tiles
	int x = ((int)Location.X) / CellSize;
	int y = ((int)Location.Y) / CellSize;

	//Make sure the locations are valid
	if (x < 0 || x >= MapSize || y < 0 || y >= MapSize)
		return nullptr;

	return Tiles[x][y];
}

/// <summary>
/// Get the tiles adjacent to this tile
/// </summary>
/// <param Name="Tiles">The 2d array of tiles</param>
/// <param Name="Tile">The tile</param>
/// <param Name="MapSize">The length and width of the tiles array</param>
/// <returns></returns>
TArray<TTile*> GetAdjacentTiles(TTile*** Tiles, TTile* Tile, int MapSize)
{
	//The tiles to return
	TArray<TTile*> Adj;

	//Get the location of the tile
	int x = Tile->location->x;
	int y = Tile->location->y;

	//Get the tile to the right
	if(x + 1 < MapSize)
		Adj.Add(Tiles[x + 1][y]);
	//Get the tile to the left
	if (x - 1 >= 0)
		Adj.Add(Tiles[x - 1][y]);
	//Get the tile above
	if (y + 1 < MapSize)
		Adj.Add(Tiles[x][y + 1]);
	//Get the tile below
	if (y - 1 >= 0)
		Adj.Add(Tiles[x][y - 1]);

	return Adj;
}

/// <summary>
/// Check if two vectors are in a direct line on the map (no diagonals)
/// </summary>
/// <param Name="V1">The first vector</param>
/// <param Name="V1Height">The first vector's Height</param>
/// <param Name="V2">The second vector</param>
/// <param Name="V2Height">The second vector's Height</param>
/// <returns></returns>
bool AreVectorsInAStraightLine(FVector2D* V1, int V1Height, FVector2D* V2, int V2Height)
{
	//If they are the same Height and in a direct line
	if (((V1->X == V2->X) != (V1->Y == V2->Y)) && V1Height == V2Height)
		return true;
	else
		return false;
}

/// <summary>
/// Get a manhattan heuristic value
/// </summary>
/// <param Name="D">Heuristic scale</param>
/// <param Name="Current">The current tile</param>
/// <param Name="Target">The target tile</param>
/// <param Name="Cardinals">Whether you can use diagonals</param>
/// <returns>The heuristic</returns>
float GetHeuristic(float D, TTile* Current, TTile* Target, bool Cardinals)
{
	float a = FMath::Abs(Current->location->x - Target->location->x);
	float b = FMath::Abs(Current->location->y - Target->location->y);

	if (Cardinals)
		return D * (a + b);
	else
		return D * (a + b) - D * FMath::Min(a, b);
}

/// <summary>
/// Get a tile at index from 2D Map
/// </summary>
/// <param Name="x">The x index</param>
/// <param Name="y">The y index</param>
/// <param Name="Map">The 2D map</param>
/// <param Name="MapSize">The 2D map Height and width</param>
/// <returns></returns>
TTile* GetTile(int x, int y, TTile*** Map, int MapSize)
{
	if (x >= MapSize || y >= MapSize || x < 0 || y < 0)
		return nullptr;
	else
		return Map[x][y];
}

/// <summary>
/// Get all the neighbors of the current tile
/// </summary>
/// <param Name="Current">The current tile</param>
/// <param Name="Map">The 2D map</param>
/// <param Name="MapSize">The 2D map Height and width</param>
/// <param Name="Cardinals">Whether you can use diagonals</param>
/// <returns></returns>
TArray<TTile*> GetNeighbors(TTile* Current, TTile*** Map, int MapSize, bool Cardinals)
{
	//Get x and y index
	int x = Current->location->x;
	int y = Current->location->y;

	TArray<TTile*> Neigh;

	TTile* Tile;

	//Get the right tile
	Tile = GetTile(x + 1, y, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the above tile
	Tile = GetTile(x, y + 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the left tile
	Tile = GetTile(x - 1, y, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the below tile
	Tile = GetTile(x, y - 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Return if we are only using cardinal directions
	if (Cardinals)
		return Neigh;

	//Get the above right tile
	Tile = GetTile(x + 1, y + 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the below right tile
	Tile = GetTile(x + 1, y - 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the above right tile
	Tile = GetTile(x - 1, y + 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	//Get the below left tile
	Tile = GetTile(x - 1, y - 1, Map, MapSize);
	if (Tile != nullptr)
		Neigh.Add(Tile);

	return Neigh;
}

TArray<FVector> UTileMovementFunctions::GetTilePathFromPoints(FVector Start, FVector End, bool Cardinals)
{
	TArray<FVector> Locations;
	
	//Get the world map
	WorldMap* Map = GameEncapsulator::GetGame()->Map;

	TTile* StartingTile = Map->GetTileAtLocation(Start + GameEncapsulator::GetGame()->Map->CellSize / 2);
	TTile* EndingTile = Map->GetTileAtLocation(End + GameEncapsulator::GetGame()->Map->CellSize / 2);

	//Make sure both tiles are valid
	if (StartingTile == nullptr || EndingTile == nullptr)
		return Locations;

	//Make sure both tiles are in the same Chunk
	if (StartingTile->ChunkNum != EndingTile->ChunkNum)
		return Locations;

	TArray<TTile*> Path;

	UTileMovementFunctions::GetTilePath(&Path, Map->Tiles, Map->MapSize, StartingTile, EndingTile, Cardinals);

	//Convert tiles to location
	int tilecount = Path.Num();
	int Next = 0;
	for (int x = 0; x < tilecount; x = Next)
	{
		Locations.Add(FVector(Path[x]->location->x * Map->CellSize, Path[x]->location->y * Map->CellSize, Path[x]->Height * (Map->CellSize * Map->CliffHeight)));
		Next = x + 1;
	}

	return Locations;
}

TArray<TTile*> UTileMovementFunctions::GetTilePath(TTile* Start, TTile* End, bool Cardinals)
{
	//Get the world map
	WorldMap* Map = GameEncapsulator::GetGame()->Map;

	TArray<TTile*> Path;

	//Make sure both tiles are valid
	if (Start == nullptr || End == nullptr)
		return Path;

	//Make sure both tiles are in the same Chunk
	if (Start->ChunkNum != End->ChunkNum)
		return Path;

	UTileMovementFunctions::GetTilePath(&Path, Map->Tiles, Map->MapSize, Start, End, Cardinals);

	return Path;
}

TArray<TTile*> UTileMovementFunctions::GetTilePath_NoWeight(TTile* Start, TTile* End, bool Cardinals)
{
	//Get the world map
	WorldMap* Map = GameEncapsulator::GetGame()->Map;

	TArray<TTile*> Path;

	//Make sure both tiles are valid
	if (Start == nullptr || End == nullptr)
		return Path;

	//Make sure both tiles are in the same Chunk
	if (Start->ChunkNum != End->ChunkNum)
		return Path;

	UTileMovementFunctions::GetTilePath_NoWeight(&Path, Map->Tiles, Map->MapSize, Start, End, Cardinals);

	return Path;
}

void UTileMovementFunctions::AsyncMoveActorTo(AEntity* Target, FVector End, bool Cardinals)
{
	//Start a new async pathfinding task
	FAutoDeleteAsyncTask<FFindPathTask>* Task = new FAutoDeleteAsyncTask<FFindPathTask>(Target, End, Cardinals);
	Task->StartBackgroundTask();
}

void UTileMovementFunctions::GetTilePath(TArray<TTile*>* Path, TTile*** Tiles, int MapSize, TTile* Start, TTile* End, bool Cardinals)
{
	PriorityQueue Frontier;
	Frontier.AddNode(new PriorityNode(Start, 0));

	TMap<TTile*, TTile*> CameFrom;
	CameFrom.Add(Start, Start);
	TMap<TTile*, float> CostSoFar;
	CostSoFar.Add(Start, 0);

	bool bEndReached = false;

	/////Perform A* pathfinding//////
	 
	//Loop while frontier contains elements
	while (!Frontier.IsEmpty())
	{
		//Pluck from the priority queue
		TTile* Current = Frontier.Pluck();

		// The end is found, break
		if (Current == End)
		{
			bEndReached = true;
			break;
		}

		//Get the neighbors of the current tile
		TArray<TTile*> Neighbors = GetNeighbors(Current, Tiles, MapSize, Cardinals);

		//Iterate over neighbors
		for (TTile* Next : Neighbors)
		{
			//Only look at valid, traversable tiles
			if (Next == nullptr || !Next->IsTraversable())
				continue;

			//Calculate the costs to get to this node
			float NewCost = CostSoFar[Current] + Next->GetCost();

			//See if the tile has been checked or if it has a higher path cost
			if (CostSoFar.Find(Next) == nullptr || NewCost < CostSoFar[Next])
			{
				CostSoFar.Add(Next, NewCost);
				//Get the path heuristic used for priority
				float Priority = GetHeuristic(Next->GetCost(), Next, End, Cardinals);
				Frontier.AddNode(new PriorityNode(Next, Priority));
				CameFrom.Add(Next, Current);
			}
		}
	}

	//End if we never reached the end tile
	if (bEndReached == false)
		return;

	//Backtrace the path
	TTile* n = End;
	while (n != Start)
	{
		Path->Add(n);
		TTile** a = CameFrom.Find(n);
		if (a == nullptr)
			return;
		else
			n = *a;
	}

	Path->Add(Start);
	
	//Reverse the path
	Algo::Reverse(*Path);
}

void UTileMovementFunctions::GetTilePath_NoWeight(TArray<TTile*>* Path, TTile*** Tiles, int MapSize, TTile* Start, TTile* End, bool Cardinals)
{
	PriorityQueue Frontier;
	Frontier.AddNode(new PriorityNode(Start, 0));

	TMap<TTile*, TTile*> CameFrom;
	CameFrom.Add(Start, Start);
	TMap<TTile*, float> CostSoFar;
	CostSoFar.Add(Start, 0);

	bool bEndReached = false;

	/////Perform A* pathfinding//////

	//Loop while frontier contains elements
	while (!Frontier.IsEmpty())
	{
		//Pluck from the priority queue
		TTile* Current = Frontier.Pluck();

		// The end is found, break
		if (Current == End)
		{
			bEndReached = true;
			break;
		}

		//Get the neighbors of the current tile
		TArray<TTile*> Neighbors = GetNeighbors(Current, Tiles, MapSize, Cardinals);

		//Iterate over neighbors
		for (TTile* Next : Neighbors)
		{
			//Only look at valid, traversable tiles
			if (Next == nullptr || !Next->IsTraversable())
				continue;

			//Calculate the costs to get to this node
			float NewCost = CostSoFar[Current] + 1;

			//See if the tile has been checked or if it has a higher path cost
			if (CostSoFar.Find(Next) == nullptr || NewCost < CostSoFar[Next])
			{
				CostSoFar.Add(Next, NewCost);
				//Get the path heuristic used for priority
				float Priority = GetHeuristic(Next->GetCost(), Next, End, Cardinals);
				Frontier.AddNode(new PriorityNode(Next, Priority));
				CameFrom.Add(Next, Current);
			}
		}
	}

	//End if we never reached the end tile
	if (bEndReached == false)
		return;

	//Backtrace the path
	TTile* n = End;
	while (n != Start)
	{
		Path->Add(n);
		TTile** a = CameFrom.Find(n);
		if (a == nullptr)
			return;
		else
			n = *a;
	}

	Path->Add(Start);

	//Reverse the path
	Algo::Reverse(*Path);
}