// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameEncapsulator.h"
#include "Async/AsyncWork.h"
#include "Entity.h"
#include "WorldMap.h"
#include "TileMovementFunctions.generated.h"

class DWELLERS_API PriorityNode
{

public:
	TTile* tile;
	float priority;

	PriorityNode* next = nullptr;

	PriorityNode::PriorityNode(TTile* t, float pri)
	{
		tile = t;
		priority = pri;
	}
};

class DWELLERS_API PriorityQueue
{

public:
	PriorityNode* Head = nullptr;

	void PriorityQueue::AddNode(PriorityNode* node)
	{
		if (node == nullptr)
			return;

		if (Head == nullptr)
		{
			Head = node;
			return;
		}

		if (Head->next == nullptr)
		{
			Head->next = node;
			return;
		}

		PriorityNode* current = Head;
		while (current->next != nullptr)
		{
			if (node->priority < current->next->priority)
			{
				node->next = current->next;
				current->next = node;
				return;
			}
			else
				current = current->next;
		}
		current->next = node;
	}

	TTile* PriorityQueue::Pluck()
	{
		if (Head == nullptr)
			return nullptr;

		TTile* a = Head->tile;

		if (Head->next == nullptr)
			Head = nullptr;
		else
			Head = Head->next;

		return a;
	}

	bool PriorityQueue::IsEmpty()
	{
		return Head == nullptr;
	}
};



/**
 * 
 */
UCLASS()
class DWELLERS_API UTileMovementFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, Category = TileMovement)
	static TArray<FVector> GetTilePathFromPoints(FVector start, FVector end, bool cardinals);
	UFUNCTION(BlueprintCallable, Category = TileMovement)
	static void AsyncMoveActorTo(AEntity* target, FVector end, bool cardinals);

	static TArray<TTile*> GetTilePath(TTile* start, TTile* end, bool cardinals);
	static TArray<TTile*> GetTilePath_NoWeight(TTile* start, TTile* end, bool cardinals);

	static void GetTilePath(TArray<TTile*>* path, TTile*** tiles, int mapsize, TTile* current, TTile* end, bool cardinals);
	static void GetTilePath_NoWeight(TArray<TTile*>* path, TTile*** tiles, int mapsize, TTile* current, TTile* end, bool cardinals);

};

class FFindPathTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FFindPathTask>;

public:
	FFindPathTask(AEntity* actor, FVector end, bool cardinals)
	{
		this->actor = actor;
		this->end = end;
		this->cardinals = cardinals;
	}

protected:
	AEntity* actor;
	FVector end;
	bool cardinals;

	void DoWork()
	{
		TTile* st = GameEncapsulator::GetGame()->map->GetTileAtLocation(actor->GetActorLocation());
		TTile* ed = GameEncapsulator::GetGame()->map->GetTileAtLocation(end);

		TArray<TTile*> path;

		double time = FPlatformTime::Seconds();

		if (GameEncapsulator::GetGame()->map->PathHandler.TilesPartOfSamePath(st, ed))
		{
			path = GameEncapsulator::GetGame()->map->PathHandler.GetPath(st, ed);

			actor->MovementComponent->AddPoints(path);
			actor->MovementComponent->DestroyOnEnd(true);
		}
		else
		{
			UTileMovementFunctions::GetTilePath(&path, GameEncapsulator::GetGame()->map->tiles, GameEncapsulator::GetGame()->map->mapsize, st, ed, cardinals);

			if (actor != nullptr && path.Num() > 0)
			{
				/*actor->MovementComponent->ResetControlPoints();
				actor->MovementComponent->AddControlPointPosition(actor->GetActorLocation(), false);

				float totalcost = 0;
				for (int x = 0; x < path.Num(); x++)
				{
					totalcost += path[x]->GetCost();
					actor->MovementComponent->AddControlPointPosition(
						FVector(
							path[x]->location->x * GameEncapsulator::GetGame()->map->cellsize,
							path[x]->location->y * GameEncapsulator::GetGame()->map->cellsize,
							path[x]->height * (GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight)),
						false);
				}

				actor->MovementComponent->Duration = FMath::RandRange(.75f, 1.25f) * (totalcost / path.Num()) * 25;
				actor->MovementComponent->FinaliseControlPoints();
				actor->MovementComponent->RestartMovement(1);*/

				actor->MovementComponent->AddPoints(path);
			}
		}
	}

	// This next section of code needs to be here.  Not important as to why.

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FFindPathTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};