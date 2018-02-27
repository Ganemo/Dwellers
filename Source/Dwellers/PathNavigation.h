#pragma once

class TTile;

class DWELLERS_API FPathObject
{
public:
	TTile * current;

	//left, up, right, down
	FPathObject* neighbors[4] = { nullptr, nullptr, nullptr, nullptr };


	FPathObject::FPathObject(TTile* cur);

	bool FPathObject::CouldBeNeighbor(TTile* othr);
};

class DWELLERS_API FPathTree
{
private:
	TMap<TTile*, FPathObject*> ContainingTiles;

public:

	void FPathTree::CombinePaths(FPathTree* tree);

	bool FPathTree::ContainsTile(TTile* tile);

	FPathObject* FPathTree::GetPathObject(TTile* tile);

	bool FPathTree::CouldBePartOfPath(TTile* tile);

	void FPathTree::AddTile(TTile* tile);

};

class DWELLERS_API FPathNavigationHandler
{
public:
	TArray<FPathTree*> Paths;

	void FPathNavigationHandler::AddTile(TTile* tile);

	bool FPathNavigationHandler::TilesPartOfSamePath(TTile* t1, TTile* t2);

	FPathTree* FPathNavigationHandler::GetAssociatedTreeForTile(TTile* ti);

	TArray<TTile*> FPathNavigationHandler::GetPath(TTile* t1, TTile* t2);
};

