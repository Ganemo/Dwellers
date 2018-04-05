// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "WorldChunk.generated.h"

/// <summary>
/// An actor which is spawned and contains all landscape actors for that chunk
/// Chunks are used to cut down on how much needs to be updated at a time.
/// </summary>
UCLASS()
class DWELLERS_API AWorldChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldChunk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// The runtime mesh used as a landscape, representing tiles
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	URuntimeMeshComponent* Landscape;

	/// <summary>
	/// The tile instances used for water
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	UInstancedStaticMeshComponent* Water;

	/// <summary>
	/// Array containing all vertex positions for this chunk
	/// </summary>
	TArray<FVector> Positions;

	/// <summary>
	/// Array containing all vertex normals for this chunk
	/// </summary>
	TArray<FVector> Normal;

	/// <summary>
	/// Array containing all UVs for each vertex of this chunk
	/// </summary>
	TArray<FVector2D> UV;

	/// <summary>
	/// Array containing all vertex tangents for each vertex of this chunk
	/// </summary>
	TArray<FRuntimeMeshTangent> Tangents;

	/// <summary>
	/// Array containing all vertex colors for this chunk
	/// </summary>
	TArray<FColor> Colors;

	/// <summary>
	/// Array containing all triangles used in the runtime mesh component
	/// </summary>
	TArray<int32> Triangles;
	
	/// <summary>
	/// Create a mesh section with the current mesh properties
	/// </summary>
	/// <param Name="Pos">The mesh section index</param>
	void CreateMeshSection(int Pos);

	/// <summary>
	/// Update the current mesh at the given index with the current mesh properties
	/// </summary>
	/// <param Name="Pos">The mesh section index</param>
	void UpdateMesh(int Pos);

	/// <summary>
	/// Map of mesh instance components 
	/// </summary>
	TMap<FString, UHierarchicalInstancedStaticMeshComponent*> InstancedMeshes;

	/// <summary>
	/// Check whener an instance mesh component exists with this Name
	/// </summary>
	/// <param Name="Name">The Name of the instance meshes</param>
	/// <returns>Whether the component is found</returns>
	bool ContainsMeshesFor(FString Name);

	/// <summary>
	/// Create an instanced static mesh component for a given Name
	/// </summary>
	/// <param Name="Name">The Name of the instance meshes</param>
	/// <returns>The created instance mesh</returns>
	UHierarchicalInstancedStaticMeshComponent* CreateMeshesFor(FString Name);

	/// <summary>
	/// Get the instance static mesh component for a given Name
	/// </summary>
	/// <param Name="Name">The Name of the instance meshes</param>
	/// <returns>The instance mesh</returns>
	UHierarchicalInstancedStaticMeshComponent* GetMeshesFor(FString Name);

	/// <summary>
	/// Add a mesh with a given transform for instance static mesh component of Name
	/// </summary>
	/// <param Name="Name">The Name of the instance meshes</param>
	/// <param Name="position">The world transform of the mesh</param>
	/// <returns>The index of the instance, -1 if component didn't exist</returns>
	int AddMesh(FString Name, FTransform Position);

	/// <summary>
	/// Remove a mesh ad index from the instance static mesh component of Name
	/// </summary>
	/// <param Name="Name">The Name of the instance meshes</param>
	/// <param Name="Pos">The index of instance</param>
	/// <returns>Whether the removal was successful</returns>
	bool RemoveMesh(FString Name, int Index);
};
