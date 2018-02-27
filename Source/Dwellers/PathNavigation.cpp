#include "PathNavigation.h"

#include "Tiles.h"
#include "GameEncapsulator.h"

FPathObject::FPathObject(TTile* cur)
{
	current = cur;
}

bool FPathObject::CouldBeNeighbor(TTile* othr)
{
	return FMath::Abs(othr->location->x - current->location->x) + FMath::Abs(othr->location->y - current->location->y) == 1;
}


void FPathTree::CombinePaths(FPathTree* tree)
{
	for (TPair<TTile*, FPathObject*> elm : tree->ContainingTiles)
	{
		AddTile(elm.Key);
	}
}

bool FPathTree::ContainsTile(TTile* tile)
{
	return ContainingTiles.Contains(tile);
}

FPathObject* FPathTree::GetPathObject(TTile* tile)
{
	return *ContainingTiles.Find(tile);
}

bool FPathTree::CouldBePartOfPath(TTile* tile)
{
	for (TPair<TTile*, FPathObject*> elm : ContainingTiles)
	{
		if (elm.Value->CouldBeNeighbor(tile))
			return true;
	}

	return false;
}

void FPathTree::AddTile(TTile* tile)
{
	FPathObject* obj = new FPathObject(tile);

	ContainingTiles.Add(tile, obj);

	TTile* til = GameEncapsulator::GetGame()->map->GetTile(tile->location->x + 1, tile->location->y);
	if (ContainsTile(til))
	{
		FPathObject* nxt = *ContainingTiles.Find(til);
		obj->neighbors[2] = nxt;
		nxt->neighbors[0] = obj;
	}

	til = GameEncapsulator::GetGame()->map->GetTile(tile->location->x - 1, tile->location->y);
	if (ContainsTile(til))
	{
		FPathObject* nxt = *ContainingTiles.Find(til);
		obj->neighbors[0] = nxt;
		nxt->neighbors[2] = obj;
	}

	til = GameEncapsulator::GetGame()->map->GetTile(tile->location->x, tile->location->y + 1);
	if (ContainsTile(til))
	{
		FPathObject* nxt = *ContainingTiles.Find(til);
		obj->neighbors[1] = nxt;
		nxt->neighbors[3] = obj;
	}

	til = GameEncapsulator::GetGame()->map->GetTile(tile->location->x, tile->location->y - 1);
	if (ContainsTile(til))
	{
		FPathObject* nxt = *ContainingTiles.Find(til);
		obj->neighbors[3] = nxt;
		nxt->neighbors[1] = obj;
	}
}


void FPathNavigationHandler::AddTile(TTile* tile)
{
	TArray<FPathTree*> trees;
	for (int x = 0; x < Paths.Num(); ++x)
	{
		if (Paths[x]->CouldBePartOfPath(tile))
			trees.Add(Paths[x]);
	}

	if (trees.Num() == 0)
	{
		FPathTree* tree = new FPathTree();
		tree->AddTile(tile);
		Paths.Add(tree);
	}
	else if (trees.Num() == 1)
	{
		trees[0]->AddTile(tile);
	}
	else if (trees.Num() > 1)
	{
		while (trees.Num() >= 2)
		{
			FPathTree* pt = trees.Pop();
			trees[0]->CombinePaths(pt);
			Paths.Remove(pt);
			delete pt;
		}

		Paths[0]->AddTile(tile);
	}
}

bool FPathNavigationHandler::TilesPartOfSamePath(TTile* t1, TTile* t2)
{
	FPathTree* tree1 = nullptr;
	FPathTree* tree2 = nullptr;
	for (int x = 0; x < Paths.Num(); ++x)
	{
		if (tree1 == nullptr && Paths[x]->ContainsTile(t1))
		{
			if (tree2 != nullptr)
				return Paths[x] == tree2;

			tree1 = Paths[x];
		}

		if (tree2 == nullptr && Paths[x]->ContainsTile(t2))
		{
			if (tree1 != nullptr)
				return Paths[x] == tree1;

			tree2 = Paths[x];
		}
	}

	return nullptr;
}

FPathTree* FPathNavigationHandler::GetAssociatedTreeForTile(TTile* ti)
{
	for (int x = 0; x < Paths.Num(); ++x)
	{
		if (Paths[x]->ContainsTile(ti))
			return Paths[x];
	}

	return nullptr;
}

TArray<TTile*> FPathNavigationHandler::GetPath(TTile* t1, TTile* t2)
{
	TArray<TTile*> path;

	if (!TilesPartOfSamePath(t1, t2))
		return path;

	FPathTree* tree = GetAssociatedTreeForTile(t1);

	FPathObject* st = tree->GetPathObject(t1);
	FPathObject* end = tree->GetPathObject(t2);

	TArray<FPathObject*> frontier;
	frontier.Add(st);

	TMap<FPathObject*, FPathObject*> camefrom;
	camefrom.Add(st, st);
	TMap<FPathObject*, int> costsofar;
	costsofar.Add(st, 0);

	while (frontier.Num() > 0)
	{
		FPathObject* cur = frontier.Pop();

		for (FPathObject* nxt : cur->neighbors)
		{
			if (nxt == nullptr)
				continue;

			float newcost = costsofar[cur] + 1;
			if (!costsofar.Contains(nxt) || newcost < costsofar[nxt])
			{
				costsofar.Add(nxt, newcost);
				frontier.Add(nxt);
				camefrom.Add(nxt, cur);
			}
		}
	}

	FPathObject* n = end;
	while (n != st)
	{
		path.Add(n->current);
		FPathObject** a = camefrom.Find(n);
		if (a == nullptr)
			return path;
		else
			n = *a;
	}

	path.Add(st->current);

	Algo::Reverse(path);

	return path;
}
