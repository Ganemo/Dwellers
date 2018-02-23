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


/**
 * 
 */
class DWELLERS_API FTileObject
{
public:
	FTileObject::FTileObject() {}

	virtual FTileObject::~FTileObject() {}

	virtual FString FTileObject::GetName() { return "Default"; }
	virtual FString FTileObject::GetDescription() { return "Default Description"; }
	virtual float FTileObject::GetTraversalCost() { return 1; }

	virtual void FTileObject::AffectTile(TTile* owningtile, AWorldChunk* chunk);
	virtual void FTileObject::Destroy(TTile* owningtile, AWorldChunk* chunk) {};
};

class DWELLERS_API FTileObject_Grass : public FTileObject
{
	TArray<int> instances;
public:
	FTileObject_Grass::~FTileObject_Grass() override {};

	FString FTileObject_Grass::GetName() override { return "Grass"; }
	FString FTileObject_Grass::GetDescription() override { return "Simple Grass"; }
	float FTileObject_Grass::GetTraversalCost() override { return 1; }

	void FTileObject_Grass::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
	void FTileObject_Grass::Destroy(TTile* owningtile, AWorldChunk* chunk) override;
};

class DWELLERS_API FTileObject_CatTail : public FTileObject
{
	int instance = 0;
public:
	FTileObject_CatTail::~FTileObject_CatTail() override {};

	FString FTileObject_CatTail::GetName() override { return "CatTail"; }
	FString FTileObject_CatTail::GetDescription() override { return "Simple Grass"; }
	float FTileObject_CatTail::GetTraversalCost() override { return 1; }

	void FTileObject_CatTail::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
};

class DWELLERS_API FTileObject_Tree : public FTileObject
{
	TArray<int> instances;
public:
	FTileObject_Tree::~FTileObject_Tree() override {};

	virtual FString FTileObject_Tree::GetName() override { return "Oak Tree"; }
	virtual FString FTileObject_Tree::GetDescription() override { return "Default Description"; }
	float FTileObject_Tree::GetTraversalCost() override { return -1; }

	void FTileObject_Tree::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
};

class DWELLERS_API FTileObject_Stone : public FTileObject
{
	TArray<int> instances;
public:
	FString FTileObject_Stone::GetName() override { return "Rock"; }
	FString FTileObject_Stone::GetDescription() override { return "Default Description"; }
	float FTileObject_Stone::GetTraversalCost() override { return -1; }

	void FTileObject_Stone::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
};
class DWELLERS_API FTileObject_AnimalHotspot : public FTileObject
{
	class AAnimalHotSpot* hotspot;
public:
	FString FTileObject_AnimalHotspot::GetName() override { return "Animal Hot Spot"; }
	FString FTileObject_AnimalHotspot::GetDescription() override { return "Default Description"; }
	float FTileObject_AnimalHotspot::GetTraversalCost() override { return 1; }

	void FTileObject_AnimalHotspot::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
};

class DWELLERS_API FTileObject_Road : public FTileObject
{
public:
	FString FTileObject_Road::GetName() override { return "Road"; }
	FString FTileObject_Road::GetDescription() override { return "Default Description"; }
	float FTileObject_Road::GetTraversalCost() override { return .5; }

	void FTileObject_Road::AffectTile(TTile* owningtile, AWorldChunk* chunk) override;
	void FTileObject_Road::Destroy(TTile* owningtile, AWorldChunk* chunk) override;
};