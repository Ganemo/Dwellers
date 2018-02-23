// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldChunk.h"


// Sets default values
AWorldChunk::AWorldChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Landscape = CreateDefaultSubobject<URuntimeMeshComponent>("Mesh");
	Landscape->SetupAttachment(RootComponent);

	Water = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Water");
}
	
// Called when the game starts or when spawned
void AWorldChunk::BeginPlay()
{
	Super::BeginPlay();

	Landscape->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
}

// Called every frame
void AWorldChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldChunk::CreateMeshSection(int pos)
{
	Landscape->CreateMeshSection(pos, Positions, Triangles, Normal, UV, Colors, Tangents, true, EUpdateFrequency::Infrequent);
}

void AWorldChunk::UpdateMesh(int pos)
{
	Landscape->UpdateMeshSection(pos, Positions, Triangles, Normal, UV, Colors, Tangents, ESectionUpdateFlags::None);
}


bool AWorldChunk::ContainsMeshesFor(FString name)
{
	return InstancedMeshes.Contains(name);
}

UHierarchicalInstancedStaticMeshComponent* AWorldChunk::CreateMeshesFor(FString name)
{
	if (ContainsMeshesFor(name))
	{
		return *InstancedMeshes.Find(name);
	}

	UHierarchicalInstancedStaticMeshComponent* cmpt = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, UHierarchicalInstancedStaticMeshComponent::StaticClass(), *name);
	cmpt->RegisterComponent();

	InstancedMeshes.Add(name, cmpt);

	return cmpt;
}

UHierarchicalInstancedStaticMeshComponent* AWorldChunk::GetMeshesFor(FString name)
{
	if (ContainsMeshesFor(name))
	{
		return *InstancedMeshes.Find(name);
	}
	else
		return nullptr;
}

int AWorldChunk::AddMesh(FString name, FTransform position)
{
	if (ContainsMeshesFor(name))
	{
		UHierarchicalInstancedStaticMeshComponent* cmpt = *InstancedMeshes.Find(name);
		return cmpt->AddInstance(position);
	}
	else
	{
		return -1;
	}
}

bool AWorldChunk::RemoveMesh(FString name, int pos)
{
	if (ContainsMeshesFor(name))
	{
		UHierarchicalInstancedStaticMeshComponent* cmpt = *InstancedMeshes.Find(name);
		return cmpt->RemoveInstance(pos);
	}
	else
	{
		return false;
	}
}