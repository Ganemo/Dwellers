// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEncapsulator.h"
#include "FunctionLibrary.h"
#include "WorldMap.h"
#include "Tiles.h"
#include "WorldChunk.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

class TTile;
class AWorldChunk;


/// <summary>
/// An object that a tile can have on it
/// Affects tile traversal
/// </summary>
class DWELLERS_API FTileObject
{
public:
	FTileObject::FTileObject() {}

	virtual FTileObject::~FTileObject() {}

	virtual FString FTileObject::GetName() { return "Default"; }
	virtual FString FTileObject::GetDescription() { return "Default Description"; }
	virtual float FTileObject::GetTraversalCost() { return 1; }

	/// <summary>
	/// Affect the tile that this object is on
	/// </summary>
	/// <param Name="Node">The tile</param>
	/// <param Name="Node">The chunk (used for instances)</param>
	virtual void FTileObject::AffectTile(TTile* OwningTile, AWorldChunk* Chunk);

	/// <summary>
	/// Destroy this tile object and remove it's affects
	/// </summary>
	/// <param Name="Node">The tile</param>
	/// <param Name="Node">The chunk (used for instances)</param>
	virtual void FTileObject::Destroy(TTile* OwningTile, AWorldChunk* Chunk) {};
};

class DWELLERS_API FTileObject_Grass : public FTileObject
{
	TArray<int> Instances;
public:
	FTileObject_Grass::~FTileObject_Grass() override {};

	FString FTileObject_Grass::GetName() override { return "Grass"; }
	FString FTileObject_Grass::GetDescription() override { return "Simple Grass"; }
	float FTileObject_Grass::GetTraversalCost() override { return 1; }

	void FTileObject_Grass::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
	void FTileObject_Grass::Destroy(TTile* OwningTile, AWorldChunk* Chunk) override;
};

class DWELLERS_API FTileObject_CatTail : public FTileObject
{
	int instance = 0;
public:
	FTileObject_CatTail::~FTileObject_CatTail() override {};

	FString FTileObject_CatTail::GetName() override { return "CatTail"; }
	FString FTileObject_CatTail::GetDescription() override { return "Simple Grass"; }
	float FTileObject_CatTail::GetTraversalCost() override { return 1; }

	void FTileObject_CatTail::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
};

class DWELLERS_API FTileObject_Tree : public FTileObject
{
	TArray<int> Instances;
public:
	FTileObject_Tree::~FTileObject_Tree() override {};

	virtual FString FTileObject_Tree::GetName() override { return "Oak Tree"; }
	virtual FString FTileObject_Tree::GetDescription() override { return "Default Description"; }
	float FTileObject_Tree::GetTraversalCost() override { return -1; }

	void FTileObject_Tree::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
};

class DWELLERS_API FTileObject_Stone : public FTileObject
{
	TArray<int> Instances;
public:
	FString FTileObject_Stone::GetName() override { return "Rock"; }
	FString FTileObject_Stone::GetDescription() override { return "Default Description"; }
	float FTileObject_Stone::GetTraversalCost() override { return -1; }

	void FTileObject_Stone::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
};
class DWELLERS_API FTileObject_AnimalHotspot : public FTileObject
{
	class AAnimalHotSpot* hotspot;
public:
	FString FTileObject_AnimalHotspot::GetName() override { return "Animal Hot Spot"; }
	FString FTileObject_AnimalHotspot::GetDescription() override { return "Default Description"; }
	float FTileObject_AnimalHotspot::GetTraversalCost() override { return 1; }

	void FTileObject_AnimalHotspot::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
};

class DWELLERS_API FTileObject_Road : public FTileObject
{
public:
	FString FTileObject_Road::GetName() override { return "Road"; }
	FString FTileObject_Road::GetDescription() override { return "Default Description"; }
	float FTileObject_Road::GetTraversalCost() override { return .5; }

	void FTileObject_Road::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) override;
	void FTileObject_Road::Destroy(TTile* OwningTile, AWorldChunk* Chunk) override;
};