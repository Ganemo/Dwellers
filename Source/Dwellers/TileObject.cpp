// Fill out your copyright notice in the Description page of Project Settings.

#include "TileObject.h"
#include "Tiles.h"
#include "AnimalHotSpot.h"


void FTileObject::AffectTile(TTile* OwningTile, AWorldChunk* Chunk) {}

void FTileObject_Grass::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	//Make sure the owning tile is a ground tile
	if (OwningTile->TileType != ETileType::Ground)
		return;
	
	TGroundTile* GroundTile = (TGroundTile*)OwningTile;

	UHierarchicalInstancedStaticMeshComponent* InstanceComponent;

	//If there are no instances of grass in this chunk, create an instance component
	if (!Chunk->ContainsMeshesFor(GetName()))
	{
		InstanceComponent = Chunk->CreateMeshesFor(GetName());

		//Set with the mesh
		InstanceComponent->SetStaticMesh(
			Cast<UStaticMesh>(
				StaticLoadObject(
					UStaticMesh::StaticClass(),
					NULL,
					TEXT("StaticMesh'/Game/FieldGrassPackVol1/Meshes/Grass/SM_grass028.SM_grass028'")
				)
			)
		);

		//Set the material
		InstanceComponent->SetMaterial(0, 
			Cast<UMaterialInterface>(
				StaticLoadObject(
					UMaterialInterface::StaticClass(),
					NULL,
					TEXT("MaterialInstanceConstant'/Game/FieldGrassPackVol1/Materials/ChildMaterials/M_grass_advanced_Inst.M_grass_advanced_Inst'")
				)
			)
		);

		InstanceComponent->SetCastShadow(true);
		InstanceComponent->SetMobility(EComponentMobility::Static);
		InstanceComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InstanceComponent->SetCullDistances(GameEncapsulator::GetGame()->MinCull, GameEncapsulator::GetGame()->MaxCull);
	}
	//Get the current instances of grass
	else
	{
		InstanceComponent = Chunk->GetMeshesFor(GetName());
	}

	//Get the size of the grass instance mesh
	float Size = GroundTile->grassDensity * 4;

	//Add the instance, at random rotation, and location deviation from the center of the tile
	Instances.Add(
		InstanceComponent->AddInstance(
			FTransform(
				FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
				FVector(
				(OwningTile->location->x + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->Map->CellSize,
				(OwningTile->location->y + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->Map->CellSize,
				OwningTile->Height * (GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight)),
				FVector(Size, Size, Size*2)
			)
		)
	);
}
void FTileObject_Grass::Destroy(TTile* OwningTile, AWorldChunk* Chunk)
{
	//Remove all the instances
	for (int x = 0; x < Instances.Num(); ++x)
	{
		UE_LOG(LogTemp, Log, TEXT("Removing: %d"), Instances[x]);
		Chunk->RemoveMesh(GetName(), Instances[x]);
	}
}

void FTileObject_CatTail::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	UHierarchicalInstancedStaticMeshComponent* Cmpt;

	//If there are no instances of cattails in this chunk, create an instance component
	if (!Chunk->ContainsMeshesFor(GetName()))
	{
		Cmpt = Chunk->CreateMeshesFor(GetName());

		Cmpt->SetStaticMesh(Cast<UStaticMesh>(
			StaticLoadObject(
				UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Cattail02.Cattail02'"))
			)
		);
		Cmpt->SetMaterial(0, Cast<UMaterialInterface>(
			StaticLoadObject(
				UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/Geometry/CatTail_Inst.CatTail_Inst'"))
			)
		);

		Cmpt->SetCastShadow(true);
		Cmpt->SetMobility(EComponentMobility::Static);
		Cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cmpt->SetCullDistances(GameEncapsulator::GetGame()->MinCull, GameEncapsulator::GetGame()->MaxCull);
	}
	//Get the current instances of cattails
	else
	{
		Cmpt = Chunk->GetMeshesFor(GetName());
	}

	//The number of cattail instances to spawn
	int NumberOfTails = UFunctionLibrary::GetSeededRandInRange(1, 5);
	for (int x = 0; x < NumberOfTails; ++x)
	{
		//Spawn a cattail at a random location based on the tile, rotation, and size
		float Size = UFunctionLibrary::GetSeededRandInRange(1, 1.5);
		Cmpt->AddInstance(
			FTransform(
				FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
				FVector(
					OwningTile->location->x * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 3, GameEncapsulator::GetGame()->Map->CellSize / 3),
					OwningTile->location->y * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 3, GameEncapsulator::GetGame()->Map->CellSize / 3),
					OwningTile->Height * (GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight)),
				FVector(Size, Size, Size)
			)
		);
	}
}

void FTileObject_Tree::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	UHierarchicalInstancedStaticMeshComponent* Cmpt;

	//If there are no instances of this tree in this chunk, create an instance component
	if (!Chunk->ContainsMeshesFor(GetName()))
	{
		Cmpt = Chunk->CreateMeshesFor(GetName());

		Cmpt->SetStaticMesh(Cast<UStaticMesh>(
			StaticLoadObject(
				UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Tree29.Tree29'"))
			)
		);
		Cmpt->SetMaterial(0, Cast<UMaterialInterface>(
			StaticLoadObject(
				UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Trunks01Mat_Inst.Trunks01Mat_Inst'"))
			)
		);
		Cmpt->SetMaterial(1, Cast<UMaterialInterface>(
			StaticLoadObject(
				UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Decidious01Mat_Inst.Decidious01Mat_Inst'"))
			)
		);

		Cmpt->SetCastShadow(true);
		Cmpt->SetMobility(EComponentMobility::Static);
		Cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cmpt->SetCullDistances(GameEncapsulator::GetGame()->MinCull, GameEncapsulator::GetGame()->MaxCull);
	}
	//Get the current instances of this tree
	else
	{
		Cmpt = Chunk->GetMeshesFor(GetName());
	}

	//Spawn a tree at a random location based on the tile, rotation, and size
	Cmpt->AddInstance(
		FTransform(
			FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
			FVector(
				OwningTile->location->x * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 8, GameEncapsulator::GetGame()->Map->CellSize / 8),
				OwningTile->location->y * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 8, GameEncapsulator::GetGame()->Map->CellSize / 8),
				OwningTile->Height * (GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight)
			),
			FVector(UFunctionLibrary::GetSeededRandInRange(.45, .5), UFunctionLibrary::GetSeededRandInRange(.45, .5), UFunctionLibrary::GetSeededRandInRange(.45, .7))
		)
	);
}

void FTileObject_Stone::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	UHierarchicalInstancedStaticMeshComponent* Cmpt;

	//If there are no instances of this stone in this chunk, create an instance component
	if (!Chunk->ContainsMeshesFor(GetName()))
	{
		Cmpt = Chunk->CreateMeshesFor(GetName());

		UStaticMesh* msh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Rock12.Rock12'")));
		UMaterialInterface* mat1 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Rocks01Mat_Inst.Rocks01Mat_Inst'")));

		Cmpt->SetStaticMesh(msh);
		Cmpt->SetMaterial(0, mat1);

		Cmpt->SetCastShadow(true);
		Cmpt->SetMobility(EComponentMobility::Static);
		Cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cmpt->SetCullDistances(GameEncapsulator::GetGame()->MinCull, GameEncapsulator::GetGame()->MaxCull);
	}
	//Get the current instances of this stone
	else
	{
		Cmpt = Chunk->GetMeshesFor(GetName());
	}

	//Spawn a stone at a random location based on the tile, rotation, and size
	Cmpt->AddInstance(
		FTransform(
			FRotator(UFunctionLibrary::GetSeededRandInRange(0, 359), UFunctionLibrary::GetSeededRandInRange(0, 359), UFunctionLibrary::GetSeededRandInRange(0, 359)),
			FVector(
				OwningTile->location->x * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 8, GameEncapsulator::GetGame()->Map->CellSize / 8),
				OwningTile->location->y * GameEncapsulator::GetGame()->Map->CellSize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->Map->CellSize / 8, GameEncapsulator::GetGame()->Map->CellSize / 8),
				OwningTile->Height * (GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight)
			),
			FVector(UFunctionLibrary::GetSeededRandInRange(.45, 1), UFunctionLibrary::GetSeededRandInRange(.45, 1), UFunctionLibrary::GetSeededRandInRange(.45, 1))
		)
	);
}

void FTileObject_AnimalHotspot::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	//Spawn An animal hotspot 
	this->hotspot = (AAnimalHotSpot*)Chunk->GetWorld()->SpawnActor(
		AAnimalHotSpot::StaticClass(),
		new FVector(
			OwningTile->location->x * GameEncapsulator::GetGame()->Map->CellSize,
			OwningTile->location->y * GameEncapsulator::GetGame()->Map->CellSize,
			OwningTile->Height * (GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight)
		),
		new FRotator()
	);
}

//Get a vertex index for a vertex used for a certain tile
int GetPointForTile(int xpos, int ypos, int xdiff, int ydiff)
{
	return ((GameEncapsulator::GetGame()->Map->ChunkSize * 2 + 2) + ((GameEncapsulator::GetGame()->Map->ChunkSize * 2 + 1) * xpos * 2) + (2 * ypos)) + xdiff * (GameEncapsulator::GetGame()->Map->ChunkSize * 2 + 1) + ydiff;
}

void FTileObject_Road::AffectTile(TTile* OwningTile, AWorldChunk* Chunk)
{
	//Get the relative tile index for the chunk
	int xrelative = OwningTile->location->x % GameEncapsulator::GetGame()->Map->ChunkSize;
	int yrelative = OwningTile->location->y % GameEncapsulator::GetGame()->Map->ChunkSize;

	//Set the color of the center vertex of the tile to a road
	Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 0)] = FColor(0,0,255,0);

	TTile* ti;

	//Set color of right most point if right tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x + 1, OwningTile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 1, 0)] = FColor(0, 0, 255, 0);
		}
	}

	//Set color of left most point if left tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x - 1, OwningTile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, -1, 0)] = FColor(0, 0, 255, 0);
		}
	}

	//Set color of southern most point if southern tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x, OwningTile->location->y - 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, -1)] = FColor(0, 0, 255, 0);
		}
	}

	//Set color of northern most point if northern tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x, OwningTile->location->y + 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 1)] = FColor(0, 0, 255, 0);
		}
	}

	//Update the mesh to reflect the changes
	Chunk->UpdateMesh(0);
}

void FTileObject_Road::Destroy(TTile* OwningTile, AWorldChunk* Chunk)
{
	//Get the relative tile index for the chunk
	int xrelative = OwningTile->location->x % GameEncapsulator::GetGame()->Map->ChunkSize;
	int yrelative = OwningTile->location->y % GameEncapsulator::GetGame()->Map->ChunkSize;

	//Set the color back to the original tile color
	Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 0)] = OwningTile->GetColor();

	TTile* ti;

	//Set color of right most point if right tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x + 1, OwningTile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 1, 0)] = OwningTile->GetColor();
		}
	}

	//Set color of left most point if left tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x - 1, OwningTile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, -1, 0)] = OwningTile->GetColor();
		}
	}

	//Set color of southern most point if southern tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x, OwningTile->location->y - 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, -1)] = OwningTile->GetColor();
		}
	}

	//Set color of northern most point if northern tile is a road
	ti = GameEncapsulator::GetGame()->Map->GetTile(OwningTile->location->x, OwningTile->location->y + 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			Chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 1)] = OwningTile->GetColor();
		}
	}

	//Update the mesh to reflect the changes
	Chunk->UpdateMesh(0);
}
