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
	double t1 = FPlatformTime::Seconds();

	GameEncapsulator::CreateGame(HalfWidth * 2, ChunkSize, CellSize, cliffheight);
	worldmap = GameEncapsulator::GetGame()->map;

	GenerateWorld();

	double t2 = FPlatformTime::Seconds();
	WaterPass();

	double t3 = FPlatformTime::Seconds();
	CliffPass();

	double t4 = FPlatformTime::Seconds();
	Generate8PointTiles();

	double t5 = FPlatformTime::Seconds();
	SpawnWorld8PointTile();

	double finaltime = FPlatformTime::Seconds();

	UE_LOG(LogTemp, Log, TEXT("TIME TO BUILD:\nWorld Generated:\t%f\nWaterPass:\t%f\nCliffPass:\t%f\nPoints Generated:\t%f\nWorld Spawned:\t%f\nTOTAL:\t%f"), (t2-t1), (t3-t2), (t4-t3), (t5-t4), (finaltime-t5), finaltime-t1);
}

void AWorldGenerator::OnConstruction(const FTransform & Transform)
{
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
	FastNoise* heightnoise = new FastNoise();
	heightnoise->SetNoiseType(FastNoise::NoiseType::Perlin);
	heightnoise->SetFrequency(heightscale);
	FastNoise* heightnoise2 = new FastNoise();
	heightnoise2->SetNoiseType(FastNoise::NoiseType::Cubic);
	heightnoise2->SetFrequency(heightscale);


	FastNoise* grassdensitynoise = new FastNoise();
	grassdensitynoise->SetNoiseType(FastNoise::NoiseType::Simplex);
	grassdensitynoise->SetFrequency(typescale);

	
	FastNoise* treenoise = new FastNoise();
	treenoise->SetNoiseType(FastNoise::NoiseType::Simplex);
	treenoise->SetFrequency(treescale);


	FastNoise* animalnoise = new FastNoise();
	animalnoise->SetNoiseType(FastNoise::NoiseType::WhiteNoise);
	animalnoise->SetFrequency(1);

	
	if(!useseed)
	{
		seed = FMath::Rand();
	}
	FMath::SRandInit(seed);
	heightnoise->SetSeed(seed+1);
	grassdensitynoise->SetSeed(seed+3);
	treenoise->SetSeed(seed+4);
	animalnoise->SetSeed(seed+5);
	heightnoise2->SetSeed(seed+6);


	float height;
	for (int x = 0; x < HalfWidth * 2; ++x)
	{
		for (int y = 0; y < HalfWidth * 2; ++y)
		{
			int hg1 = FMath::RoundToInt(heightnoise->GetNoise(x, y) * 4);
			int hg2 = FMath::RoundToInt(heightnoise2->GetNoise(x, y) * 4);

			height = (hg1 + hg2)/2;

			TGroundTile* tile = new TGroundTile(new FTileLoc(x, y), height);

			float val = grassdensitynoise->GetNoise(x, y, height * 1000);
			val = val + (FMath::Sign(val) - val) / GrassValueShove;
			tile->grassDensity = val;

			if (val > grassthrsh)
			{
				if (treenoise->GetNoise(x, y, height * 1000) > treethrsh)
				{
					tile->object = new FTileObject_Tree();
				}
				else if (animalnoise->GetNoise(x, y, height * 1000) > animalthrsh)
				{
					tile->object = new FTileObject_AnimalHotspot();
				}
				else if (UFunctionLibrary::GetSeededRandBool(.01))
				{
					tile->object = new FTileObject_Stone();
				}
				else
				{
					tile->object = new FTileObject_Grass();
				}
			}
			else if (UFunctionLibrary::GetSeededRandBool(.01))
			{
				tile->object = new FTileObject_Stone();
			}

			worldmap->tiles[x][y] = tile;
		}
	}
}

FVector2D GetStartingDirection(int xval, int yval, int max)
{
	if (xval == 0)
	{
		return FVector2D(1, 0);
	}
	if(xval == max-1)
	{
		return FVector2D(-1, 0);
	}
	if (yval == 0)
	{
		return FVector2D(0, 1);
	}
	if (yval == max - 1)
	{
		return FVector2D(0, -1);
	}

	return FVector2D(0, 0);
}

void AWorldGenerator::WaterPass()
{
	
	int numrivers = UFunctionLibrary::GetSeededRandInRange(1, 3);

	for (int x = 0; x < numrivers; ++x)
	{
		int xval, yval;

		bool extremex = FMath::SRand() > .5f;

		//extreme x, else extreme y
		if (extremex)
		{
			xval = FMath::RoundToInt(FMath::SRand()) * (HalfWidth * 2 - 1);
			yval = FMath::RoundToInt(FMath::SRand() * (HalfWidth * 2 - 1));
		}
		else
		{
			xval = FMath::RoundToInt(FMath::SRand() * (HalfWidth * 2 - 1));
			yval = FMath::RoundToInt(FMath::SRand()) * (HalfWidth * 2 - 1);
		}

		TArray<FRiverContainer*> river;
		
		FVector2D st = GetStartingDirection(xval, yval, HalfWidth * 2);

		for(int starting = 0; starting < HalfWidth/2; ++starting)
			river.Add(new FRiverContainer(FMath::RoundToInt(xval + starting * st.X), FMath::RoundToInt(yval + starting * st.Y), 5));

		GenerateRiver(xval + 9 * st.X, yval + 9 * st.Y, st, true, 5, true, &river);

		int rivnum = river.Num();
		for (int num = 0; num < rivnum; ++num)
		{
			CreateWaterWithBrush(river[num]->xpos, river[num]->ypos, river[num]->radius);
		}
	}
}

void AWorldGenerator::GenerateRiver(int xpos, int ypos, FVector2D direction, bool justturned, int rad, bool justchangedrad, TArray<FRiverContainer*>* river)
{
	if (xpos < 0 || xpos >= HalfWidth * 2 || ypos < 0 || ypos >= HalfWidth * 2)
	{
		return;
	}

	int newrad = rad;

	if (!justchangedrad)
	{
		newrad = rad + (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);

		if (newrad < 2)
			newrad = 2;
		else if (newrad > 15)
			newrad = 15;
		else
			justchangedrad = true;
	}
	else
	{
		justchangedrad = false;
	}

	river->Add(new FRiverContainer(xpos, ypos, newrad));

	FVector2D dir(direction);

	//change direction
	if (!justturned && FMath::SRand() > .9f)
	{
		justturned = true;

		if (FMath::SRand() > .5f)
		{
			dir.X = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
		}
		else
		{
			dir.Y = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
		}

		//dont go backwards
		while (dir.X == -direction.X && dir.Y == -direction.Y)
		{
			if (FMath::SRand() > .5f)
			{
				dir.X = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
			}
			else
			{
				dir.Y = (FMath::RoundToInt(UFunctionLibrary::GetSeededRandInRange(0, 2)) - 1);
			}
		}
	}
	else
	{
		justturned = false;
	}

	GenerateRiver(xpos + FMath::RoundToInt(dir.X), ypos + FMath::RoundToInt(dir.Y), dir, justturned, newrad, justchangedrad, river);
}

void AWorldGenerator::CreateWaterWithBrush(int xpos, int ypos, int rads)
{
	MakeTileWater(xpos, ypos);

	//go right
	for (int x = 0; x < rads; ++x)
	{
		for (int y = 0; y < rads - x; ++y)
		{
			MakeTileWater(xpos + x, ypos + y);
			MakeTileWater(xpos + x, ypos - y);
		}
	}

	//go left
	for (int x = 0; x < rads; ++x)
	{
		for (int y = 0; y < rads - x; ++y)
		{
			MakeTileWater(xpos - x, ypos + y);
			MakeTileWater(xpos - x, ypos - y);
		}
	}

	//go up
	for (int y = 0; y < rads; ++y)
	{
		for (int x = 0; x < rads - y; ++x)
		{
			MakeTileWater(xpos + x, ypos + y);
			MakeTileWater(xpos - x, ypos + y);
		}
	}

	//go down
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
	if (xpos < 0 || xpos >= HalfWidth * 2 || ypos < 0 || ypos >= HalfWidth * 2)
	{
		return;
	}

	TTile* tile = GetTile(xpos, ypos);

	if (tile == nullptr || tile->type != ETileType::Water)
	{
		worldmap->tiles[xpos][ypos] = new TWaterTile(tile->location, -5);
	}
}

void AWorldGenerator::CliffPass()
{
	for (int x = 0; x < HalfWidth * 2; ++x)
	{
		for (int y = 0; y < HalfWidth * 2; ++y)
		{
			TTile* current = worldmap->tiles[x][y];
			if (!worldmap->IsTileInChunk(current))
			{
				TMap<TTile*, TTile*> map;
				int chunknum = worldmap->heightchunks.Num();

				if (current->type == ETileType::Water)
					TraverseWaterChunk(&map, chunknum, x, y, current->height, current->type);
				else if (current->type == ETileType::Ground)
					TraverseTileHeightChunk(&map, chunknum, x, y, current->height, current->type);
				else
					continue;

				worldmap->heightchunks.Add(map);
			}
		}
	}
}


void AWorldGenerator::TraverseTileHeightChunk(TMap<TTile*, TTile*>* map, int chunknum, int startx, int starty, int height, ETileType type)
{
	TArray<FTileLoc> TileStack;

	TileStack.Push(FTileLoc(startx, starty));

	while (TileStack.Num() > 0)
	{
		FTileLoc loc = TileStack.Pop();
		int x = loc.x;
		int y = loc.y;

		if (x < 0 || x >= HalfWidth * 2 || y < 0 || y >= HalfWidth * 2)
			continue;


		TTile* current = worldmap->tiles[x][y];

		if (map->Contains(current) || current->type != type)
			continue;

		if (current->height > height)
		{
			worldmap->tiles[x][y] = new TCliffTile(current->location, (current->height + height) / (2 + FMath::SRand() * 2 - 1));
			current = worldmap->tiles[x][y];
			map->Add(current, current);
			continue;
		}
		else if (current->height != height)
		{
			continue;
		}

		current->chunknum = chunknum;
		map->Add(current, current);

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

void AWorldGenerator::TraverseWaterChunk(TMap<TTile*, TTile*>* map, int chunknum, int x, int y, int height, ETileType type)
{
	if (x < 0 || x >= HalfWidth * 2 || y < 0 || y >= HalfWidth * 2)
		return;

	TTile* current = worldmap->tiles[x][y];

	if (map->Contains(current))
		return;

	if (current->type != ETileType::Water)
	{
		worldmap->tiles[x][y] = new TWaterEdgeTile(current->location, current->height);

		if (UFunctionLibrary::GetSeededRandBool(.7))
		{
			worldmap->tiles[x][y]->object = new FTileObject_CatTail();
		}
		current = worldmap->tiles[x][y];
		current->chunknum = chunknum;

		map->Add(current, current);

		//set the lowest point on the river for water spawning
		if (worldmap->waterchunkheight.Contains(chunknum))
		{
			int val = *worldmap->waterchunkheight.Find(chunknum);
			if (current->height < val)
			{
				worldmap->waterchunkheight.Add(chunknum, current->height);
			}
		}
		else
		{
			worldmap->waterchunkheight.Add(chunknum, current->height);
		}

		return;
	}

	current->chunknum = chunknum;
	map->Add(current, current);

	TraverseWaterChunk(map, chunknum, x + 1, y, height, type);
	TraverseWaterChunk(map, chunknum, x, y + 1, height, type);
	TraverseWaterChunk(map, chunknum, x, y - 1, height, type);
	TraverseWaterChunk(map, chunknum, x - 1, y, height, type);

	TraverseWaterChunk(map, chunknum, x + 1, y + 1, height, type);
	TraverseWaterChunk(map, chunknum, x - 1, y + 1, height, type);
	TraverseWaterChunk(map, chunknum, x + 1, y - 1, height, type);
	TraverseWaterChunk(map, chunknum, x - 1, y - 1, height, type);
}

TTile* AWorldGenerator::GetTile(int x, int y)
{
	if (x >= HalfWidth * 2 || y >= HalfWidth * 2 || x < 0 || y < 0)
		return nullptr;

	TTile* tilelookup = worldmap->tiles[x][y];
	if (tilelookup != nullptr)
		return tilelookup;
	else
		return nullptr;
}

int AWorldGenerator::IsTileHigher(TTile* current, TTile* next)
{
	if (current == nullptr || next == nullptr)
		return 0;

	return next->height - current->height;
}

TMap<int, FColor> colormapings;

FColor GetHeightChunkColors(TTile* tile)
{
	if (tile->type == ETileType::Cliff)
		return FColor::Black;

	if (tile->type == ETileType::WaterEdge)
		return FColor::White;

	FColor* col = colormapings.Find(tile->chunknum);
	if (col != nullptr)
	{
		return *col;
	}
	else
	{
		FColor returner = FColor::MakeRandomColor();
		colormapings.Add(tile->chunknum, returner);
		return returner;
	}
}

FColor GetVertexColors(TTile* tile)
{
	return tile->GetColor();
}

TTile* GetDominantTile(TTile* tile, TTile* other)
{
	if (other->type < tile->type)
		return other;
	else if (tile->type < other->type)
		return tile;
	else
	{
		if (tile->type == ETileType::Ground)
		{
			TGroundTile* grnd = (TGroundTile*)tile;
			TGroundTile* domgrnd = (TGroundTile*)other;

			if (grnd->grassDensity > domgrnd->grassDensity)
				return tile;
			else
				return domgrnd;
		}
		else
			return other;
	}
}
TMapPoint* AWorldGenerator::MakeNewPoint(TTile* currenttile, TMapPoint currentpoint, ETilePointPos pointpos)
{
	TTile* ti;

	if (currentpoint.tile == nullptr)
	{
		ti = currenttile;
	}
	else
	{
		ti = GetDominantTile(currenttile, currentpoint.tile);
	}

	return new TMapPoint(ti->height, (ShowHeightChunks ? GetHeightChunkColors(ti) : GetVertexColors(ti)), ti);
}

void AWorldGenerator::Generate8PointTiles()
{
	points = new TMapPoint*[(HalfWidth * 2) * 2 + 1];
	for (int x = 0; x <= (HalfWidth * 2) * 2 + 1; ++x)
		points[x] = new TMapPoint[(HalfWidth * 2) * 2 + 1];

	for (int x = 0; x < (HalfWidth * 2); ++x)
	{
		for (int y = 0; y < (HalfWidth * 2); ++y)
		{
			TTile* tile = GetTile(x, y);

			points[x * 2][y * 2] =			*MakeNewPoint(tile, points[x * 2][y * 2], ETilePointPos::BotRight);
			points[x * 2][y * 2 + 1] =		*MakeNewPoint(tile, points[x * 2][y * 2 + 1], ETilePointPos::Right);
			points[x * 2][y * 2 + 2] =		*MakeNewPoint(tile, points[x * 2][y * 2 + 2], ETilePointPos::TopRight);

			points[x * 2 + 1][y * 2] =		*MakeNewPoint(tile, points[x * 2 + 1][y * 2], ETilePointPos::Bot);
			points[x * 2 + 1][y * 2 + 1] =	*MakeNewPoint(tile, points[x * 2 + 1][y * 2 + 1], ETilePointPos::Center);
			points[x * 2 + 1][y * 2 + 2] =	*MakeNewPoint(tile, points[x * 2 + 1][y * 2 + 2], ETilePointPos::Top);

			points[x * 2 + 2][y * 2] =		*MakeNewPoint(tile, points[x * 2 + 2][y * 2], ETilePointPos::BotLeft);
			points[x * 2 + 2][y * 2 + 1] =	*MakeNewPoint(tile, points[x * 2 + 2][y * 2 + 1], ETilePointPos::Left);
			points[x * 2 + 2][y * 2 + 2] =	*MakeNewPoint(tile, points[x * 2 + 2][y * 2 + 2], ETilePointPos::TopLeft);
		}
	}
}

int GetPointFromReference(int mid, int width, int xdiff, int ydiff)
{
	return mid + xdiff * (width * 2 + 1) + ydiff;;
}

void AWorldGenerator::SpawnWorld8PointTile()
{
	UWorld* world = GetWorld();
	for (int chunkx = 0; chunkx < HalfWidth * 2 / ChunkSize; chunkx++)
	{
		for (int chunky = 0; chunky < HalfWidth * 2 / ChunkSize; chunky++)
		{
			AWorldChunk* chunk = world->SpawnActor<AWorldChunk>(FVector(chunkx * ChunkSize * CellSize, chunky * ChunkSize * CellSize, 0), FRotator(0, 0, 0));
			GameEncapsulator::GetGame()->map->chunks.Add(chunk);

			chunk->Water->SetStaticMesh(WaterMesh);
			chunk->Water->SetMaterial(0, WaterMaterial);
			chunk->Water->SetCullDistances(mindraw, maxdraw);

			URuntimeMeshComponent* cmpt = chunk->Landscape;
			cmpt->SetMaterial(0, Material);
			cmpt->bCastShadowAsTwoSided = true;

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			TArray<FVector>* Positions = &chunk->Positions;
			TArray<FVector>* Normal = &chunk->Normal;
			TArray<FVector2D>* UV = &chunk->UV;
			TArray<FRuntimeMeshTangent>* Tangents = &chunk->Tangents;
			TArray<FColor>* Colors = &chunk->Colors;
			TArray<int32>* Triangles = &chunk->Triangles;

			FBox BoundingBox = FBox(FVector(-ChunkSize / 2 * CellSize, -ChunkSize / 2 * CellSize, -200), FVector(ChunkSize / 2 * CellSize, ChunkSize / 2 * CellSize, 200));
			

			float Scale = CellSize / 50.0f;
			for (int32 x = 0; x < ChunkSize * 2 + 1; ++x)
			{
				for (int32 y = 0; y < ChunkSize * 2 + 1; ++y)
				{
					TMapPoint* pnt = &points[chunkx * (ChunkSize * 2) + x][chunky * (ChunkSize * 2) + y];

					Positions->Add(FVector(x * CellSize / 2 - CellSize / 2, y * CellSize / 2 - CellSize / 2, pnt->height * CellSize * cliffheight));
					Normal->Add(FVector(0, 0, 1));
					UV->Add(FVector2D(x * Scale, y * Scale));
					Tangents->Add(FRuntimeMeshTangent(-1, 0, 0));
					Colors->Add(pnt->vertexcolor);
				}
			}

			int xmove = ChunkSize * 2 + 1;
			for (int x = 0; x < ChunkSize; ++x)
			{
				for (int y = 0; y < ChunkSize; ++y)
				{
					int mid = xmove + 1 + (xmove * x * 2) + (2 * y);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, -1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, 0));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, 0));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, 1));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, 1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, 0, 1));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, 0, 1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, 1));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, 1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, 0));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, 0));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, -1));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, 1, -1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, 0, -1));
					Triangles->Add(mid);

					Triangles->Add(GetPointFromReference(mid, ChunkSize, 0, -1));
					Triangles->Add(GetPointFromReference(mid, ChunkSize, -1, -1));
					Triangles->Add(mid);
				}
			}

			chunk->CreateMeshSection(0);
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			for (int32 x = 0; x < ChunkSize; ++x)
			{
				for (int32 y = 0; y < ChunkSize; ++y)
				{
					TTile* tile;
					tile = GetTile(chunkx*ChunkSize + x, chunky*ChunkSize + y);
					if (tile)
					{

						if (tile->object != nullptr)
							tile->object->AffectTile(tile, chunk);

						if(tile->type == ETileType::Water || tile->type == ETileType::WaterEdge)
						{
							//TWaterTile* wtr = (TWaterTile*)tile;
							int* height = worldmap->waterchunkheight.Find(tile->chunknum);

							if (height != nullptr)
							{
								if (!ShowHeightChunks)
								{
									chunk->Water->AddInstance(
										FTransform(FRotator(0, 0, 0),
											FVector(
												(chunkx * ChunkSize + x) * CellSize,
												(chunky * ChunkSize + y) * CellSize,
												*height * (CellSize * cliffheight) - CellSize * WaterHeight),
											FVector((CellSize + 1) / 100, (CellSize + 1) / 100, (CellSize + 1) / 100)));
								}
							}
							else
							{
								if (!ShowHeightChunks)
								{
									chunk->Water->AddInstance(
										FTransform(FRotator(0, 0, 0),
											FVector(
											(chunkx * ChunkSize + x) * CellSize,
												(chunky * ChunkSize + y) * CellSize,
												10 * (CellSize * cliffheight) - CellSize * WaterHeight),
											FVector((CellSize + 1) / 100, (CellSize + 1) / 100, (CellSize + 1) / 100)));
								}
							}
						}
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
}