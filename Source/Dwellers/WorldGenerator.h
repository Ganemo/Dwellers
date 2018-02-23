// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastNoise.h"
#include "RuntimeMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "WorldChunk.h"
#include "GameEncapsulator.h"
#include "WorldMap.h"
#include "TileObject.h"
#include "Tiles.h"
#include "WorldGenerator.generated.h"



enum ETilePointPos
{
	BotLeft,
	Left,
	TopLeft,
	Top,
	TopRight,
	Right,
	BotRight,
	Bot,
	Center
};

class DWELLERS_API FRiverContainer
{

public:
	int xpos;
	int ypos;

	int radius;

	FRiverContainer::FRiverContainer(int x, int y, int rad)
	{
		xpos = x;
		ypos = y;
		radius = rad;
	}
};

struct TMapPoint
{
public:
	TMapPoint() {};
	TMapPoint(float hght, FColor color, TTile* ti)
	{
		height = hght;
		vertexcolor = color;
		tile = ti;
	};

	float height = 0;
	FColor vertexcolor;

	TTile* tile = nullptr;
};

 UCLASS()
class DWELLERS_API AWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	AWorldGenerator();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int HalfWidth = 400;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ChunkSize = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CellSize = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float cliffheight = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaterHeight = .5f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GrassValueShove = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool useseed = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int seed = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float heightscale = .005f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float typescale = .07f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float treescale = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float waterscale = .005f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float wateroctaves = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float mindraw = 1000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxdraw = 5000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float grassthrsh = .5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float treethrsh = .5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float animalthrsh = .95f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float waterthrsh = .95f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* GrassMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* GrassMaterial;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GrassMeshScale = 1.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* TreeMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UMaterialInterface*> TreeMaterials;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* WaterMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* WaterMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UBlueprint*> TreeBPs;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform & Transform) override;

public:
	void GenerateWorld();

	void WaterPass();

	void GenerateRiver(int xpos, int ypos, FVector2D direction, bool justturned, int rad, bool justchangedrad, TArray<FRiverContainer*>* river);
	void CreateWaterWithBrush(int xpos, int ypos, int rads);
	void MakeTileWater(int xpos, int ypos);

	void CliffPass();

	void Generate8PointTiles();

	void SpawnWorld8PointTile();

	int IsTileHigher(TTile* current, TTile* next);

	TMapPoint* MakeNewPoint(TTile* currenttile, TMapPoint currentpoint, ETilePointPos pointpos);

private:
	WorldMap* worldmap;
	TMapPoint** points;

	TTile* GetTile(int x, int y);
	void TraverseTileHeightChunk(TMap<TTile*, TTile*>* map, int chunknum, int x, int y, int height, ETileType type);
	void TraverseWaterChunk(TMap<TTile*, TTile*>* map, int chunknum, int x, int y, int height, ETileType type);
	
};
