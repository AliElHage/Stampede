// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseNPC.h"
#include "BiomeBuilder.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HUMANFLOCK_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	TArray<ABaseNPC*> NPCs;

	TArray<bool> SpawnMap;

	TArray<float> SpawnCoordinates;

	UBiomeBuilder* BiomeBuilder;

public:

	virtual void Init() override;

	void SetSpawnMap(TArray<bool> SpawnMap);

	TArray<bool> GetSpawnMap();

	void SetSpawnCoordinates(TArray<float> SpawnCoordinates);

	TArray<float> GetSpawnCoordinates();
	
	void AddNPC(ABaseNPC* NPC);

	void SetBiomeIndex(int NewBiomeIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaderboard Info")
	int score = 5; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaderboard Info")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Builder")
	int currentBiomeIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Builder")
	TSubclassOf<UBiomeBuilder> BiomeBuilderBP;

	UFUNCTION(BlueprintCallable, Category = "NPC Management")
	void RemoveNPC(ABaseNPC* NPC);

	UFUNCTION(BlueprintCallable, Category = "Biome Builder")
	UBiomeBuilder* GetBiomeBuilder() { return BiomeBuilder; }

	TArray<ABaseNPC*> GetNPCs() { return NPCs; }
	
	UFUNCTION(BlueprintCallable, Category = "NPC Management")
	void ClearNPCs() { NPCs.Empty(); }
};
