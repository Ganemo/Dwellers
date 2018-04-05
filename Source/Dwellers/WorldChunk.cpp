// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldChunk.h"


AWorldChunk::AWorldChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Landscape = CreateDefaultSubobject<URuntimeMeshComponent>("Mesh");
	Landscape->SetupAttachment(RootComponent);

	Water = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Water");
}
	
void AWorldChunk::BeginPlay()
{
	Super::BeginPlay();

	//Make sure traces on the landscape respond with a block
	Landscape->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
}

void AWorldChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorldChunk::CreateMeshSection(int Pos)
{
	Landscape->CreateMeshSection(Pos, Positions, Triangles, Normal, UV, Colors, Tangents, true, EUpdateFrequency::Infrequent);
}

void AWorldChunk::UpdateMesh(int Pos)
{
	Landscape->UpdateMeshSection(Pos, Positions, Triangles, Normal, UV, Colors, Tangents, ESectionUpdateFlags::None);
}

bool AWorldChunk::ContainsMeshesFor(FString Name)
{
	return InstancedMeshes.Contains(Name);
}

UHierarchicalInstancedStaticMeshComponent* AWorldChunk::CreateMeshesFor(FString Name)
{
	//Return the component instance if it already exists
	if (ContainsMeshesFor(Name))
	{
		return *InstancedMeshes.Find(Name);
	}

	//Create and register the component
	UHierarchicalInstancedStaticMeshComponent* Cmpt = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass(), *Name);
	Cmpt->RegisterComponent();

	//Add the component to the map of instanced mesh components
	InstancedMeshes.Add(Name, Cmpt);

	return Cmpt;
}

UHierarchicalInstancedStaticMeshComponent* AWorldChunk::GetMeshesFor(FString Name)
{
	//Get component if it exists
	//Return nullptr otherwise
	if (ContainsMeshesFor(Name))
	{
		return *InstancedMeshes.Find(Name);
	}
	else
		return nullptr;
}

int AWorldChunk::AddMesh(FString Name, FTransform Position)
{
	//Add an instance if the component exists
	//Return -1 if the component doesn't exist
	if (ContainsMeshesFor(Name))
	{
		UHierarchicalInstancedStaticMeshComponent* Cmpt = *InstancedMeshes.Find(Name);
		return Cmpt->AddInstance(Position);
	}
	else
		return -1;
}

bool AWorldChunk::RemoveMesh(FString Name, int Index)
{
	//Remove the instance at the index
	//Return false if the index is invalid or the component doesn't exist
	if (ContainsMeshesFor(Name))
	{
		UHierarchicalInstancedStaticMeshComponent* Cmpt = *InstancedMeshes.Find(Name);
		return Cmpt->RemoveInstance(Index);
	}
	else
		return false;
}