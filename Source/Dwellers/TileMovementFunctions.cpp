// Fill out your copyright notice in the Description page of Project Settings.

#include "TileMovementFunctions.h"
#include "Algo/Reverse.h"
#include "TileObject.h"
#include "Async/TaskGraphInterfaces.h"

float D = .0001f;

TTile* GetTileAtLocation(FVector loc, int cellsize, TTile*** tiles, int mapsize)
{
	int x = ((int)loc.X) / cellsize;
	int y = ((int)loc.Y) / cellsize;

	if (x < 0 || x >= mapsize || y < 0 || y >= mapsize)
		return nullptr;

	return tiles[x][y];
}

TArray<TTile*> GetAdjacentTiles(TTile*** tiles, TTile* tile, int mapsize)
{
	TArray<TTile*> returner;

	int x = tile->location->x;
	int y = tile->location->y;

	if(x + 1 < mapsize)
		returner.Add(tiles[x + 1][y]);
	if (x - 1 >= 0)
		returner.Add(tiles[x - 1][y]);
	if (y + 1 < mapsize)
		returner.Add(tiles[x][y + 1]);
	if (y - 1 >= 0)
		returner.Add(tiles[x][y - 1]);

	return returner;
}

bool AreVectorsInAStraightLine(FVector2D* v1, int v1height, FVector2D* v2, int v2height)
{
	if ((v1->X == v2->X || v1->Y == v2->Y) && v1height == v2height)
		return true;
	else
		return false;
}

float GetHeuristic(TTile* current, TTile* target)
{
	float a = FMath::Abs(current->location->x - target->location->x) / GameEncapsulator::GetGame()->map->cellsize;
	float b = FMath::Abs(current->location->y - target->location->y) / GameEncapsulator::GetGame()->map->cellsize;

	return D * (a + b);
}

TTile* GetTile(int x, int y, TTile*** map, int mapsize)
{
	if (x >= mapsize || y >= mapsize || x < 0 || y < 0)
		return nullptr;
	else
		return map[x][y];
}

TArray<TTile*> GetNeighbors(TTile* current, TTile*** map, int mapsize, bool cardinals)
{
	int x = current->location->x;
	int y = current->location->y;

	TArray<TTile*> returner;

	TTile* ti;

	ti = GetTile(x + 1, y, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x, y + 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x - 1, y, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x, y - 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	if (cardinals)
		return returner;

	ti = GetTile(x + 1, y + 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x + 1, y - 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x - 1, y + 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	ti = GetTile(x - 1, y - 1, map, mapsize);
	if (ti != nullptr)
		returner.Add(ti);

	return returner;
}

TArray<FVector> UTileMovementFunctions::GetTilePathFromPoints(FVector start, FVector end, bool cardinals)
{
	TArray<FVector> locations;

	WorldMap* map = GameEncapsulator::GetGame()->map;

	TTile* st = GetTileAtLocation(start, map->cellsize, map->tiles, map->mapsize);
	TTile* ed = GetTileAtLocation(end, map->cellsize, map->tiles, map->mapsize);

	if (st == nullptr || ed == nullptr)
		return locations;

	if (st->chunknum != ed->chunknum)
		return locations;

	TArray<TTile*> path;

	double time = FPlatformTime::Seconds();

	UTileMovementFunctions::GetTilePath(&path, map->tiles, map->mapsize, st, ed, cardinals);

	//UE_LOG(LogTemp, Log, TEXT("Path Gen Time: %f"), FPlatformTime::Seconds() - time);

	int tilecount = path.Num();
	int next = 0;
	for (int x = 0; x < tilecount; x = next)
	{
		locations.Add(FVector(path[x]->location->x * map->cellsize, path[x]->location->y * map->cellsize, path[x]->height * (map->cellsize * map->cliffheight)));
		next = x + 1;
	}

	return locations;
}

TArray<TTile*> UTileMovementFunctions::GetTilePath(TTile* start, TTile* end, bool cardinals)
{
	WorldMap* map = GameEncapsulator::GetGame()->map;

	TArray<TTile*> path;

	if (start == nullptr || end == nullptr)
		return path;

	if (start->chunknum != end->chunknum)
		return path;

	//double time = FPlatformTime::Seconds();

	UTileMovementFunctions::GetTilePath(&path, map->tiles, map->mapsize, start, end, cardinals);

	//UE_LOG(LogTemp, Log, TEXT("Path Gen Time: %f"), FPlatformTime::Seconds() - time);

	return path;
}

TArray<TTile*> UTileMovementFunctions::GetTilePath_NoWeight(TTile* start, TTile* end, bool cardinals)
{
	WorldMap* map = GameEncapsulator::GetGame()->map;

	TArray<TTile*> path;

	if (start == nullptr || end == nullptr)
		return path;

	if (start->chunknum != end->chunknum)
		return path;

	//double time = FPlatformTime::Seconds();

	UTileMovementFunctions::GetTilePath_NoWeight(&path, map->tiles, map->mapsize, start, end, cardinals);

	//UE_LOG(LogTemp, Log, TEXT("Path Gen Time: %f"), FPlatformTime::Seconds() - time);

	return path;
}

void UTileMovementFunctions::AsyncMoveActorTo(AEntity* target, FVector end)
{
	FAutoDeleteAsyncTask<FFindPathTask>* task = new FAutoDeleteAsyncTask<FFindPathTask>(target, end);
	task->StartBackgroundTask();
}

void UTileMovementFunctions::GetTilePath(TArray<TTile*>* path, TTile*** tiles, int mapsize, TTile* start, TTile* end, bool cardinals)
{
	PriorityQueue frontier;
	frontier.AddNode(new PriorityNode(start, 0));

	TMap<TTile*, TTile*> camefrom;
	camefrom.Add(start, start);
	TMap<TTile*, float> costsofar;
	costsofar.Add(start, 0);

	bool endreached = false;

	while (!frontier.IsEmpty())
	{
		TTile* current = frontier.Pluck();

		if (current == end)
		{
			endreached = true;
			break;
		}

		TArray<TTile*> neighbors = GetNeighbors(current, tiles, mapsize, cardinals);
		for (TTile* next : neighbors)
		{
			if (next == nullptr || !next->IsTraversable())
				continue;

			float newcost = costsofar[current] + next->GetCost();
			if (costsofar.Find(next) == nullptr || newcost < costsofar[next])
			{
				costsofar.Add(next, newcost);
				float priority = newcost + GetHeuristic(next, end);
				frontier.AddNode(new PriorityNode(next, priority));
				camefrom.Add(next, current);
			}
		}
	}

	if (endreached == false)
		return;

	TTile* n = end;
	while (n != start)
	{
		path->Add(n);
		TTile** a = camefrom.Find(n);
		if (a == nullptr)
			return;
		else
			n = *a;
	}

	Algo::Reverse(*path);
}

void UTileMovementFunctions::GetTilePath_NoWeight(TArray<TTile*>* path, TTile*** tiles, int mapsize, TTile* start, TTile* end, bool cardinals)
{
	PriorityQueue frontier;
	frontier.AddNode(new PriorityNode(start, 0));

	TMap<TTile*, TTile*> camefrom;
	camefrom.Add(start, start);
	TMap<TTile*, float> costsofar;
	costsofar.Add(start, 0);

	bool endreached = false;

	while (!frontier.IsEmpty())
	{
		TTile* current = frontier.Pluck();

		if (current == end)
		{
			endreached = true;
			break;
		}

		TArray<TTile*> neighbors = GetNeighbors(current, tiles, mapsize, cardinals);
		for (TTile* next : neighbors)
		{
			if (next == nullptr || !next->IsTraversable())
				continue;

			float newcost = costsofar[current] + 1;
			if (costsofar.Find(next) == nullptr || newcost < costsofar[next])
			{
				costsofar.Add(next, newcost);
				float priority = newcost + GetHeuristic(next, end);
				frontier.AddNode(new PriorityNode(next, priority));
				camefrom.Add(next, current);
			}
		}
	}

	if (endreached == false)
		return;

	TTile* n = end;
	while (n != start)
	{
		path->Add(n);
		TTile** a = camefrom.Find(n);
		if (a == nullptr)
			return;
		else
			n = *a;
	}

	Algo::Reverse(*path);
}


/*
	TArray<TTile*> returner;

	if (start == nullptr || end == nullptr)
		return returner;

	int minDistance = 9999999;

	TArray<TTile*> visitedNodes;
	TMap<TTile*, TTile*> parentNodes;

	TArray<TTile*> queue;
	queue.Add(start);

	while (queue.Num() > 0)
	{
		TTile* focus = queue[0];
		queue.RemoveAt(0);

		//building path
		if (focus == end)
		{
			TArray<TTile*> pth;
			TTile* node = end;
			while (node != nullptr && node != start)
			{
				pth.Add(node);
				TTile** adsf = parentNodes.Find(node);
				node = adsf == nullptr ? nullptr : *adsf;
			}
			pth.Add(start);

			if (pth.Num() < minDistance)
			{
				returner.Append(pth);
				minDistance = pth.Num();
			}
		}

		if (!visitedNodes.Contains(focus))
			visitedNodes.Add(focus);
		else
			continue;

		TArray<TTile*> adjacents = GetAdjacentTiles(tiles, focus, mapsize);

		for (int x = 0; x < adjacents.Num(); x++)
		{
			if (!visitedNodes.Contains(adjacents[x]) && !queue.Contains(adjacents[x]) && adjacents[x]->IsTraversable())
			{
				queue.Add(adjacents[x]);
				parentNodes.Add(adjacents[x], focus);
			}
		}
	}

	if (returner.Num() <= 0)
		returner.Add(start);

	return returner;
*/