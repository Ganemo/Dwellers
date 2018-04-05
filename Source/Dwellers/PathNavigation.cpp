#include "PathNavigation.h"

#include "Tiles.h"
#include "GameEncapsulator.h"

FPathObject::FPathObject(TTile* CurrentTile)
{
	Current = CurrentTile;
}

bool FPathObject::CouldBeNeighbor(TTile* Other)
{
	//If the difference between the x and y locations of the tiles are not 1,
	//it cannot be a neighbor as the y location or the x location exclusively must be 1.
	return FMath::Abs(Other->location->x - Current->location->x) + FMath::Abs(Other->location->y - Current->location->y) == 1;
}


void FPathTree::CombinePaths(FPathTree* Tree)
{
	//Add all tiles from other tree
	for (TPair<TTile*, FPathObject*> Element : Tree->ContainingTiles)
	{
		AddTile(Element.Key);
	}
}

bool FPathTree::ContainsTile(TTile* Tile)
{
	//Check if tile in this Path
	return ContainingTiles.Contains(Tile);
}

FPathObject* FPathTree::GetPathObject(TTile* Tile)
{
	//Get the Path ovbject for the tile
	FPathObject** Obj = ContainingTiles.Find(Tile);
	if (Obj != nullptr)
		return *Obj;
	else
		return nullptr;
}

bool FPathTree::CouldBePartOfPath(TTile* Tile)
{
	//check if the tile could be a neighbor to any of the tiles in this Path
	for (TPair<TTile*, FPathObject*> Elm : ContainingTiles)
	{
		if (Elm.Value->CouldBeNeighbor(Tile))
			return true;
	}

	return false;
}

void FPathTree::AddTile(TTile* Tile)
{
	//Create a Path object for the tile
	FPathObject* Obj = new FPathObject(Tile);

	//Add the tile to the storage map
	ContainingTiles.Add(Tile, Obj);

	//Check the tile to the right of this tile
	TTile* Til = GameEncapsulator::GetGame()->Map->GetTile(Tile->location->x + 1, Tile->location->y);
	if (ContainsTile(Til))
	{
		FPathObject* nxt = *ContainingTiles.Find(Til);
		Obj->Neighbors[2] = nxt;
		nxt->Neighbors[0] = Obj;
	}

	//Check the tile to the left of this tile
	Til = GameEncapsulator::GetGame()->Map->GetTile(Tile->location->x - 1, Tile->location->y);
	if (ContainsTile(Til))
	{
		FPathObject* nxt = *ContainingTiles.Find(Til);
		Obj->Neighbors[0] = nxt;
		nxt->Neighbors[2] = Obj;
	}

	//Check the tile to the above this tile
	Til = GameEncapsulator::GetGame()->Map->GetTile(Tile->location->x, Tile->location->y + 1);
	if (ContainsTile(Til))
	{
		FPathObject* nxt = *ContainingTiles.Find(Til);
		Obj->Neighbors[1] = nxt;
		nxt->Neighbors[3] = Obj;
	}

	//Check the tile to the below this tile
	Til = GameEncapsulator::GetGame()->Map->GetTile(Tile->location->x, Tile->location->y - 1);
	if (ContainsTile(Til))
	{
		FPathObject* nxt = *ContainingTiles.Find(Til);
		Obj->Neighbors[3] = nxt;
		nxt->Neighbors[1] = Obj;
	}
}


void FPathNavigationHandler::AddTile(TTile* Tile)
{
	TArray<FPathTree*> Trees;
	
	//Get all the paths that could contain this tile
	for (int x = 0; x < Paths.Num(); ++x)
	{
		if (Paths[x]->CouldBePartOfPath(Tile))
			Trees.Add(Paths[x]);
	}

	//Create a new tree if there are no existing paths that could contain the tile
	if (Trees.Num() == 0)
	{
		FPathTree* Tree = new FPathTree();
		Tree->AddTile(Tile);
		Paths.Add(Tree);
	}
	//Add the tile to a tree if there is only one valid tree
	else if (Trees.Num() == 1)
	{
		Trees[0]->AddTile(Tile);
	}
	//Combine trees until there is only one left
	else if (Trees.Num() > 1)
	{
		while (Trees.Num() >= 2)
		{
			FPathTree* PT = Trees.Pop();
			Trees[0]->CombinePaths(PT);
			Paths.Remove(PT);
			delete PT;
		}

		//add the tile ot the remaining tree
		Trees[0]->AddTile(Tile);
	}
}

bool FPathNavigationHandler::TilesPartOfSamePath(TTile* T1, TTile* T2)
{
	//The two trees
	FPathTree* Tree1 = nullptr;
	FPathTree* Tree2 = nullptr;

	for (int x = 0; x < Paths.Num(); ++x)
	{
		//Set the first tree containing the first tile
		if (Tree1 == nullptr && Paths[x]->ContainsTile(T1))
		{
			//If the other tree is already found, check if they are the same tree
			if (Tree2 != nullptr)
				return Paths[x] == Tree2;

			Tree1 = Paths[x];
		}

		//Set the second tree containing the second tile
		if (Tree2 == nullptr && Paths[x]->ContainsTile(T2))
		{
			//If the other tree is already found, check if they are the same tree
			if (Tree1 != nullptr)
				return Paths[x] == Tree1;

			Tree2 = Paths[x];
		}
	}

	return nullptr;
}

FPathTree* FPathNavigationHandler::GetAssociatedTreeForTile(TTile* Tile)
{
	//Find the tree that contains the tile
	for (int x = 0; x < Paths.Num(); ++x)
	{
		if (Paths[x]->ContainsTile(Tile))
			return Paths[x];
	}

	return nullptr;
}

TArray<TTile*> FPathNavigationHandler::GetPath(TTile* T1, TTile* T2)
{
	//The Path to return
	TArray<TTile*> Path;

	//If the tiles aren't part of the same Path, the return an empty Path
	if (!TilesPartOfSamePath(T1, T2))
		return Path;

	//Get the tree that both tiles are in
	FPathTree* Tree = GetAssociatedTreeForTile(T1);

	//Get the starting and ending Path objects
	FPathObject* StartTile = Tree->GetPathObject(T1);
	FPathObject* EndTile = Tree->GetPathObject(T2);

	///////Perform A* pathfinding algorithm///////////
	 
	//Create the fronteir and add the starting tile
	TArray<FPathObject*> Frontier;
	Frontier.Add(StartTile);

	//The maps used for backtracing full Path
	TMap<FPathObject*, FPathObject*> CameFrom;
	CameFrom.Add(StartTile, StartTile);

	//Store the lowest cost to get to this tile
	TMap<FPathObject*, int> CostsSoFar;
	CostsSoFar.Add(StartTile, 0);

	while (Frontier.Num() > 0)
	{
		//Take the top node in the frontier
		FPathObject* Current = Frontier.Pop();

		//Iterate over neighbors
		for (FPathObject* Next : Current->Neighbors)
		{
			if (Next == nullptr)
				continue;

			//Determine the new cost to get to this node
			float NewCost = CostsSoFar[Current] + 1;

			//Check if the node has been seen or if it has been reached by a lower cost Path
			if (!CostsSoFar.Contains(Next) || NewCost < CostsSoFar[Next])
			{
				//Add the next to the custs so far
				CostsSoFar.Add(Next, NewCost);
				//Add the next node to the frontier
				Frontier.Add(Next);
				//Store that the next node was reached from the Current node
				CameFrom.Add(Next, Current);
			}
		}
	}

	//Retrace the shortest Path to the destination
	FPathObject* n = EndTile;
	while (n != StartTile)
	{
		Path.Add(n->Current);
		FPathObject** a = CameFrom.Find(n);
		if (a == nullptr)
			return Path;
		else
			n = *a;
	}

	//Add the starting tile to the Path
	Path.Add(StartTile->Current);

	//Reverse the Path
	Algo::Reverse(Path);

	return Path;
}
