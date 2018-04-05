#pragma once

class TTile;

/// <summary>
/// Object that contains a tile and its cardinal neighbors
/// </summary>
class DWELLERS_API FPathObject
{
public:
	/// <summary>
	/// The tile that this Path object represents
	/// </summary>
	TTile* Current;

	/// <summary>
	/// The four possible neighbors
	/// </summary>
	FPathObject* Neighbors[4] = { nullptr, nullptr, nullptr, nullptr };

	FPathObject(TTile* CurrentTile);

	/// <summary>
	/// Determine if a tile could be a neighbor of this tile
	/// </summary>
	/// <param Name="Other">Other tile</param>
	bool CouldBeNeighbor(TTile* Other);
};

/// <summary>
/// The full Path in a tree format
/// </summary>
class DWELLERS_API FPathTree
{
private:
	/// <summary>
	/// Tiles within the Path
	/// </summary>
	TMap<TTile*, FPathObject*> ContainingTiles;

public:
	/// <summary>
	/// Add a Path tree to this Path tree
	/// </summary>
	/// <param Name="Tree">Other tree</param>
	void CombinePaths(FPathTree* Tree);

	/// <summary>
	/// Check if this tile is in this Path
	/// </summary>
	/// <param Name="Tile">The tile we are checking for</param>
	bool ContainsTile(TTile* Tile);

	/// <summary>
	/// Get the Path object for a tile
	/// </summary>
	/// <param Name="Tile">The tile</param>
	FPathObject* GetPathObject(TTile* Tile);

	/// <summary>
	/// Check if a tile could be part of this Path
	/// </summary>
	/// <param Name="Tile">The tile</param>
	bool CouldBePartOfPath(TTile* Tile);

	/// <summary>
	/// Add a tile to this Path
	/// </summary>
	/// <param Name="Tile">The tile</param>
	void AddTile(TTile* Tile);
};

/// <summary>
/// A handler used to get paths for navigation on roads
/// </summary>
class DWELLERS_API FPathNavigationHandler
{
public:
	/// <summary>
	/// All the paths currently
	/// </summary>
	TArray<FPathTree*> Paths;

	/// <summary>
	/// Add a tile to the Current Path system
	/// </summary>
	/// <param Name="Tile">The tile</param>
	void FPathNavigationHandler::AddTile(TTile* Tile);

	/// <summary>
	/// Check if two tiles are part of the same Path.
	/// Used for actors who can't leave a Path
	/// </summary>
	/// <param Name="T1">The first tile</param>
	/// <param Name="T2">The second tile</param>
	bool FPathNavigationHandler::TilesPartOfSamePath(TTile* T1, TTile* T2);

	/// <summary>
	/// Get a Path tree that contains a certain tile
	/// </summary>
	/// <param Name="Tile">The tile</param>
	FPathTree* FPathNavigationHandler::GetAssociatedTreeForTile(TTile* Tile);

	/// <summary>
	/// Get the Path to get from one tile to another
	/// </summary>
	/// <param Name="T1">The first tile</param>
	/// <param Name="T2">The second tile</param>
	TArray<TTile*> FPathNavigationHandler::GetPath(TTile* T1, TTile* T2);
};

