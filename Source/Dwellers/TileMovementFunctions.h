// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameEncapsulator.h"
#include "Async/AsyncWork.h"
#include "Entity.h"
#include "WorldMap.h"
#include "TileMovementFunctions.generated.h"

/// <summary>
/// A priority node used in a linked list for A* pathfinding
/// </summary>
class DWELLERS_API PriorityNode
{

public:
	/// <summary>
	/// The node's tile
	/// </summary>
	TTile* Tile;

	/// <summary>
	/// The node's priority, created based on a heuristic
	/// </summary>
	float Priority;

	/// <summary>
	/// The next node in the linked list
	/// </summary>
	PriorityNode* Next = nullptr;

	PriorityNode::PriorityNode(TTile* Tile, float Priority)
	{
		this->Tile = Tile;
		this->Priority = Priority;
	}
};

/// <summary>
/// A priority queue used in A* pathfinding
/// Priority is in ascending order
/// </summary>
class DWELLERS_API PriorityQueue
{

public:
	/// <summary>
	/// The head of the queue
	/// </summary>
	PriorityNode* Head = nullptr;

	/// <summary>
	/// Add a node to the queue
	/// </summary>
	/// <param Name="Node">The node to add</param>
	void PriorityQueue::AddNode(PriorityNode* Node)
	{
		//Make sure node is valid
		if (Node == nullptr)
			return;

		//Set the node to the head if there is no head
		if (Head == nullptr)
		{
			Head = Node;
			return;
		}

		//Add the new node to in the correct position based on ascending priority
		PriorityNode* Current = Head;
		while (Current->Next != nullptr)
		{
			//If the node's priority is less than the Current node's priority
			//Insert this node
			if (Node->Priority < Current->Next->Priority)
			{
				Node->Next = Current->Next;
				Current->Next = Node;
				return;
			}
			//Continue down the list
			else
				Current = Current->Next;
		}
		//If the End of the list is reached, add this node to the End of the list
		Current->Next = Node;
	}

	/// <summary>
	/// Get the highest priority node
	/// </summary>
	/// <returns>The tile</returns>
	TTile* PriorityQueue::Pluck()
	{
		if (Head == nullptr)
			return nullptr;
		
		//Get the head
		TTile* Tile = Head->Tile;

		//Set the next node to the head
		Head = Head->Next;

		return Tile;
	}

	/// <summary>
	/// Check if the list is empty
	/// </summary>
	/// <returns>Whether the list is empty</returns>
	bool PriorityQueue::IsEmpty()
	{
		return Head == nullptr;
	}
};



/// <summary>
/// Blueprint function library used to do async pathfunding
/// </summary>
UCLASS()
class DWELLERS_API UTileMovementFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/// <summary>
	/// Get the Path between two points
	/// </summary>
	/// <param Name="Start">The starting location</param>
	/// <param Name="End">The ending location</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	/// <returns>The Path</returns>
	UFUNCTION(BlueprintCallable, Category = TileMovement)
	static TArray<FVector> GetTilePathFromPoints(FVector Start, FVector End, bool Cardinals);

	/// <summary>
	/// Asyncronosly find a Path between an Actor and a location and move the Actor to that location
	/// </summary>
	/// <param Name="Target">Actor to move</param>
	/// <param Name="End">The ending location</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	UFUNCTION(BlueprintCallable, Category = TileMovement)
	static void AsyncMoveActorTo(AEntity* Target, FVector End, bool Cardinals);

	/// <summary>
	/// Get the Path between two tiles
	/// </summary>
	/// <param Name="Start">The starting tile</param>
	/// <param Name="End">The ending tile</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	/// <returns>The Path</returns>
	static TArray<TTile*> GetTilePath(TTile* Start, TTile* End, bool Cardinals);

	/// <summary>
	/// Get the Path between two tiles, not using tile movement weights
	/// </summary>
	/// <param Name="Start">The starting tile</param>
	/// <param Name="End">The ending tile</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	/// <returns>The Path</returns>
	static TArray<TTile*> GetTilePath_NoWeight(TTile* Start, TTile* End, bool Cardinals);

	/// <summary>
	/// Get the Path between two tiles, used for recursive calls
	/// </summary>
	/// <param Name="Path">The returning path</param>
	/// <param Name="Tiles">The map of tiles</param>
	/// <param Name="MapSize">The Height and width of the map</param>
	/// <param Name="Current">The current tile</param>
	/// <param Name="End">The destination tile</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	static void GetTilePath(TArray<TTile*>* Path, TTile*** Tiles, int MapSize, TTile* Current, TTile* End, bool Cardinals);

	/// <summary>
	/// Get the Path between two tiles, used for recursive calls, without tile movement weights
	/// </summary>
	/// <param Name="Path">The returning path</param>
	/// <param Name="Tiles">The map of tiles</param>
	/// <param Name="MapSize">The Height and width of the map</param>
	/// <param Name="Current">The current tile</param>
	/// <param Name="End">The destination tile</param>
	/// <param Name="Cardinals">Whether to use diagonals or not</param>
	static void GetTilePath_NoWeight(TArray<TTile*>* Path, TTile*** Tiles, int MapSize, TTile* Current, TTile* End, bool Cardinals);
};

/// <summary>
/// Async task used for pathfinding
/// </summary>
class FFindPathTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FFindPathTask>;

public:
	FFindPathTask(AEntity* Actor, FVector End, bool Cardinals)
	{
		this->Actor = Actor;
		this->End = End;
		this->Cardinals = Cardinals;
	}

protected:
	/// <summary>
	/// The actor to move
	/// </summary>
	AEntity* Actor;
	
	/// <summary>
	/// The destination
	/// </summary>
	FVector End;
	
	/// <summary>
	/// Whether to only move up, down, left, and right
	/// </summary>
	bool Cardinals;

	/// <summary>
	/// Do the async pathfinding work
	/// </summary>
	void DoWork()
	{
		//Get the starting and ending tiles
		TTile* StartingTile = GameEncapsulator::GetGame()->Map->GetTileAtLocation(Actor->GetActorLocation());
		TTile* EndingTile = GameEncapsulator::GetGame()->Map->GetTileAtLocation(End);

		//The path to return
		TArray<TTile*> Path;

		//Starting time
		double time = FPlatformTime::Seconds();

		//Find a path if both tiles are on the same road path
		if (GameEncapsulator::GetGame()->Map->PathHandler.TilesPartOfSamePath(StartingTile, EndingTile))
		{
			Path = GameEncapsulator::GetGame()->Map->PathHandler.GetPath(StartingTile, EndingTile);

			//Add path to movement component
			if (Actor != nullptr && Path.Num() > 0)
			{
				Actor->MovementComponent->AddPoints(Path);
				Actor->MovementComponent->DestroyOnEnd(true);
			}
		}
		//Find weighted path
		else
		{
			UTileMovementFunctions::GetTilePath(&Path, GameEncapsulator::GetGame()->Map->Tiles, GameEncapsulator::GetGame()->Map->MapSize, StartingTile, EndingTile, Cardinals);

			//Add path to movement component 
			if (Actor != nullptr && Path.Num() > 0)
			{
				Actor->MovementComponent->AddPoints(Path);
			}
		}
	}

	/// <summary>
	/// The async state id
	/// </summary>
	/// <returns></returns>
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FFindPathTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};