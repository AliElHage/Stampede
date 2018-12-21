// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "BiomeBuilder.generated.h"


USTRUCT(BlueprintType)
struct FAssetContainer
{
	GENERATED_USTRUCT_BODY();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Asset")
	//TArray<AActor*> ObstacleAssets;
	TArray<TSubclassOf<AActor>> ObstacleAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Spawn Percentage")
	TArray<float> SpawnPercentages;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Asset")
	TSubclassOf<AActor> ObstacleAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Spawn Percentage")
	float SpawnPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Spawn Transform")
	FTransform SpawnTransform;
};

USTRUCT(BlueprintType)
struct FObstaclesContainer
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacles Of Length X")
	TArray<FAssetContainer> ObstaclesOfLength;

};

USTRUCT(BlueprintType)
struct FAllBiomeObstaclesContainer
{
	GENERATED_USTRUCT_BODY();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowy Beach Biome")
		TArray<FAssetContainer> SnowyBeachObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deep Forest Biome")
		TArray<FAssetContainer> DeepForestObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Biome")
		TArray<FAssetContainer> VillageObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing Camp Biome")
		TArray<FAssetContainer> FishingCampObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barbarian Hideout Biome")
		TArray<FAssetContainer> BarbarianHideoutObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Graveyard Biome")
		TArray<FAssetContainer> ShipGraveyardObstacles;*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Obstacles")
	//TArray<FAssetContainer> BiomeObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Obstacles By Length")
	TArray<FObstaclesContainer> BiomeObstaclesByLength;

};

USTRUCT(BlueprintType)
struct FBiomeContainer
{
	GENERATED_USTRUCT_BODY();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowy Beach Biome")
	TArray<FAssetContainer> SnowyBeachObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deep Forest Biome")
	TArray<FAssetContainer> DeepForestObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Biome")
	TArray<FAssetContainer> VillageObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing Camp Biome")
	TArray<FAssetContainer> FishingCampObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barbarian Hideout Biome")
	TArray<FAssetContainer> BarbarianHideoutObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Graveyard Biome")
	TArray<FAssetContainer> ShipGraveyardObstacles;*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Obstacles")
	//TArray<FAssetContainer> BiomeObstacles;

	// The name of the biome, inputtable for clarity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Name")
	FString BiomeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Asset")
	FAssetContainer GroundAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Environment Asset")
	TArray<FAssetContainer> TopEnvironmentAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bottom Environment Asset")
	TArray<FAssetContainer> BottomEnvironmentAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Obstacles By Length")
	TArray<FObstaclesContainer> BiomeObstaclesByLength;

};

/**
 * 
 */
UCLASS(Blueprintable)
class HUMANFLOCK_API UBiomeBuilder : public UObject
{
	GENERATED_BODY()
	
private:

	// The current active biome must be stored to quickly transition between environments
	TArray<FAssetContainer> ActiveBiomeObstacles;

	// Index of the active biome
	int ActiveBiomeIndex = 0;

	// Index of the previous biome
	int PreviousBiomeIndex = 0;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "All Environments")
	TArray<FBiomeContainer> AllBiomes;

	//UBiomeBuilder(/*const FObjectInitializer& ObjectInitializer*/);

	// These arrays store the blueprints to be spawned according to the biome. The first array contains all
	// single-length obstacles, the second, double-length and so on.

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowy Beach Biome")
	TArray<FAssetContainer> SnowyBeachObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deep Forest Biome")
	TArray<FAssetContainer> DeepForestObstacles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Biome")
	TArray<FAssetContainer> VillageObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing Camp Biome")
	TArray<FAssetContainer> FishingCampObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barbarian Hideout Biome")
	TArray<FAssetContainer> BarbarianHideoutObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Graveyard Biome")
	TArray<FAssetContainer> ShipGraveyardObstacles;*/


	// These arrays store the spawn percentages of each asset for each biome. Float value *must* be
	// ~~ BETWEEN 0 AND 1 ~~
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowy Beach Biome")
	TArray<FPercentageContainer> SnowyBeachPercentages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deep Forest Biome")
	TArray<FPercentageContainer> DeepForestPercentages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Biome")
	TArray<FPercentageContainer> VillagePercentages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fishing Camp Biome")
	TArray<FPercentageContainer> FishingCampPercentages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barbarian Hideout Biome")
	TArray<FPercentageContainer> BarbarianHideoutPercentages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Graveyard Biome")
	TArray<FPercentageContainer> ShipGraveyardPercentages;*/
	
	// Choose asset among those of the same length in the active biome depending on random draw
	UFUNCTION()
	TSubclassOf<AActor> ChooseAssetToSpawn(int length, float ChoiceValue);

	UFUNCTION(BlueprintCallable, Category = "Active Biome")
	TArray<FAssetContainer> GetActiveBiomeObstacles() { return ActiveBiomeObstacles; }

	UFUNCTION(BlueprintCallable, Category = "Active Biome")
	void SetActiveBiome(int BiomeIndex);

	UFUNCTION(BlueprintCallable, Category = "Active Biome")
	int GetActiveBiomeIndex() { return ActiveBiomeIndex; }

	UFUNCTION(BlueprintCallable, Category = "Active Biome")
	int GetPreviousBiomeIndex() { return PreviousBiomeIndex; }

	UFUNCTION(BlueprintCallable, Category = "Previous Biome")
	void SetPreviousBiomeIndex(int BiomeIndex) { PreviousBiomeIndex = BiomeIndex; }

	UFUNCTION()
	void BiomeArrayInit();

};
