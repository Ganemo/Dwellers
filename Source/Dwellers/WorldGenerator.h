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


/// <summary>
/// The location of a vertex on a 9 point tile
/// </summary>
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

/// <summary>
/// Container used to paint a river of radius on the map
/// </summary>
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

/// <summary>
/// A structure containing all vertex information for tiles
/// </summary>
struct TMapPoint
{
public:
	TMapPoint() {};
	TMapPoint(float hght, FColor color, TTile* ti)
	{
		Height = hght;
		vertexcolor = color;
		tile = ti;
	};

	float Height = 0;
	FColor vertexcolor;

	TTile* tile = nullptr;
};

 /// <summary>
 /// Generates a world with given properties, with 9 point tiles
 /// </summary>
 UCLASS()
class DWELLERS_API AWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	AWorldGenerator();

	/// <summary>
	/// Half the width of the map to spawn
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int HalfWidth = 400;
	
	/// <summary>
	/// The Height and width of each chunk
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ChunkSize = 50;

	/// <summary>
	/// The world size of each tile
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CellSize = 100;

	/// <summary>
	/// The scalar for Height of cliffs
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CliffHeight = 5;

	/// <summary>
	/// The scalar for how low the water is below the lowest tile
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaterHeight = .5f;
	
	/// <summary>
	/// Value to shove grass values to more extreme gradient values
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GrassValueShove = 2;

	/// <summary>
	/// Whether to use a seed on the next generation
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseSeed = false;

	/// <summary>
	/// The seed to use when generating a seeded map
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Seed = -1;

	/// <summary>
	/// The scale of the Height noise
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HeightScale = .005f;

	/// <summary>
	/// The scale of the TileType noise
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TypeScale = .07f;

	/// <summary>
	/// The scale of the tree noise
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TreeScale = 1.0f;

	/// <summary>
	/// Minimum draw distance for instances
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinDraw = 1000;

	/// <summary>
	/// Maximum draw distance for the instances
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxDraw = 5000;

	/// <summary>
	/// The threshold to create a grass tile
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GrassThrsh = .5f;

	/// <summary>
	/// The threshold to spawn a tree
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TreeThrsh = .5f;

	/// <summary>
	/// The threshold to spawn an animal hotspot
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AnimalThrsh = .95f;

	/// <summary>
	/// The threshold to create a water tile
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaterThrsh = .95f;

	/// <summary>
	/// The material for the landscape instances
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	/// <summary>
	/// The mesh used for water tile instances
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* WaterMesh;

	/// <summary>
	/// The material used for water tile instances
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* WaterMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform & Transform) override;

public:
	/// <summary>
	/// Generates a new world map
	/// </summary>
	void GenerateWorld();

	/// <summary>
	/// Creates water bodies
	/// </summary>
	void WaterPass();

	/// <summary>
	/// Generates a river by drawing with a brush of veriable radius until it reaches a map edge
	/// </summary>
	void GenerateRiver(int XPos, int YPos, FVector2D Direction, bool JustTurned, int Radius, bool RadiusJustChanged, TArray<FRiverContainer*>* River);

	/// <summary>
	/// Changes tiles at location and within brush radius into water
	/// </summary>
	void CreateWaterWithBrush(int xpos, int ypos, int rads);

	/// <summary>
	/// Makes a tile at location into a water tile
	/// </summary>
	void MakeTileWater(int xpos, int ypos);

	/// <summary>
	/// Creates cliff tiles between tiles of different Height
	/// </summary>
	void CliffPass();

	/// <summary>
	/// Generates 8 point tiles from generated tile map
	/// </summary>
	void Generate9PointTiles();

	/// <summary>
	/// Spawn a map with 9 point tiles from generated tile map
	/// </summary>
	void SpawnWorld9PointTile();

	/// <summary>
	/// Check if a tile is higher than another tile
	/// </summary>
	/// <param name="Current">The tile which should be higher</param>
	/// <param name="Next">The tile which should be lower</param>
	/// <returns>Whether Current->Height > Next->Height</returns>
	int IsTileHigher(TTile* Current, TTile* Next);

	/// <summary>
	/// Create a new map point for tile, at a tile vertex location
	/// Replaces the current point at that map location if the new point is higher priority
	/// </summary>
	/// <param name="CurrentTile">The tile containing the point</param>
	/// <param name="CurrentPoint">The point already at the location</param>
	/// <param name="PointPosition">The location of the point relative to the tile</param>
	/// <returns>The map point at the location</returns>
	TMapPoint* MakeNewPoint(TTile* CurrentTile, TMapPoint CurrentPoint, ETilePointPos PointPosition);

private:
	/// <summary>
	/// The world map
	/// </summary>
	WorldMap* WorldMap;

	/// <summary>
	/// The 2D array of map points
	/// </summary>
	TMapPoint** Points;

	/// <summary>
	/// Get the tile at location
	/// </summary>
	/// <param name="x">X location</param>
	/// <param name="y">Y location</param>
	/// <returns></returns>
	TTile* GetTile(int x, int y);

	/// <summary>
	/// Traverse all tiles of the same height and type which are adjacent and place them into the same chunk
	/// </summary>
	/// <param name="Map">The chunk</param>
	/// <param name="ChunkNum">The index of the chunk</param>
	/// <param name="x">The current x location</param>
	/// <param name="y">The current y location</param>
	/// <param name="Height">The height of the chunk</param>
	/// <param name="TileType">The type of the chunk</param>
	void TraverseTileHeightChunk(TMap<TTile*, TTile*>* Map, int ChunkNum, int x, int y, int Height, ETileType TileType);


	/// <summary>
	/// Traverse all water tiles of the same body of water which are adjacent and place them into the same chunk
	/// Also set the lowest point to the water height
	/// </summary>
	/// <param name="Map">The chunk</param>
	/// <param name="ChunkNum">The index of the chunk</param>
	/// <param name="x">The current x location</param>
	/// <param name="y">The current y location</param>
	/// <param name="Height">The height of the chunk</param>
	/// <param name="TileType">The type of the chunk</param>
	void TraverseWaterChunk(TMap<TTile*, TTile*>* Map, int ChunkNum, int x, int y, int Height, ETileType TileType);
	
};
