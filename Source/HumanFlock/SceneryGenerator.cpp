// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneryGenerator.h"
#include "Engine.h"
#include "Engine/World.h"
#include "GenericPlatformMath.h"


// Sets default values
ASceneryGenerator::ASceneryGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASceneryGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		// Find pointer to the biome builder instance and set the active biome for the beginning of the game.
		BiomeBuilder = Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->GetBiomeBuilder();
		BiomeBuilder->SetActiveBiome(0);
		BiomeBuilder->SetPreviousBiomeIndex(0);

		// Set GroundBufferedBiomeIndex to the current index.
		GroundBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
		BeachBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();

		//Binding the function with specific values
		SnowyBeachForestDelegate.BindUFunction(this, FName("GenerateForestEnvironment"), true, false);
		DeepForestDelegate.BindUFunction(this, FName("GenerateForestEnvironment"), true, true);
		DF_Bot2Delegate.BindUFunction(this, FName("GenerateForestEnvironment"), true, true);

		// Set the timers that will generate the environment.
		GetWorld()->GetTimerManager().SetTimer(GenerateGroundTimerHandle, this, &ASceneryGenerator::GenerateGround, GroundGenRate, true, 0.0f);
		GetWorld()->GetTimerManager().SetTimer(GenerateWaterTimerHandle, this, &ASceneryGenerator::GenerateWater, WaterGenRate, true, 0.0f);
		GetWorld()->GetTimerManager().SetTimer(GenerateBeachTimerHandle, this, &ASceneryGenerator::SB_BotGen1, SnowyBeachGenRate, true, 1.8f);
		GetWorld()->GetTimerManager().SetTimer(SB_TopTimerHandle1, this, &ASceneryGenerator::SB_TopGen1, ForestGenRate, true, 4.0f);
	}

	ChangeBiomeSignature_OnChange.AddDynamic(this, &ASceneryGenerator::GenerateTopEnvironment);

	// Seed the PRNG for random calls
	FGenericPlatformMath::SRandInit(GetUniqueID());
	
}

// Called every frame
void ASceneryGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASceneryGenerator::SetDeferredBiome()
{
	BiomeBuilder->SetPreviousBiomeIndex(BiomeBuilder->GetActiveBiomeIndex());
}

void ASceneryGenerator::GenerateGround()
{

	// Increment the number of cycles elapsed each time this function is called.
	CurrentGroundCycleCount++;

	// Get ground asset from biome builder class
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[BiomeBuilder->GetActiveBiomeIndex()].GroundAsset.ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[BiomeBuilder->GetActiveBiomeIndex()].GroundAsset.SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (GroundAssets.Num() < 3)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			GroundAssets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = GroundAssets[0];
		GroundAssets.RemoveAt(0);

		// If the biome has changed, replace the current asset with a new one. Count the cycles until
		// GroundBufferedBiomeIndex gets updated.
		if (GroundBufferedBiomeIndex != BiomeBuilder->GetActiveBiomeIndex() && GetWorld())
		{
			TempAsset = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			GroundBufferCycles++;
		}
		// Otherwise, move the asset back as expected.
		else
		{
			TempAsset->SetActorTransform(Transform);
		}
		GroundAssets.Insert(TempAsset, 2);

		// If all assets have been changed to those of the new biome, update GroundBufferedBiomeIndex to
		// that of the current biome.
		if (GroundBufferCycles == 3)
		{
			GroundBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
			GroundBufferCycles = 0;
		}
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (CurrentGroundCycleCount == GroundGenCycles)
	{
		CurrentGroundCycleCount = 0;
		if (BiomeBuilder->GetActiveBiomeIndex() == 0)
		{
			BiomeBuilder->SetActiveBiome(1);
			ChangeBiomeSignature_OnChange.Broadcast(1);
		}
		else if(BiomeBuilder->GetActiveBiomeIndex() == 1)
		{
			BiomeBuilder->SetActiveBiome(2);
			ChangeBiomeSignature_OnChange.Broadcast(2);
		}
		else if (BiomeBuilder->GetActiveBiomeIndex() == 2)
		{
			BiomeBuilder->SetActiveBiome(3);
			ChangeBiomeSignature_OnChange.Broadcast(3);
			GetWorld()->GetTimerManager().ClearTimer(GenerateGroundTimerHandle);
		}
		
		GetWorld()->GetTimerManager().SetTimer(DefferedNewBiomeTimerHandle, this, &ASceneryGenerator::SetDeferredBiome, ForestGenRate, false, 9.0f);
	}

}

void ASceneryGenerator::GenerateTopEnvironment(int NewBiomeIndex)
{
	Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->SetBiomeIndex(NewBiomeIndex);

	switch (NewBiomeIndex)
	{
	case 0:
		BeachCoastAssets.Empty();
		GetWorld()->GetTimerManager().SetTimer(GenerateGroundTimerHandle, this, &ASceneryGenerator::GenerateGround, GroundGenRate, true, 0.0f);
		GetWorld()->GetTimerManager().SetTimer(GenerateBeachTimerHandle, this, &ASceneryGenerator::SB_BotGen1, SnowyBeachGenRate, true, 1.8f);
		GetWorld()->GetTimerManager().SetTimer(SB_TopTimerHandle1, this, &ASceneryGenerator::SB_TopGen1, ForestGenRate, true, 4.0f);
		break;
	case 1:
		if (GetWorld())
		{
			DF_Top1Assets.Empty();
			DL_Bot1Assets.Empty();
			//GetWorld()->GetTimerManager().SetTimer(GenerateDeepForestTimerHandle, DeepForestDelegate, ForestGenRate, true, 9.4f);
			GetWorld()->GetTimerManager().SetTimer(DF_BotTimerHandle1, this, &ASceneryGenerator::DF_BotGen1, DL_Bot1GenRate, true, 4.8f);
			GetWorld()->GetTimerManager().SetTimer(DF_BotTimerHandle2, this, &ASceneryGenerator::DF_BotGen2, ForestGenRate, true, 8.4f);
			//GetWorld()->GetTimerManager().SetTimer(DF_BotTimerHandle2, DF_Bot2Delegate, ForestGenRate, true, 9.4f);
			GetWorld()->GetTimerManager().SetTimer(DF_TopTimerHandle1, this, &ASceneryGenerator::DF_TopGen1, DF_Top1GenRate, true, 4.8f);
			GetWorld()->GetTimerManager().SetTimer(DF_TopTimerHandle2, this, &ASceneryGenerator::DF_TopGen2, ForestGenRate, true, 8.4f);
			DF_Bot1BufferedBiomeIndex = NewBiomeIndex;
			DF_Top1BufferedBiomeIndex = NewBiomeIndex;
			DF_Top2BufferedBiomeIndex = NewBiomeIndex;
		}
		break;
	case 2:
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(Vi_TopTimerHandle1, this, &ASceneryGenerator::Vi_TopGen1, Vi_Top1GenRate, true, 7.0f);
			GetWorld()->GetTimerManager().SetTimer(Vi_BotTimerHandle1, this, &ASceneryGenerator::Vi_BotGen1, Vi_Bot1GenRate, true, 7.0f);
			GetWorld()->GetTimerManager().SetTimer(Vi_FenceTimerHandle1, this, &ASceneryGenerator::Vi_FenceGen, 1.0f, false, 7.4f);
			GetWorld()->GetTimerManager().SetTimer(Vi_FenceTimerHandle2, this, &ASceneryGenerator::Vi_FenceGen, 1.0f, false, 37.6f);
			Vi_Top1BufferedBiomeIndex = NewBiomeIndex;
			Vi_Bot1BufferedBiomeIndex = NewBiomeIndex;
		}
		break;
	case 3:
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(FC_GroundTimerHandle1, this, &ASceneryGenerator::FC_GroundGen1, FC_GroundGenRate, true, 7.0f);
			GetWorld()->GetTimerManager().SetTimer(FC_GroundTimerHandle2, this, &ASceneryGenerator::FC_GroundGen2, 1.0f, false, 6.0f);
			GetWorld()->GetTimerManager().SetTimer(FC_SeaGroundTimerHandle, this, &ASceneryGenerator::FC_SeaGroundGen, FC_SeaGroundGenRate, true, 5.5f);
			FC_Ground1BufferedBiomeIndex = NewBiomeIndex;
			FC_SeaGroundBufferedBiomeIndex = NewBiomeIndex;
		}
	default:
		break;
	}

	
	

}

void ASceneryGenerator::GenerateBottomEnvironment()
{

}

void ASceneryGenerator::GenerateWater()
{

	// Get ground asset from biome builder class
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[0].TopEnvironmentAssets[1].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[0].TopEnvironmentAssets[1].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (WaterAssets.Num() < 2)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			WaterAssets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = WaterAssets[0];
		WaterAssets.RemoveAt(0);

		// Move the asset back as expected.
		TempAsset->SetActorTransform(Transform);
		WaterAssets.Insert(TempAsset, 1);
	}
}

void ASceneryGenerator::GenerateForestEnvironment(bool bIsSnowy, bool bIsMagnified)
{

	if (ForestGenCallCount == 0)
	{
		ForestBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
	}
	ForestGenCallCount++;
	TArray<AActor*> GeneratedTrees;

	for (int i = 0; i < TreesPerSpawnArea; i++)
	{
		float XOffset = BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().X;
		float YOffset = BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().Y;
		float XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
		float YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
		FVector SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
		FTransform Transform;
		FActorSpawnParameters SpawnParams;
		if (GeneratedTrees.Num() == 0)
		{
			Transform.SetTranslation(SpawnPosition);
			AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
			GeneratedTrees.Emplace(SpawnedTree);
		}
		else
		{
			int index = 0;
			int NumberIterations = 0;
			bool bHasReachedMaxIterations = false;
			while (index < GeneratedTrees.Num())
			{
				if (NumberIterations == 10)
				{
					bHasReachedMaxIterations = true;
					break;
				}
				if (FMath::Sqrt(FMath::Pow(XCoordinate - GeneratedTrees[index]->GetActorLocation().X, 2) + FMath::Pow(YCoordinate - GeneratedTrees[index]->GetActorLocation().Y, 2)) < TreesMinDistance)
				{
					XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
					YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
					SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
					index = 0;
					NumberIterations++;
				}
				else
				{
					index++;
				}
			}
			if (!bHasReachedMaxIterations)
			{
				Transform.SetTranslation(SpawnPosition);
				AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
				GeneratedTrees.Emplace(SpawnedTree);
			}
		}
	}
	if (ForestGenCallCount == ForestGenCycles && GetWorld())
	{
		ForestGenCallCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(GenerateForestTimerHandle);
	}
}

void ASceneryGenerator::SB_TopGen1()
{
	if (ForestGenCallCount == 0)
	{
		ForestBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
	}
	ForestGenCallCount++;
	TArray<AActor*> GeneratedTrees;

	for (int i = 0; i < TreesPerSpawnArea; i++)
	{
		float XOffset = BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().X;
		float YOffset = BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().Y;
		float XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
		float YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
		FVector SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
		FTransform Transform;
		FActorSpawnParameters SpawnParams;
		if (GeneratedTrees.Num() == 0)
		{
			Transform.SetTranslation(SpawnPosition);
			Transform.SetScale3D(BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetScale3D());
			AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
			GeneratedTrees.Emplace(SpawnedTree);
		}
		else
		{
			int index = 0;
			int NumberIterations = 0;
			bool bHasReachedMaxIterations = false;
			while (index < GeneratedTrees.Num())
			{
				if (NumberIterations == 10)
				{
					bHasReachedMaxIterations = true;
					break;
				}
				if (FMath::Sqrt(FMath::Pow(XCoordinate - GeneratedTrees[index]->GetActorLocation().X, 2) + FMath::Pow(YCoordinate - GeneratedTrees[index]->GetActorLocation().Y, 2)) < TreesMinDistance)
				{
					XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
					YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
					SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
					index = 0;
					NumberIterations++;
				}
				else
				{
					index++;
				}
			}
			if (!bHasReachedMaxIterations)
			{
				Transform.SetTranslation(SpawnPosition);
				AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[ForestBufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
				GeneratedTrees.Emplace(SpawnedTree);
			}
		}
	}
	if (ForestGenCallCount == ForestGenCycles && GetWorld())
	{
		ForestGenCallCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(SB_TopTimerHandle1);
	}
}

void ASceneryGenerator::SB_BotGen1()
{
	// Increment the number of cycles elapsed each time this function is called.
	CurrentSnowyBeachCycleCount++;

	// Get ground asset from biome builder class
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[BeachBufferedBiomeIndex].BottomEnvironmentAssets[0].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[BeachBufferedBiomeIndex].BottomEnvironmentAssets[0].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (BeachCoastAssets.Num() < 4)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			BeachCoastAssets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = BeachCoastAssets[0];
		BeachCoastAssets.RemoveAt(0);

		// If the biome has changed, replace the current asset with a new one. Count the cycles until
		// GroundBufferedBiomeIndex gets updated.
		if (BeachBufferedBiomeIndex != BiomeBuilder->GetActiveBiomeIndex() && GetWorld())
		{
			TempAsset = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			BeachBufferCycles++;
		}
		// Otherwise, move the asset back as expected.
		else
		{
			TempAsset->SetActorTransform(Transform);
		}
		BeachCoastAssets.Insert(TempAsset, 3);

		// If all assets have been changed to those of the new biome, update GroundBufferedBiomeIndex to
		// that of the current biome.
		if (BeachBufferCycles == 4)
		{
			BeachBufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
			BeachBufferCycles = 0;
		}
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (CurrentSnowyBeachCycleCount == SnowyBeachGenCycles && GetWorld())
	{
		CurrentSnowyBeachCycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(GenerateBeachTimerHandle);
	}

}

void ASceneryGenerator::DF_TopGen1()
{
	DF_Top1CycleCount++;

	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[DF_Top1BufferedBiomeIndex].TopEnvironmentAssets[1].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[DF_Top1BufferedBiomeIndex].TopEnvironmentAssets[1].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (DF_Top1Assets.Num() < 8)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			DF_Top1Assets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = DF_Top1Assets[0];
		DF_Top1Assets.RemoveAt(0);

		TempAsset->SetActorTransform(Transform);
		DF_Top1Assets.Insert(TempAsset, 7);

	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (DF_Top1CycleCount == DF_Top1GenCycles && GetWorld())
	{
		DF_Top1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DF_TopTimerHandle1);
	}
}

void ASceneryGenerator::DF_TopGen2()
{
	if (DF_Top2CallCount == 0)
	{
		DF_Top2BufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
	}
	DF_Top2CallCount++;
	TArray<AActor*> GeneratedTrees;

	for (int i = 0; i < TreesPerSpawnArea; i++)
	{
		float XOffset = BiomeBuilder->AllBiomes[DF_Top2BufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().X;
		float YOffset = BiomeBuilder->AllBiomes[DF_Top2BufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetTranslation().Y;
		float XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
		float YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
		FVector SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
		FTransform Transform;
		FActorSpawnParameters SpawnParams;
		if (GeneratedTrees.Num() == 0)
		{
			Transform.SetTranslation(SpawnPosition);
			Transform.SetScale3D(BiomeBuilder->AllBiomes[DF_Top2BufferedBiomeIndex].TopEnvironmentAssets[0].SpawnTransform.GetScale3D());
			AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[DF_Top2BufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
			GeneratedTrees.Emplace(SpawnedTree);
		}
		else
		{
			int index = 0;
			int NumberIterations = 0;
			bool bHasReachedMaxIterations = false;
			while (index < GeneratedTrees.Num())
			{
				if (NumberIterations == 10)
				{
					bHasReachedMaxIterations = true;
					break;
				}
				if (FMath::Sqrt(FMath::Pow(XCoordinate - GeneratedTrees[index]->GetActorLocation().X, 2) + FMath::Pow(YCoordinate - GeneratedTrees[index]->GetActorLocation().Y, 2)) < TreesMinDistance)
				{
					XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
					YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
					SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
					index = 0;
					NumberIterations++;
				}
				else
				{
					index++;
				}
			}
			if (!bHasReachedMaxIterations)
			{
				Transform.SetTranslation(SpawnPosition);
				AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[DF_Top2BufferedBiomeIndex].TopEnvironmentAssets[0].ObstacleAsset, Transform, SpawnParams);
				GeneratedTrees.Emplace(SpawnedTree);
			}
		}
	}
	if (DF_Top2CallCount == DF_Top2GenCycles && GetWorld())
	{
		DF_Top2CallCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DF_TopTimerHandle2);
	}
}

void ASceneryGenerator::DF_BotGen2()
{
	if (DF_Bot2CallCount == 0)
	{
		DF_Bot2BufferedBiomeIndex = BiomeBuilder->GetActiveBiomeIndex();
	}
	DF_Bot2CallCount++;
	TArray<AActor*> GeneratedTrees;

	for (int i = 0; i < TreesPerSpawnArea; i++)
	{
		float XOffset = BiomeBuilder->AllBiomes[DF_Bot2BufferedBiomeIndex].BottomEnvironmentAssets[1].SpawnTransform.GetTranslation().X;
		float YOffset = BiomeBuilder->AllBiomes[DF_Bot2BufferedBiomeIndex].BottomEnvironmentAssets[1].SpawnTransform.GetTranslation().Y;
		float XCoordinate = FGenericPlatformMath::SRand() * (SpawnAreaWidth + 200.0f) - ((SpawnAreaWidth + 200.0f) / 2.0f) + XOffset;
		float YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
		FVector SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
		FTransform Transform;
		FActorSpawnParameters SpawnParams;
		if (GeneratedTrees.Num() == 0)
		{
			Transform.SetTranslation(SpawnPosition);
			Transform.SetScale3D(BiomeBuilder->AllBiomes[DF_Bot2BufferedBiomeIndex].BottomEnvironmentAssets[1].SpawnTransform.GetScale3D());
			AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[DF_Bot2BufferedBiomeIndex].BottomEnvironmentAssets[1].ObstacleAsset, Transform, SpawnParams);
			GeneratedTrees.Emplace(SpawnedTree);
		}
		else
		{
			int index = 0;
			int NumberIterations = 0;
			bool bHasReachedMaxIterations = false;
			while (index < GeneratedTrees.Num())
			{
				if (NumberIterations == 10)
				{
					bHasReachedMaxIterations = true;
					break;
				}
				if (FMath::Sqrt(FMath::Pow(XCoordinate - GeneratedTrees[index]->GetActorLocation().X, 2) + FMath::Pow(YCoordinate - GeneratedTrees[index]->GetActorLocation().Y, 2)) < TreesMinDistance)
				{
					XCoordinate = FGenericPlatformMath::SRand() * SpawnAreaWidth - (SpawnAreaWidth / 2.0f) + XOffset;
					YCoordinate = FGenericPlatformMath::SRand() * SpawnAreaLength - (SpawnAreaLength / 2.0f) + YOffset;
					SpawnPosition = FVector(XCoordinate, YCoordinate, 0.0f);
					index = 0;
					NumberIterations++;
				}
				else
				{
					index++;
				}
			}
			if (!bHasReachedMaxIterations)
			{
				Transform.SetTranslation(SpawnPosition);
				AActor* SpawnedTree = GetWorld()->SpawnActor<AActor>(BiomeBuilder->AllBiomes[DF_Bot2BufferedBiomeIndex].BottomEnvironmentAssets[1].ObstacleAsset, Transform, SpawnParams);
				GeneratedTrees.Emplace(SpawnedTree);
			}
		}
	}
	if (DF_Bot2CallCount == DF_Bot2GenCycles && GetWorld())
	{
		DF_Bot2CallCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DF_BotTimerHandle2);
	}
}

void ASceneryGenerator::DF_BotGen1()
{
	DF_Bot1CycleCount++;

	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[DF_Bot1BufferedBiomeIndex].BottomEnvironmentAssets[0].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[DF_Bot1BufferedBiomeIndex].BottomEnvironmentAssets[0].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (DL_Bot1Assets.Num() < 8)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			DL_Bot1Assets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = DL_Bot1Assets[0];
		DL_Bot1Assets.RemoveAt(0);

		TempAsset->SetActorTransform(Transform);
		DL_Bot1Assets.Insert(TempAsset, 7);
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (DF_Bot1CycleCount == DL_Bot1GenCycles && GetWorld())
	{
		DF_Bot1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DF_BotTimerHandle1);
	}

}

void ASceneryGenerator::Vi_TopGen1()
{
	if (Vi_Top1CycleCount == 0)
	{
		Vi_Top1Assets.Empty();
	}
	Vi_Top1CycleCount++;

	// Get the index of the asset to spawn
	int AssetIndex = 0;
	if (bVi_TopWalkwayHasSpawned)
	{
		AssetIndex = (int)FMath::FRandRange(0, BiomeBuilder->AllBiomes[Vi_Top1BufferedBiomeIndex].TopEnvironmentAssets.Num() - 2.01f);
		bVi_TopWalkwayHasSpawned = false;
	}
	else AssetIndex = (int)FMath::FRandRange(0, BiomeBuilder->AllBiomes[Vi_Top1BufferedBiomeIndex].TopEnvironmentAssets.Num() - 1.01f);

	if (AssetIndex == BiomeBuilder->AllBiomes[Vi_Top1BufferedBiomeIndex].TopEnvironmentAssets.Num() - 1)
	{
		bVi_TopWalkwayHasSpawned = true;
	}

	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[Vi_Top1BufferedBiomeIndex].TopEnvironmentAssets[AssetIndex].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[Vi_Top1BufferedBiomeIndex].TopEnvironmentAssets[AssetIndex].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (Vi_Top1Assets.Num() < 8)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			Vi_Top1Assets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = Vi_Top1Assets[0];
		Vi_Top1Assets.RemoveAt(0);

		TempAsset->SetActorTransform(Transform);
		Vi_Top1Assets.Insert(TempAsset, 7);
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (Vi_Top1CycleCount == Vi_Top1GenCycles && GetWorld())
	{
		Vi_Top1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(Vi_TopTimerHandle1);
	}
}

void ASceneryGenerator::Vi_BotGen1()
{
	if (Vi_Bot1CycleCount == 0)
	{
		Vi_Bot1Assets.Empty();
	}
	Vi_Bot1CycleCount++;

	// Get the index of the asset to spawn
	int AssetIndex = 0;
	if (bVi_BotWalkwayHasSpawned)
	{
		AssetIndex = (int)FMath::FRandRange(0, BiomeBuilder->AllBiomes[Vi_Bot1BufferedBiomeIndex].BottomEnvironmentAssets.Num() - 1.01f);
		bVi_BotWalkwayHasSpawned = false;
	}
	else AssetIndex = (int)FMath::FRandRange(0, BiomeBuilder->AllBiomes[Vi_Bot1BufferedBiomeIndex].BottomEnvironmentAssets.Num() - 0.01f);
	
	if (AssetIndex == BiomeBuilder->AllBiomes[Vi_Bot1BufferedBiomeIndex].BottomEnvironmentAssets.Num() - 1)
	{
		bVi_BotWalkwayHasSpawned = true;
	}

	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[Vi_Bot1BufferedBiomeIndex].BottomEnvironmentAssets[AssetIndex].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[Vi_Bot1BufferedBiomeIndex].BottomEnvironmentAssets[AssetIndex].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (Vi_Bot1Assets.Num() < 8)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			Vi_Bot1Assets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = Vi_Bot1Assets[0];
		Vi_Bot1Assets.RemoveAt(0);

		TempAsset->SetActorTransform(Transform);
		Vi_Bot1Assets.Insert(TempAsset, 7);
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (Vi_Bot1CycleCount == Vi_Bot1GenCycles && GetWorld())
	{
		Vi_Bot1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(Vi_BotTimerHandle1);
	}
}

void ASceneryGenerator::Vi_FenceGen()
{
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[2].TopEnvironmentAssets[4].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[2].TopEnvironmentAssets[4].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	AActor* SpawnedActor;
	if (GetWorld())
	{
		SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
	}
}

void ASceneryGenerator::FC_TopGen1()
{

}

void ASceneryGenerator::FC_BotGen1()
{

}

void ASceneryGenerator::FC_GroundGen1()
{
	/*if (FC_Ground1CycleCount == 0)
	{
		//FC_Ground1Assets.Empty();
	}
	FC_Ground1CycleCount++;

	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[FC_Ground1BufferedBiomeIndex].BottomEnvironmentAssets[0].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[FC_Ground1BufferedBiomeIndex].BottomEnvironmentAssets[0].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (FC_Ground1Assets.Num() < 8)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			FC_Ground1Assets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = FC_Ground1Assets[0];
		FC_Ground1Assets.RemoveAt(0);

		TempAsset->SetActorTransform(Transform);
		FC_Ground1Assets.Insert(TempAsset, 7);
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (FC_Ground1CycleCount == FC_GroundGenCycles && GetWorld())
	{
		FC_Ground1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(FC_GroundTimerHandle1);
	}
	*/
	FC_Ground1CycleCount++;
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[FC_Ground1BufferedBiomeIndex].BottomEnvironmentAssets[0].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[FC_Ground1BufferedBiomeIndex].BottomEnvironmentAssets[0].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	AActor* SpawnedActor;
	if (GetWorld())
	{
		SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
	}

	// Change biomes when the current cycle count has reached the desired amount (UPROPERTY: GroundGenCycles).
	if (FC_Ground1CycleCount == FC_GroundGenCycles && GetWorld())
	{
		FC_Ground1CycleCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(FC_GroundTimerHandle1);
		///////////////////////////////////////////
		BiomeBuilder->SetActiveBiome(0);
		ChangeBiomeSignature_OnChange.Broadcast(0);
		//GetWorld()->GetTimerManager().ClearTimer(GenerateGroundTimerHandle);
	}
}

void ASceneryGenerator::FC_GroundGen2()
{
	TSubclassOf<AActor> AssetToSpawn;
	AssetToSpawn = BiomeBuilder->AllBiomes[3].TopEnvironmentAssets[0].ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[3].TopEnvironmentAssets[0].SpawnTransform;
	FActorSpawnParameters SpawnParams;

	AActor* SpawnedActor;
	if (GetWorld())
	{
		SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
	}
}

void ASceneryGenerator::FC_SeaGroundGen()
{
	/*
	// Increment the number of cycles elapsed each time this function is called.
	//CurrentGroundCycleCount++;

	// Get ground asset from biome builder class
	TSubclassOf<AActor> AssetToSpawn;
	//AssetToSpawn = BiomeBuilder->AllBiomes[BiomeBuilder->GetActiveBiomeIndex()].GroundAsset.ObstacleAsset;
	AssetToSpawn = BiomeBuilder->AllBiomes[3].GroundAsset.ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[3].GroundAsset.SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	if (FC_SeaGroundAssets.Num() < 10)
	{
		AActor* SpawnedActor;
		if (GetWorld())
		{
			SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
			FC_SeaGroundAssets.Add(SpawnedActor);
		}
	}
	// If the array is already filled, move first spawned ground asset back to the spawn point and reorder
	// the array. This is to avoid destroying and respawning the asset needlessly for better performance.
	else
	{
		AActor* TempAsset = FC_SeaGroundAssets[0];
		FC_SeaGroundAssets.RemoveAt(0);
		TempAsset->SetActorTransform(Transform);
		FC_SeaGroundAssets.Insert(TempAsset, 2);
	}
	*/
	// Get ground asset from biome builder class
	TSubclassOf<AActor> AssetToSpawn;
	//AssetToSpawn = BiomeBuilder->AllBiomes[BiomeBuilder->GetActiveBiomeIndex()].GroundAsset.ObstacleAsset;
	AssetToSpawn = BiomeBuilder->AllBiomes[3].GroundAsset.ObstacleAsset;

	// Get the desired transform and default parameters for spawning the ground
	FTransform Transform = BiomeBuilder->AllBiomes[3].GroundAsset.SpawnTransform;
	FActorSpawnParameters SpawnParams;

	// In the beginning, spawn the ground asset and fill the array of ground assets until it reaches length 3
	// (enough to fill the screen with ground assets)
	AActor* SpawnedActor;
	if (GetWorld())
	{
		SpawnedActor = GetWorld()->SpawnActor<AActor>(AssetToSpawn, Transform, SpawnParams);
	}
}