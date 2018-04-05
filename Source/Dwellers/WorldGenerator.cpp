// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGenerator.h"
#include "Dwellers.h"
#include "ConstructorHelpers.h"
#include "RuntimeMeshLibrary.h"
#include "AnimalHotSpot.h"
#include "GameEncapsulator.h"

#include <fstream>
using namespace std;


bool ShowHeightChunks = false;

AWorldGenerator::AWorldGenerator()
{
	//Mesh = CreateDefaultSubobject<URuntimeMeshComponent>("Mesh");
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AWorldGenerator::Destroyed()
{
}

void AWorldGenerator::BeginPlay()
{
	//Create a game instance to store the map at
	GameEncapsulator::CreateGame(HalfWidth * 2, ChunkSize, CellSize, CliffHeight);
	WorldMap = GameEncapsulator::GetGame()->Map;

	double T1 = FPlatformTime::Seconds();
	//Generate the world using noise
	GenerateWorld();

	double T2 = FPlatformTime::Seconds();
	//Generate water bodies
	WaterPass();

	double T3 = FPlatformTime::Seconds();
	//Create cliff and water edge tiles
	CliffPass();

	double T4 = FPlatformTime::Seconds();
	//Create tiles with 9 points from generated map
	Generate9PointTiles();

	double T5 = FPlatformTime::Seconds();
	//Spawn the tiles from the point map
	SpawnWorld9PointTile();

	double FinalTime = FPlatformTime::Seconds();

	UE_LOG(LogTemp, Log, TEXT("TIME TO BUILD:\nWorld Generated:\t%f\nWaterPass:\t%f\nCliffPass:\t%f\nPoints Generated:\t%f\nWorld Spawned:\t%f\nTOTAL:\t%f"), (T2-T1), (T3-T2), (T4-T3), (T5-T4), (FinalTime-T5), FinalTime-T1);
}

void AWorldGenerator::OnConstruction(const FTransform & Transform)
{
	//Ensure that the chunksize is a factor of the width and height of the map
	int remainder = (HalfWidth * 2) % ChunkSize;
	if (remainder != 0)
	{
		while (remainder != 0)
		{
			ChunkSize++;
			remainder = (HalfWidth * 2) % ChunkSize;
		}
	}
}

void AWorldGenerator::GenerateWorld()
{
	//Using two height noises to combine into more organic heights
	FastNoise* HeightNoise1 = new FastNoise();
	HeightNoise1->SetNoiseType(FastNoise::NoiseType::Perlin);
	HeightNoise1->SetFrequency(HeightScale);
	FastNoise* HeightNoise2 = new FastNoise();
	HeightNoise2->SetNoiseType(FastNoise::NoiseType::Cubic);
	HeightNoise2->SetFrequency(HeightScale);

	//Init the grass density noise
	FastNoise* GrassDensityNoise = new FastNoise();
	GrassDensityNoise->SetNoiseType(FastNoise::NoiseType::Simplex);
	GrassDensityNoise->SetFrequency(TypeScale);

	//Init the tree noise
	FastNoise* TreeNoise = new FastNoise();
	TreeNoise->SetNoiseType(FastNoise::NoiseType::Simplex);
	TreeNoise->SetFrequency(TreeScale);

	//Init the animal hot spot noise
	FastNoise* AnimalNoise = new FastNoise();
	AnimalNoise->SetNoiseType(FastNoise::NoiseType::WhiteNoise);
	AnimalNoise->SetFrequency(1);

	//Set the seed
	if(!UseSeed)
	{
		Seed = FMath::Rand();
	}

	//Initialize noises with seed
	FMath::SRandInit(Seed);
	HeightNoise1->SetSeed(Seed+1);
	GrassDensityNoise->SetSeed(Seed+3);
	TreeNoise->SetSeed(Seed+4);
	AnimalNoise->SetSeed(Seed+5);
	HeightNoise2->SetSeed(Seed+6);


	float Height;
	for (int x = 0; x < HalfWidth * 2; ++x)
	{
		for (int y = 0; y < HalfWidth * 2; ++y)
		{
			//Get the height of the tile
			Height = (FMath::RoundToInt(HeightNoise1->GetNoise(x, y) * 4) + FMath::RoundToInt(HeightNoise2->GetNoise(x, y) * 4))/2;

			//Create a ground tile
			TGroundTile* tile = new TGroundTile(new FTileLoc(x, y), Height);

			//Set the density of the tile
			float GrassDensity = GrassDensityNoise->GetNoise(x, y, Height * 1000);
			//Shove grass values into more extreme gradients
			GrassDensity += (FMath::Sign(GrassDensity) - GrassDensity) / GrassValueShove;
			//Set the grass density
			tile->grassDensity = GrassDensity;

			//If the density is above the threshold
			if (GrassDensity > GrassThrsh)
			{
				//Create a tree if it is above the tree threshold
				if (TreeNoise->GetNoise(x, y, Height * 1000) > TreeThrsh)
				{
					tile->object = new FTileObject_Tree();
				}
				//Create an animal hotspot if it is above the threshold
				else if (AnimalNoise->GetNoise(x, y, Height * 1000) > AnimalThrsh)
				{
					tile->object = new FTileObject_AnimalHotspot();
				}
				//Create a stone randomly
				else if (UFunctionLibrary::GetSeededRandBool(.01))
				{
					tile->object = new FTileObject_Stone();
				}
				//Create a grass object
				else
				{
					tile->object = new FTileObject_Grass();
				}
			}
			//Randomly create a stone
			else if (UFunctionLibrary::GetSeededRandBool(.01))
			{
				tile->object = new FTileObject_Stone();
			}

			//Set the tile on the map
			WorldMap->Tiles[x][y] = tile;
		}
	}
}

/// <summary>
/// Get the direction opposit of the closest wall for water drawing
/// </summary>
/// <param name="XValue">Starting X location</param>
/// <param name="YValue">Starting Y location</param>
/// <param name="Max">Highest location you can get to</param>
/// <returns>The starting direction for water drawing</returns>
FVector2D GetStartingDirection(int XValue, int YValue, int Max)
{
	if (XValue == 0)
		return FVector2D(1, 0);
	if(XValue == Max-1)
		return FVector2D(-1, 0);
	if (YValue == 0)
		return FVector2D(0, 1);
	if (YValue == Max - 1)
		return FVector2D(0, -1);
	else
		return FVector2D(0, 0);
}

void AWorldGenerator::WaterPass()
{
	int NumRivers = UFunctionLibrary::GetSeededRandInRange(1, 3);

	//Create a random number of rivers
	for (int x = 0; x < NumRivers; ++x)
	{
		int XVal, YVal;

		//Whether to start left/right or up/down
		bool ExtremeX = FMath::SRand() > .5f;

		//extreme x, else extreme y
		if (ExtremeX)
		{
			XVal = FMath::RoundToInt(FMath::SRand()) * (HalfWidth * 2 - 1);
			YVal = FMath::RoundToInt(FMath::SRand() * (HalfWidth * 2 - 1));
		}
		else
		{
			XVal = FMath::RoundToInt(FMath::SRand() * (HalfWidth * 2 - 1));
			YVal = FMath::RoundToInt(FMath::SRand()) * (HalfWidth * 2 - 1);
		}

		//The river being created
		TArray<FRiverContainer*> River;
		
		//Get the starting direction
		FVector2D StartLoc = GetStartingDirection(XVal, YVal, HalfWidth * 2);

		//Make river go 1/4 the map size in before randomly twisting
		for(int StartingTiles = 0; StartingTiles < HalfWidth/2; ++StartingTiles)
			River.Add(new FRiverContainer(FMath::RoundToInt(XVal + StartingTiles * StartLoc.X), FMath::RoundToInt(YVal + StartingTiles * StartLoc.Y), 5));

		//Create the single river tiles used to make river
		GenerateRiver(XVal + 9 * StartLoc.X, YVal + 9 * StartLoc.Y, StartLoc, true, 5, true, &River);

		//Paint the river using tiles
		int RiverTileNumber = River.Num();
		for (int num = 0; num < RiverTileNumber; ++num)
		{
			CreateWaterWithBrush(River[num]->xpos, River[num]->ypos, River[num]->radius);
		}
	}
}

void AWorldGenerator::GenerateRiver(int XPos, int YPos, FVector2D Direction, bool JustTurned, int Radius, bool RadiusJustChanged, TArray<FRiverContainer*>* River)
{
	//Make sure the locations are valid
	if (XPos < 0 || XPos >= HalfWidth * 2 || YPos < 0 || YPos >= HalfWidth * 2)
	{
		return;
	}

	int NewRadius = Radius;

	//Only change the radius of it wasn't just changed
	if (!RadiusJustChanged)
	{
		//Increase the radius by += 1
		NewRadius = Radius + (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);

		//Make sure the radius is atleast 2
		if (NewRadius < 2)
			NewRadius = 2;
		//Make sure the radius is not greater than 15
		else if (NewRadius > 15)
			NewRadius = 15;
		//Set that the radius just changed
		else
			RadiusJustChanged = true;
	}
	else
	{
		RadiusJustChanged = false;
	}

	//Add the new tile to the river
	River->Add(new FRiverContainer(XPos, YPos, NewRadius));

	//Next direction
	FVector2D Dir(Direction);

	//Change direction if it didn't just change
	if (!JustTurned && FMath::SRand() > .9f)
	{
		JustTurned = true;

		//Change horizontally or vertically
		if (FMath::SRand() > .5f)
		{
			Dir.X = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
		}
		else
		{
			Dir.Y = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
		}

		//Dont go backwards
		while (Dir.X == -Direction.X && Dir.Y == -Direction.Y)
		{
			if (FMath::SRand() > .5f)
			{
				Dir.X = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
			}
			else
			{
				Dir.Y = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
			}
		}
	}
	else
	{
		JustTurned = false;
	}

	//Recursive call
	GenerateRiver(XPos + FMath::RoundToInt(Dir.X), YPos + FMath::RoundToInt(Dir.Y), Dir, JustTurned, NewRadius, RadiusJustChanged, River);
}

void AWorldGenerator::CreateWaterWithBrush(int xpos, int ypos, int rads)
{
	MakeTileWater(xpos, ypos);

	//Go right
	for (int x = 0; x < rads; ++x)
	{
		for (int y = 0; y < rads - x; ++y)
		{
			MakeTileWater(xpos + x, ypos + y);
			MakeTileWater(xpos + x, ypos - y);
		}
	}

	//Go left
	for (int x = 0; x < rads; ++x)
	{
		for (int y = 0; y < rads - x; ++y)
		{
			MakeTileWater(xpos - x, ypos + y);
			MakeTileWater(xpos - x, ypos - y);
		}
	}

	//Go up
	for (int y = 0; y < rads; ++y)
	{
		for (int x = 0; x < rads - y; ++x)
		{
			MakeTileWater(xpos + x, ypos + y);
			MakeTileWater(xpos - x, ypos + y);
		}
	}

	//Go down
	for (int y = 0; y < rads; ++y)
	{
		for (int x = 0; x < rads - y; ++x)
		{
			MakeTileWater(xpos + x, ypos - y);
			MakeTileWater(xpos - x, ypos - y);
		}
	}
}

void AWorldGenerator::MakeTileWater(int xpos, int ypos)
{
	//Make sure location is valid
	if (xpos < 0 || xpos >= HalfWidth * 2 || ypos < 0 || ypos >= HalfWidth * 2)
	{
		return;
	}

	//Get the tile
	TTile* Tile = GetTile(xpos, ypos);

	//Make it into a water tile if it is valid and not already a water tile
	if (Tile == nullptr || Tile->TileType != ETileType::Water)
	{
		WorldMap->Tiles[xpos][ypos] = new TWaterTile(Tile->location, -5);
	}
}

void AWorldGenerator::CliffPass()
{
	for (int x = 0; x < HalfWidth * 2; ++x)
	{
		for (int y = 0; y < HalfWidth * 2; ++y)
		{
			//Get the tile
			TTile* Current = WorldMap->Tiles[x][y];

			//Make sure the tile isn't already checked
			if (!WorldMap->IsTileInChunk(Current))
			{
				//Create the chunk and get its index
				TMap<TTile*, TTile*> Map;
				int ChunkNum = WorldMap->HeightChunks.Num();

				//Traverse as water chunk if water tile
				if (Current->TileType == ETileType::Water)
					TraverseWaterChunk(&Map, ChunkNum, x, y, Current->Height, Current->TileType);
				//Traverse as normal tile if ground tile
				else if (Current->TileType == ETileType::Ground)
					TraverseTileHeightChunk(&Map, ChunkNum, x, y, Current->Height, Current->TileType);
				//Don't traverse if anything else
				else
					continue;

				//Add the chunk to the registry
				WorldMap->HeightChunks.Add(Map);
			}
		}
	}
}

void AWorldGenerator::TraverseTileHeightChunk(TMap<TTile*, TTile*>* Map, int ChunkNum, int StartX, int StartY, int Height, ETileType TileType)
{
	//Frontier
	TArray<FTileLoc> TileStack;

	//Add the starting tile
	TileStack.Push(FTileLoc(StartX, StartY));

	while (TileStack.Num() > 0)
	{
		//Get the current tile location
		FTileLoc Loc = TileStack.Pop();
		int x = Loc.x;
		int y = Loc.y;

		//Make sure it's valid
		if (x < 0 || x >= HalfWidth * 2 || y < 0 || y >= HalfWidth * 2)
			continue;

		//Get the tile at the location
		TTile* Current = WorldMap->Tiles[x][y];

		//Make sure tile hasn't been checked and is of the same type
		if (Map->Contains(Current) || Current->TileType != TileType)
			continue;

		//Make sure tiles are the same height
		if (Current->Height > Height)
		{
			//Make the buffer tile between into a cliff
			WorldMap->Tiles[x][y] = new TCliffTile(Current->location, (Current->Height + Height) / (2 + FMath::SRand() * 2 - 1));
			Current = WorldMap->Tiles[x][y];
			Map->Add(Current, Current);
			continue;
		}
		else if (Current->Height != Height)
		{
			continue;
		}

		//Set the chunk number to this tile
		Current->ChunkNum = ChunkNum;
		//Add it to the chunk
		Map->Add(Current, Current);

		////Add locations around the tile to the frontier////
		TileStack.Push(FTileLoc(x + 1, y));
		TileStack.Push(FTileLoc(x, y + 1));
		TileStack.Push(FTileLoc(x, y - 1));
		TileStack.Push(FTileLoc(x - 1, y));
		TileStack.Push(FTileLoc(x + 1, y + 1));
		TileStack.Push(FTileLoc(x - 1, y + 1));
		TileStack.Push(FTileLoc(x + 1, y - 1));
		TileStack.Push(FTileLoc(x - 1, y - 1));
	}
}

void AWorldGenerator::TraverseWaterChunk(TMap<TTile*, TTile*>* Map, int ChunkNum, int StartX, int StartY, int Height, ETileType TileType)
{
	//Make sure the starting location is valid
	if (StartX < 0 || StartX >= HalfWidth * 2 || StartY < 0 || StartY >= HalfWidth * 2)
		return;

	//Get the tile at the location
	TTile* Current = WorldMap->Tiles[StartX][StartY];

	//Make sure tile hasn't been checked already
	if (Map->Contains(Current))
		return;

	//If the next tile isn't a water tile, make it into a water edge tile
	if (Current->TileType != ETileType::Water)
	{
		WorldMap->Tiles[StartX][StartY] = new TWaterEdgeTile(Current->location, Current->Height);

		//Randomly generate cat tail objects for the water edge tile
		if (UFunctionLibrary::GetSeededRandBool(.7))
		{
			WorldMap->Tiles[StartX][StartY]->object = new FTileObject_CatTail();
		}
		//Set the chunk num
		Current = WorldMap->Tiles[StartX][StartY];
		Current->ChunkNum = ChunkNum;

		//Add tile to the current chunk
		Map->Add(Current, Current);

		//set the lowest point on the river for water spawning
		if (WorldMap->WaterChunkHeight.Contains(ChunkNum))
		{
			int val = *WorldMap->WaterChunkHeight.Find(ChunkNum);
			if (Current->Height < val)
			{
				WorldMap->WaterChunkHeight.Add(ChunkNum, Current->Height);
			}
		}
		else
		{
			WorldMap->WaterChunkHeight.Add(ChunkNum, Current->Height);
		}

		return;
	}

	Current->ChunkNum = ChunkNum;
	Map->Add(Current, Current);

	////Search tiles areound current tile///
	TraverseWaterChunk(Map, ChunkNum, StartX + 1, StartY, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX, StartY + 1, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX, StartY - 1, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX - 1, StartY, Height, TileType);

	TraverseWaterChunk(Map, ChunkNum, StartX + 1, StartY + 1, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX - 1, StartY + 1, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX + 1, StartY - 1, Height, TileType);
	TraverseWaterChunk(Map, ChunkNum, StartX - 1, StartY - 1, Height, TileType);
}

TTile* AWorldGenerator::GetTile(int x, int y)
{
	//Make sure location is valid
	if (x >= HalfWidth * 2 || y >= HalfWidth * 2 || x < 0 || y < 0)
		return nullptr;

	//Get the tile from the map
	TTile* TileLookup = WorldMap->Tiles[x][y];
	
	//Make sure it's valid
	if (TileLookup != nullptr)
		return TileLookup;
	else
		return nullptr;
}

int AWorldGenerator::IsTileHigher(TTile* Current, TTile* Next)
{
	//Make sure both tiles are valid
	if (Current == nullptr || Next == nullptr)
		return 0;

	//Return the difference in height
	return Next->Height - Current->Height;
}

TMap<int, FColor> ColoMapings;

/// <summary>
/// Show chunks as differnt colors for debug 
/// </summary>
/// <param name="Tile">The tile we are getting the chunk color for</param>
/// <returns></returns>
FColor GetHeightChunkColors(TTile* Tile)
{
	//Make cliffs black
	if (Tile->TileType == ETileType::Cliff)
		return FColor::Black;

	//Make water edges white
	if (Tile->TileType == ETileType::WaterEdge)
		return FColor::White;

	//Get the chunk color for a chunk color that has been created already
	FColor* Color = ColoMapings.Find(Tile->ChunkNum);
	if (Color != nullptr)
	{
		return *Color;
	}
	//Generate a random color if not
	else
	{
		FColor returner = FColor::MakeRandomColor();
		ColoMapings.Add(Tile->ChunkNum, returner);
		return returner;
	}
}

/// <summary>
/// Get the dominant tile between two tiles
/// </summary>
/// <param name="Tile">The first tile</param>
/// <param name="Other">The second tile</param>
/// <returns>The dominant tile</returns>
TTile* GetDominantTile(TTile* Tile, TTile* Other)
{
	//Return the lowest tile type
	if (Other->TileType < Tile->TileType)
		return Other;
	else if (Tile->TileType < Other->TileType)
		return Tile;
	//If the same tile type
	else
	{
		//If the type is ground, return the highest grass density
		if (Tile->TileType == ETileType::Ground)
		{
			if (((TGroundTile*)Tile)->grassDensity > ((TGroundTile*)Other)->grassDensity)
				return Tile;
			else
				return Other;
		}
		//Return the other tile
		else
			return Other;
	}
}

TMapPoint* AWorldGenerator::MakeNewPoint(TTile* CurrentTile, TMapPoint CurrentPoint, ETilePointPos PointPosition)
{
	TTile* Tile;
	
	//Get the most dominant tile
	if (CurrentPoint.tile == nullptr)
		Tile = CurrentTile;
	else
		Tile = GetDominantTile(CurrentTile, CurrentPoint.tile);

	//Create a map point for the dominant tile
	return new TMapPoint(Tile->Height, (ShowHeightChunks ? GetHeightChunkColors(Tile) : Tile->GetColor()), Tile);
}

void AWorldGenerator::Generate9PointTiles()
{
	//Create the 2D array of points
	Points = new TMapPoint*[(HalfWidth * 2) * 2 + 1];
	for (int x = 0; x <= (HalfWidth * 2) * 2 + 1; ++x)
		Points[x] = new TMapPoint[(HalfWidth * 2) * 2 + 1];

	//Iterate over tiles
	for (int x = 0; x < (HalfWidth * 2); ++x)
	{
		for (int y = 0; y < (HalfWidth * 2); ++y)
		{
			//Get the tile at indexes
			TTile* Tile = GetTile(x, y);

			//Make a point for the bottom right point
			Points[x * 2][y * 2] =			*MakeNewPoint(Tile, Points[x * 2][y * 2], ETilePointPos::BotRight);
			//Make a point for the right point
			Points[x * 2][y * 2 + 1] =		*MakeNewPoint(Tile, Points[x * 2][y * 2 + 1], ETilePointPos::Right);
			//Make a point for the top right point
			Points[x * 2][y * 2 + 2] =		*MakeNewPoint(Tile, Points[x * 2][y * 2 + 2], ETilePointPos::TopRight);

			//Make a point for the bottom point
			Points[x * 2 + 1][y * 2] =		*MakeNewPoint(Tile, Points[x * 2 + 1][y * 2], ETilePointPos::Bot);
			//Make a point for the center point
			Points[x * 2 + 1][y * 2 + 1] =	*MakeNewPoint(Tile, Points[x * 2 + 1][y * 2 + 1], ETilePointPos::Center);
			//Make a point for the top point
			Points[x * 2 + 1][y * 2 + 2] =	*MakeNewPoint(Tile, Points[x * 2 + 1][y * 2 + 2], ETilePointPos::Top);

			//Make a point for the bottom left point
			Points[x * 2 + 2][y * 2] =		*MakeNewPoint(Tile, Points[x * 2 + 2][y * 2], ETilePointPos::BotLeft);
			//Make a point for the left point
			Points[x * 2 + 2][y * 2 + 1] =	*MakeNewPoint(Tile, Points[x * 2 + 2][y * 2 + 1], ETilePointPos::Left);
			//Make a point for the top left point
			Points[x * 2 + 2][y * 2 + 2] =	*MakeNewPoint(Tile, Points[x * 2 + 2][y * 2 + 2], ETilePointPos::TopLeft);
		}
	}
}

/// <summary>
/// Get a point for a tile, with middle point.
/// Needed to make triangles from points ina a 1D array
/// </summary>
/// <param name="Mid">The middle point</param>
/// <param name="Width">The width and height of the map</param>
/// <param name="XDiff">The distance from the mid in the X direction</param>
/// <param name="YDiff">The distance from the mid in the y direction</param>
/// <returns>The location in the points array</returns>
int inline GetPointFromReference(int Mid, int Width, int XDiff, int YDiff)
{
	return Mid + XDiff * (Width * 2 + 1) + YDiff;
}

void AWorldGenerator::SpawnWorld9PointTile()
{
	//Get the world t ospawn in
	UWorld* World = GetWorld();

	//Traverse the chunks
	for (int chunkx = 0; chunkx < HalfWidth * 2 / ChunkSize; chunkx++)
	{
		for (int chunky = 0; chunky < HalfWidth * 2 / ChunkSize; chunky++)
		{
			//Create a chunk
			AWorldChunk* Chunk = World->SpawnActor<AWorldChunk>(FVector(chunkx * ChunkSize * CellSize, chunky * ChunkSize * CellSize, 0), FRotator(0, 0, 0));
			GameEncapsulator::GetGame()->Map->Chunks.Add(Chunk);

			//Init chunk water instances
			Chunk->Water->SetStaticMesh(WaterMesh);
			Chunk->Water->SetMaterial(0, WaterMaterial);
			Chunk->Water->SetCullDistances(MinDraw, MaxDraw);

			//Init chunk landscape
			URuntimeMeshComponent* Cmpt = Chunk->Landscape;
			Cmpt->SetMaterial(0, Material);
			Cmpt->bCastShadowAsTwoSided = true;

			///////////Spawn everything landscape////////////
			// Get the mesh property arrays
			TArray<FVector>* Positions = &Chunk->Positions;
			TArray<FVector>* Normal = &Chunk->Normal;
			TArray<FVector2D>* UV = &Chunk->UV;
			TArray<FRuntimeMeshTangent>* Tangents = &Chunk->Tangents;
			TArray<FColor>* Colors = &Chunk->Colors;
			TArray<int32>* Triangles = &Chunk->Triangles;

			//Create the bounding box to the size of the chunk
			FBox BoundingBox = FBox(FVector(-ChunkSize / 2 * CellSize, -ChunkSize / 2 * CellSize, -200), FVector(ChunkSize / 2 * CellSize, ChunkSize / 2 * CellSize, 200));
			
			//Set the scale of the cells
			float Scale = CellSize / 50.0f;

			//Traverse the chunk tiles
			for (int32 x = 0; x < ChunkSize * 2 + 1; ++x)
			{
				for (int32 y = 0; y < ChunkSize * 2 + 1; ++y)
				{
					//Get the tile point
					TMapPoint* Point = &Points[chunkx * (ChunkSize * 2) + x][chunky * (ChunkSize * 2) + y];

					//Add properties of tile point
					Positions->Add(FVector(x * CellSize / 2 - CellSize / 2, y * CellSize / 2 - CellSize / 2, Point->Height * CellSize * CliffHeight));
					Normal->Add(FVector(0, 0, 1));
					UV->Add(FVector2D(x * Scale, y * Scale));
					Tangents->Add(FRuntimeMeshTangent(-1, 0, 0));
					Colors->Add(Point->vertexcolor);
				}
			}

			//Traverse the chunk tiles
			int XMove = ChunkSize * 2 + 1;
			for (int x = 0; x < ChunkSize; ++x)
			{
				for (int y = 0; y < ChunkSize; ++y)
				{
					//Get the middle point of the tile
					int Mid = XMove + 1 + (XMove * x * 2) + (2 * y);

					////Add all the triangles for each tile////
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, -1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, 0));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, 0));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, 1));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, 1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 0, 1));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 0, 1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, 1));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, 1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, 0));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, 0));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, -1));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 1, -1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 0, -1));
					Triangles->Add(Mid);

					Triangles->Add(GetPointFromReference(Mid, ChunkSize, 0, -1));
					Triangles->Add(GetPointFromReference(Mid, ChunkSize, -1, -1));
					Triangles->Add(Mid);
				}
			}

			//Create the mesh
			Chunk->CreateMeshSection(0);
			//////////////////////////////////////////////////////////////

			///////////Spawn everything that isn't landscape////////////
			for (int32 x = 0; x < ChunkSize; ++x)
			{
				for (int32 y = 0; y < ChunkSize; ++y)
				{
					//Get the current tile nad make sure it's valid
					TTile* CurrentTile = GetTile(chunkx*ChunkSize + x, chunky*ChunkSize + y);
					
					//Make sure it's valid
					if (CurrentTile != nullptr)
					{
						//If the tile has an object, let it affect the tile
						if (CurrentTile->object != nullptr)
							CurrentTile->object->AffectTile(CurrentTile, Chunk);

						//Create spawn water tile instances
						if(CurrentTile->TileType == ETileType::Water || CurrentTile->TileType == ETileType::WaterEdge)
						{
							//Get the height for that water chunk
							int* Height = WorldMap->WaterChunkHeight.Find(CurrentTile->ChunkNum);

							//Make sure the height exists
							if (Height != nullptr)
							{
								//Spawn a water instance if we aren't in chunk debug mode
								if (!ShowHeightChunks)
								{
									Chunk->Water->AddInstance(
										FTransform(FRotator(0, 0, 0),
											FVector(
												(chunkx * ChunkSize + x) * CellSize,
												(chunky * ChunkSize + y) * CellSize,
												*Height * (CellSize * CliffHeight) - CellSize * WaterHeight),
											FVector((CellSize + 1) / 100, (CellSize + 1) / 100, (CellSize + 1) / 100)));
								}
							}
							else
							{
								//Spawn the water instance much higher if there is no water chunk height
								//This shouldn't happen
								if (!ShowHeightChunks)
								{
									Chunk->Water->AddInstance(
										FTransform(FRotator(0, 0, 0),
											FVector(
											(chunkx * ChunkSize + x) * CellSize,
												(chunky * ChunkSize + y) * CellSize,
												10 * (CellSize * CliffHeight) - CellSize * WaterHeight),
											FVector((CellSize + 1) / 100, (CellSize + 1) / 100, (CellSize + 1) / 100)));
								}
							}
						}
					}
				}
			}
			////////////////////////////////////////////////////////////////
		}
	}
}