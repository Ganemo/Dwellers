// Fill out your copyright notice in the Description page of Project Settings.

#include "TileObject.h"
#include "Tiles.h"
#include "AnimalHotSpot.h"


void FTileObject::AffectTile(TTile* owningtile, AWorldChunk* chunk) {}

void FTileObject_Grass::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	if (owningtile->type != ETileType::Ground)
		return;

	TGroundTile* grnd = (TGroundTile*)owningtile;

	UHierarchicalInstancedStaticMeshComponent* cmpt;

	if (!chunk->ContainsMeshesFor(GetName()))
	{
		cmpt = chunk->CreateMeshesFor(GetName());

		UStaticMesh* msh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FieldGrassPackVol1/Meshes/Grass/SM_grass028.SM_grass028'")));
		UMaterialInterface* mat1 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FieldGrassPackVol1/Materials/ChildMaterials/M_grass_advanced_Inst.M_grass_advanced_Inst'")));

		cmpt->SetStaticMesh(msh);
		cmpt->SetMaterial(0, mat1);

		cmpt->SetCastShadow(true);
		cmpt->SetMobility(EComponentMobility::Static);
		cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		cmpt->SetCullDistances(GameEncapsulator::GetGame()->mincull, GameEncapsulator::GetGame()->maxcull);
	}
	else
	{
		cmpt = chunk->GetMeshesFor(GetName());
	}

	float size = grnd->grassDensity * 4;

	instances.Add(
		cmpt->AddInstance(
			FTransform(
				FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
				FVector(
				(owningtile->location->x + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->map->cellsize,
				(owningtile->location->y + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->map->cellsize,
				owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)),
				FVector(size, size, size*2)
			)
		)
	);

	//int grass = 2;
	//for (int x = 0; x < grass; ++x)
	//{
	//	for (int y = 0; y < grass; ++y)
	//	{
	//		float size = grnd->grassDensity * 4;
	//
	//		instances.Add(
	//			cmpt->AddInstance(
	//				FTransform(
	//					FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
	//					FVector(
	//						(owningtile->location->x + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->map->cellsize,
	//						(owningtile->location->y + UFunctionLibrary::GetSeededRandInRange(-.25, .25)) * GameEncapsulator::GetGame()->map->cellsize,
	//						owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)),
	//					FVector(size, size, size * 2)
	//				)
	//			)
	//		);
	//	}
	//}

	/*
							(owningtile->location->x - .5 + ((1.0f / grass) * x)) * GameEncapsulator::GetGame()->map->cellsize,
							(owningtile->location->y - .5 + ((1.0f / grass) * y)) * GameEncapsulator::GetGame()->map->cellsize,*/
}
void FTileObject_Grass::Destroy(TTile* owningtile, AWorldChunk* chunk)
{
	for (int x = 0; x < instances.Num(); ++x)
	{
		UE_LOG(LogTemp, Log, TEXT("Removing: %d"), instances[x]);
		chunk->RemoveMesh(GetName(), instances[x]);
	}
}

void FTileObject_CatTail::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	UHierarchicalInstancedStaticMeshComponent* cmpt;

	if (!chunk->ContainsMeshesFor(GetName()))
	{
		cmpt = chunk->CreateMeshesFor(GetName());

		UStaticMesh* msh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Cattail02.Cattail02'")));
		UMaterialInterface* mat1 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/Geometry/CatTail_Inst.CatTail_Inst'")));

		cmpt->SetStaticMesh(msh);
		cmpt->SetMaterial(0, mat1);

		cmpt->SetCastShadow(true);
		cmpt->SetMobility(EComponentMobility::Static);
		cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		cmpt->SetCullDistances(GameEncapsulator::GetGame()->mincull, GameEncapsulator::GetGame()->maxcull);
	}
	else
	{
		cmpt = chunk->GetMeshesFor(GetName());
	}

	int tailnum = UFunctionLibrary::GetSeededRandInRange(1, 5);
	for (int x = 0; x < tailnum; ++x)
	{
		float size = UFunctionLibrary::GetSeededRandInRange(1, 1.5);
		cmpt->AddInstance(
			FTransform(
				FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
				FVector(
					owningtile->location->x * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 3, GameEncapsulator::GetGame()->map->cellsize / 3),
					owningtile->location->y * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 3, GameEncapsulator::GetGame()->map->cellsize / 3),
					owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)),
				FVector(size, size, size)
			)
		);
	}
}

void FTileObject_Tree::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	UHierarchicalInstancedStaticMeshComponent* cmpt;

	if (!chunk->ContainsMeshesFor(GetName()))
	{
		cmpt = chunk->CreateMeshesFor(GetName());

		UStaticMesh* msh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Tree29.Tree29'")));
		UMaterialInterface* mat1 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Trunks01Mat_Inst.Trunks01Mat_Inst'")));
		UMaterialInterface* mat2 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Decidious01Mat_Inst.Decidious01Mat_Inst'")));

		cmpt->SetStaticMesh(msh);
		cmpt->SetMaterial(0, mat1);
		cmpt->SetMaterial(1, mat2);

		cmpt->SetCastShadow(true);
		cmpt->SetMobility(EComponentMobility::Static);
		cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		cmpt->SetCullDistances(GameEncapsulator::GetGame()->mincull, GameEncapsulator::GetGame()->maxcull);
	}
	else
	{
		cmpt = chunk->GetMeshesFor(GetName());
	}

	cmpt->AddInstance(
		FTransform(
			FRotator(0, UFunctionLibrary::GetSeededRandInRange(0, 359), 0),
			FVector(
				owningtile->location->x * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 8, GameEncapsulator::GetGame()->map->cellsize / 8),
				owningtile->location->y * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 8, GameEncapsulator::GetGame()->map->cellsize / 8),
				owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)
			),
			FVector(UFunctionLibrary::GetSeededRandInRange(.45, .5), UFunctionLibrary::GetSeededRandInRange(.45, .5), UFunctionLibrary::GetSeededRandInRange(.45, .7))
		)
	);
}

void FTileObject_Stone::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	UHierarchicalInstancedStaticMeshComponent* cmpt;

	if (!chunk->ContainsMeshesFor(GetName()))
	{
		cmpt = chunk->CreateMeshesFor(GetName());

		UStaticMesh* msh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/FantasyEnvironment/Meshes/Rock12.Rock12'")));
		UMaterialInterface* mat1 = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/FantasyEnvironment/Materials/Rocks01Mat_Inst.Rocks01Mat_Inst'")));

		cmpt->SetStaticMesh(msh);
		cmpt->SetMaterial(0, mat1);

		cmpt->SetCastShadow(true);
		cmpt->SetMobility(EComponentMobility::Static);
		cmpt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		cmpt->SetCullDistances(GameEncapsulator::GetGame()->mincull, GameEncapsulator::GetGame()->maxcull);
	}
	else
	{
		cmpt = chunk->GetMeshesFor(GetName());
	}

	cmpt->AddInstance(
		FTransform(
			FRotator(UFunctionLibrary::GetSeededRandInRange(0, 359), UFunctionLibrary::GetSeededRandInRange(0, 359), UFunctionLibrary::GetSeededRandInRange(0, 359)),
			FVector(
				owningtile->location->x * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 8, GameEncapsulator::GetGame()->map->cellsize / 8),
				owningtile->location->y * GameEncapsulator::GetGame()->map->cellsize + UFunctionLibrary::GetSeededRandInRange(-GameEncapsulator::GetGame()->map->cellsize / 8, GameEncapsulator::GetGame()->map->cellsize / 8),
				owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)
			),
			FVector(UFunctionLibrary::GetSeededRandInRange(.45, 1), UFunctionLibrary::GetSeededRandInRange(.45, 1), UFunctionLibrary::GetSeededRandInRange(.45, 1))
		)
	);
}

void FTileObject_AnimalHotspot::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	this->hotspot = (AAnimalHotSpot*)chunk->GetWorld()->SpawnActor(
		AAnimalHotSpot::StaticClass(),
		new FVector(
			owningtile->location->x * GameEncapsulator::GetGame()->map->cellsize,
			owningtile->location->y * GameEncapsulator::GetGame()->map->cellsize,
			owningtile->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)
		),
		new FRotator()
	);
}

int GetPointForTile(int xpos, int ypos, int xdiff, int ydiff)
{
	return ((GameEncapsulator::GetGame()->map->chunksize * 2 + 2) + ((GameEncapsulator::GetGame()->map->chunksize * 2 + 1) * xpos * 2) + (2 * ypos)) + xdiff * (GameEncapsulator::GetGame()->map->chunksize * 2 + 1) + ydiff;
}

void FTileObject_Road::AffectTile(TTile* owningtile, AWorldChunk* chunk)
{
	int xrelative = owningtile->location->x % GameEncapsulator::GetGame()->map->chunksize;
	int yrelative = owningtile->location->y % GameEncapsulator::GetGame()->map->chunksize;

	chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 0)] = FColor(0,0,255,0);

	TTile* ti;

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x + 1, owningtile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 1, 0)] = FColor(0, 0, 255, 0);
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x - 1, owningtile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, -1, 0)] = FColor(0, 0, 255, 0);
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x, owningtile->location->y - 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 0, -1)] = FColor(0, 0, 255, 0);
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x, owningtile->location->y + 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 1)] = FColor(0, 0, 255, 0);
		}
	}

	chunk->UpdateMesh(0);
}

void FTileObject_Road::Destroy(TTile* owningtile, AWorldChunk* chunk)
{
	int xrelative = owningtile->location->x % GameEncapsulator::GetGame()->map->chunksize;
	int yrelative = owningtile->location->y % GameEncapsulator::GetGame()->map->chunksize;

	chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 0)] = owningtile->GetColor();

	TTile* ti;

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x + 1, owningtile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 1, 0)] = owningtile->GetColor();
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x - 1, owningtile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, -1, 0)] = owningtile->GetColor();
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x, owningtile->location->y - 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 0, -1)] = owningtile->GetColor();
		}
	}

	ti = GameEncapsulator::GetGame()->map->GetTile(owningtile->location->x, owningtile->location->y + 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			chunk->Colors[GetPointForTile(xrelative, yrelative, 0, 1)] = owningtile->GetColor();
		}
	}

	chunk->UpdateMesh(0);
}
