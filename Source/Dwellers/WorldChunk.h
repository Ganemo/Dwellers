// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "WorldChunk.generated.h"

UCLASS()
class DWELLERS_API AWorldChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldChunk();

	UPROPERTY(BlueprintReadOnly)
	URuntimeMeshComponent* Landscape;


	UPROPERTY(BlueprintReadOnly)
	UInstancedStaticMeshComponent* Water;

	/*
	UPROPERTY(BlueprintReadOnly)
	UHierarchicalInstancedStaticMeshComponent* Trees;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FVector> Positions;
	TArray<FVector> Normal;
	TArray<FVector2D> UV;
	TArray<FRuntimeMeshTangent> Tangents;
	TArray<FColor> Colors;
	TArray<int32> Triangles;
	
	void CreateMeshSection(int pos);
	void UpdateMesh(int pos);

	TMap<FString, UHierarchicalInstancedStaticMeshComponent*> InstancedMeshes;
	bool ContainsMeshesFor(FString name);
	UHierarchicalInstancedStaticMeshComponent* CreateMeshesFor(FString name);
	UHierarchicalInstancedStaticMeshComponent* GetMeshesFor(FString name);
	int AddMesh(FString name, FTransform position);
	bool RemoveMesh(FString name, int pos);
};
