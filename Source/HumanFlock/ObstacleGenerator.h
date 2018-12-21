// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstance.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "ObstacleGenerator.generated.h"

UCLASS()
class HUMANFLOCK_API AObstacleGenerator : public AActor
{
	GENERATED_BODY()
	
private:

	TArray<bool> SpawnMap;

	FTimerHandle SpawnTimer;

	FTimerHandle NPCSpawnTimer;

	FTimerHandle DifficultyScalingTimer;

	FTimerHandle ScoringTimer;

	UMyGameInstance* GameInstance;

	void SpawnObstacles();

	void SpawnNPCs();

	bool IsGenerated();

	void GenerateSpawnMap();

	void ScaleDifficulty();

	void UpdateScore();

	TArray<TArray<int32>> CleanSpawnMap();

	int FlockSize = 0;

public:	
	// Sets default values for this actor's properties
	AObstacleGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float SpawnLongitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float DestroyLatitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	TArray<float> SpawnLatitudes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float ObstacleSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float SpawnInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float InitialSpawnInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float DifficultyInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float LowestSpawnInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float SpawnIncrements;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	float ProgressRatio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Settings")
	int SpawnableNumber;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float Score = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float ScoringInterval = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float BaseScoreUnits = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float FlockSizeMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float FlockSizeMultiplierIncrement = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float DifficultyMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score Variables")
	float DifficultyMultiplierIncrement = 0.5f;

	float Counter = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obstacle Spawning")
	TSubclassOf<AActor> ObstacleBP;

	UPROPERTY(EditDefaultsOnly, Category = "Obstacle Spawning")
	TSubclassOf<AActor> DoubleObstacleBP;

	UPROPERTY(EditDefaultsOnly, Category = "Obstacle Spawning")
	TSubclassOf<AActor> TripleObstacleBP;

	UPROPERTY(EditDefaultsOnly, Category = "Obstacle Spawning")
	TSubclassOf<AActor> QuadrupleObstacleBP;

	UPROPERTY(EditDefaultsOnly, Category = "Humans Spawning")
	TSubclassOf<ACharacter> HumansBP;

};
