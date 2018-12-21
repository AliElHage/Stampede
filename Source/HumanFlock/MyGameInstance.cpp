// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine.h"

void UMyGameInstance::Init()
{

	BiomeBuilder = Cast<UBiomeBuilder>(BiomeBuilderBP.GetDefaultObject());
	if (BiomeBuilder)
	{
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("IS IT WORKING YET"));
		}
		//BiomeBuilder->SetActiveBiome(1);
	}
}

void UMyGameInstance::AddNPC(ABaseNPC* NPC)
{
	NPCs.Add(NPC);
}

void UMyGameInstance::RemoveNPC(ABaseNPC* NPC)
{
	NPCs.Remove(NPC);
}

void UMyGameInstance::SetSpawnMap(TArray<bool> SpawnMap)
{
	this->SpawnMap = SpawnMap;
}

TArray<bool> UMyGameInstance::GetSpawnMap()
{
	return SpawnMap;
}

void UMyGameInstance::SetSpawnCoordinates(TArray<float> SpawnCoordinates)
{
	this->SpawnCoordinates = SpawnCoordinates;
}

TArray<float> UMyGameInstance::GetSpawnCoordinates()
{
	return SpawnCoordinates;
}

void UMyGameInstance::SetBiomeIndex(int NewBiomeIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Purple, "In Game Instance " + FString::FromInt(NewBiomeIndex));
	currentBiomeIndex = NewBiomeIndex;
}
