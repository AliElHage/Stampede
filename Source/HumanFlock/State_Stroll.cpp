// Fill out your copyright notice in the Description page of Project Settings.

#include "State_Stroll.h"
#include "State_Dodge.h"
#include "MyGameInstance.h"
#include "Engine.h"

float State_Stroll::Alpha = 0.0f;

void State_Stroll::ToDodgeState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_Dodge::Instance());
}

void State_Stroll::AvoidObstacle(ABaseNPC* pNPC)
{
	TArray<bool> SpawnMap;
	FVector NewLocation;
	float TargetAlignment = 0.0f;
	float xCoordinate = pNPC->GetActorLocation().X;
	if (pNPC->GetWorld())
	{
		UMyGameInstance* GameInstance;
		GameInstance = Cast<UMyGameInstance>(pNPC->GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			SpawnMap = GameInstance->GetSpawnMap();
		}
	}

	if (SpawnMap.Num() > 0)
	{
		int index = 1;
		int AlignedCellIndex = 0;
		float temp = xCoordinate - SpawnLatitudes[0];
		while (index < SpawnLatitudes.Num())
		{
			if (xCoordinate - SpawnLatitudes[index] < temp)
			{
				temp = xCoordinate - SpawnLatitudes[index];
				AlignedCellIndex = index;
			}
			index++;
		}

		index = 0;
		while (AlignedCellIndex + index < SpawnMap.Num())
		{
			if (AlignedCellIndex + index > 0)
			{
				if (!SpawnMap[AlignedCellIndex + index])
				{
					TargetAlignment = SpawnLatitudes[AlignedCellIndex + index];
					break;
				}
			}
			if (index == 0) index++;
			else
			{
				if (index > 0) index = -index;
				else index = -index + 1;
			}
		}

	}
	NewLocation.X = FMath::Lerp(pNPC->GetActorLocation().X, TargetAlignment, pNPC->DeltaTime);
	NewLocation.Y = pNPC->GetActorLocation().Y - 250.0f * pNPC->DeltaTime;
	NewLocation.Z = pNPC->GetActorLocation().Z;

	//Alpha += pNPC->DeltaTime;

	pNPC->SetActorLocation(NewLocation);
}
