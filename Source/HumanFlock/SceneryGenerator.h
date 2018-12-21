// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGameInstance.h"
#include "BiomeBuilder.h"
#include "TimerManager.h"
#include "SceneryGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeBiomeSignature, int, NewBiomeIndex);

UCLASS()
class HUMANFLOCK_API ASceneryGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneryGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Biome Regulation")
	void SetDeferredBiome();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void GenerateGround();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void GenerateTopEnvironment(int NewBiomeIndex);

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void GenerateBottomEnvironment();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void GenerateWater();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void GenerateForestEnvironment(bool bIsSnowy, bool bIsMagnified);

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void SB_TopGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void SB_BotGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void DF_TopGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void DF_TopGen2();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void DF_BotGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void DF_BotGen2();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void Vi_TopGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void Vi_BotGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void Vi_FenceGen();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void FC_TopGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void FC_BotGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void FC_GroundGen1();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void FC_GroundGen2();

	UFUNCTION(BlueprintCallable, Category = "Generate Environment")
	void FC_SeaGroundGen();

protected:

	// Pointer to the game instance
	UMyGameInstance* GameInstance;

	// Pointer to the biome builder class, which holds all the assets
	UBiomeBuilder* BiomeBuilder;

	// Handles to manage the timers
	FTimerHandle DefferedNewBiomeTimerHandle;
	FTimerHandle GenerateGroundTimerHandle;
	FTimerHandle GenerateWaterTimerHandle;
	FTimerHandle GenerateForestTimerHandle;
	FTimerHandle GenerateBeachTimerHandle;
	FTimerHandle GenerateDeepForestTimerHandle;
	FTimerHandle SB_TopTimerHandle1;
	FTimerHandle DF_TopTimerHandle1;
	FTimerHandle DF_TopTimerHandle2;
	FTimerHandle DF_BotTimerHandle1;
	FTimerHandle DF_BotTimerHandle2;
	FTimerHandle Vi_TopTimerHandle1;
	FTimerHandle Vi_BotTimerHandle1;
	FTimerHandle Vi_FenceTimerHandle1;
	FTimerHandle Vi_FenceTimerHandle2;
	FTimerHandle FC_TopTimerHandle1;
	FTimerHandle FC_BotTimerHandle1;
	FTimerHandle FC_GroundTimerHandle1;
	FTimerHandle FC_GroundTimerHandle2;
	FTimerHandle FC_SeaGroundTimerHandle;

	// Delegates for timed functions with parameters
	FTimerDelegate SnowyBeachForestDelegate;
	FTimerDelegate DeepForestDelegate;
	FTimerDelegate DF_Bot2Delegate;

	FChangeBiomeSignature ChangeBiomeSignature_OnChange;

	// Ground assets array
	TArray<AActor*> GroundAssets;

	// Water assets array
	TArray<AActor*> WaterAssets;

	// Beach assets array
	TArray<AActor*> BeachCoastAssets;

	TArray<AActor*> DL_Bot1Assets;
	TArray<AActor*> DF_Top1Assets;
	TArray<AActor*> Vi_Top1Assets;
	TArray<AActor*> Vi_Bot1Assets;
	TArray<AActor*> FC_Top1Assets;
	TArray<AActor*> FC_Bot1Assets;
	TArray<AActor*> FC_Ground1Assets;
	TArray<AActor*> FC_SeaGroundAssets;

	////// Indices to use for smooth transition to other biome grounds //////
	// Stores the previous biome index temporarily (duration: GroundBufferCycles cycles)
	int GroundBufferedBiomeIndex = 0;
	int BeachBufferedBiomeIndex = 0;
	int DF_Bot1BufferedBiomeIndex = 0;
	int DF_Bot2BufferedBiomeIndex = 0;
	int DF_Top1BufferedBiomeIndex = 0;
	int DF_Top2BufferedBiomeIndex = 0;
	int Vi_Top1BufferedBiomeIndex = 0;
	int Vi_Bot1BufferedBiomeIndex = 0;
	int FC_Top1BufferedBiomeIndex = 0;
	int FC_Bot1BufferedBiomeIndex = 0;
	int FC_Ground1BufferedBiomeIndex = 0;
	int FC_SeaGroundBufferedBiomeIndex = 0;

	// Number of cycles before GroundBufferedBiomeIndex gets updated to current biome
	int GroundBufferCycles = 3;
	int BeachBufferCycles = 0;
	int DF_Bot1BufferCycles = 0;
	int DF_Bot2BufferCycles = 0;
	int DF_Top1BufferCycles = 0;
	int Vi_Top1BufferCycles = 0;
	int Vi_Bot1BufferCycles = 0;
	int FC_Top1BufferCycles = 0;
	int FC_Bot1BufferCycles = 0;
	int FC_Ground1BufferCycles = 0;

	// Number of calls made to the forest generation function
	int ForestGenCallCount = 0;
	int DF_Top2CallCount = 0;
	int DF_Bot2CallCount = 0;
	int Vi_Top1CallCount = 0;
	int Vi_Bot1CallCount = 0;
	int FC_Top1CallCount = 0;
	int FC_Bot1CallCount = 0;
	int FC_Ground1CallCount = 0;

	int ForestBufferedBiomeIndex = 0;
	/////////////////////////////////////////////////////////////////////////

	// Number of cycles elapsed since in current biome.
	int CurrentGroundCycleCount = 0;
	int CurrentSnowyBeachCycleCount = 0;
	int DF_Top1CycleCount = 0;
	int DF_Bot1CycleCount = 0;
	int DF_Bot2CycleCount = 0;
	int Vi_Top1CycleCount = 0;
	int Vi_Bot1CycleCount = 0;
	int FC_Top1CycleCount = 0;
	int FC_Bot1CycleCount = 0;
	int FC_Ground1CycleCount = 0;
	int FC_SeaGroundCycleCount = 0;

	// prevent village walkway from spawning successively
	bool bVi_TopWalkwayHasSpawned = false;
	bool bVi_BotWalkwayHasSpawned = false;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float GroundGenRate = 4.27f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int GroundGenCycles = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float WaterGenRate = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float ForestGenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int ForestGenCycles = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float SnowyBeachGenRate = 3.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int SnowyBeachGenCycles = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float DL_Bot1GenRate = 0.9333f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int DL_Bot1GenCycles = 31;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float DF_Bot2GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int DF_Bot2GenCycles = 28;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float DF_Top1GenRate = 0.9333f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int DF_Top1GenCycles = 31;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float DF_Top2GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int DF_Top2GenCycles = 28;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float Vi_Top1GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int Vi_Top1GenCycles = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float Vi_Bot1GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int Vi_Bot1GenCycles = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float FC_Top1GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int FC_Top1GenCycles = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float FC_Bot1GenRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int FC_Bot1GenCycles = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	float FC_GroundGenRate = 1.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
	int FC_GroundGenCycles = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
		float FC_SeaGroundGenRate = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenery Generation Rates")
		int FC_SeaGroundGenCycles = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forest Generation Parameters")
	int TreesPerSpawnArea = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forest Generation Parameters")
	float SpawnAreaWidth = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forest Generation Parameters")
	float SpawnAreaLength = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forest Generation Parameters")
	float TreesMinDistance = 200.0f;

};
