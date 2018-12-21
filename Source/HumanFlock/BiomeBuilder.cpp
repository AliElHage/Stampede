// Fill out your copyright notice in the Description page of Project Settings.

#include "BiomeBuilder.h"
#include "Engine.h"


/*UBiomeBuilder::UBiomeBuilder()
{
	Super::UObject();
	FAssetContainer Container;
	Container.ObstacleAssets.Init(NULL, 3);
	Container.SpawnPercentages.Init(0.0f, 3);
	ActiveBiomeObstacles.Init(Container, 3);
}*/

void UBiomeBuilder::BiomeArrayInit()
{
	
}

TSubclassOf<AActor> UBiomeBuilder::ChooseAssetToSpawn(int length, float ChoiceValue)
{
	TSubclassOf<AActor> ReturnedAsset;
	float Comparator = 0.0f;
	//BiomeArrayInit();
	//ActiveBiomeObstacles = SnowyBeachObstacles;
	float Denominator = 0.0f;
	if (length < AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength.Num())
	{
		for (int i = 0; i < AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength[length - 1].ObstaclesOfLength.Num(); i++)
		{
			Denominator += AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength[length - 1].ObstaclesOfLength[i].SpawnPercentage;
		}
		for (int i = 0; i < AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength[length - 1].ObstaclesOfLength.Num(); i++)
		{
			//Comparator += ActiveBiomeObstacles[length].SpawnPercentages[i];
			Comparator += AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength[length - 1].ObstaclesOfLength[i].SpawnPercentage;
			if (ChoiceValue < Comparator / Denominator)
			{
				//ReturnedAsset = ActiveBiomeObstacles[length].ObstacleAssets[i];
				ReturnedAsset = AllBiomes[PreviousBiomeIndex].BiomeObstaclesByLength[length - 1].ObstaclesOfLength[i].ObstacleAsset;
				break;
			}
		}
	}
	return ReturnedAsset;
}

void UBiomeBuilder::SetActiveBiome(int BiomeIndex)
{
	ActiveBiomeIndex = BiomeIndex;
}


