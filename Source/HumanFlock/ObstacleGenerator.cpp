// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstacleGenerator.h"
#include "State_Stroll.h"
#include "Engine.h"
#include "BiomeBuilder.h"


// Sets default values
AObstacleGenerator::AObstacleGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnInterval = 2.5f;
	InitialSpawnInterval = SpawnInterval;
	DifficultyInterval = 20.0f;
	LowestSpawnInterval = 1.5f;
	ProgressRatio = 0.5f;
	SpawnIncrements = 0.1f;

	SpawnableNumber = 2;

	SpawnMap.Init(false, 5);
	SpawnLatitudes.Init(0.0f, 5);
	SpawnLatitudes[0] = -400.0f;
	SpawnLatitudes[1] = -200.0f;
	SpawnLatitudes[2] = 0.0f;
	SpawnLatitudes[3] = 200.0f;
	SpawnLatitudes[4] = 400.0f;

}

// Called when the game starts or when spawned
void AObstacleGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TIMERD"));
		}*/
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AObstacleGenerator::GenerateSpawnMap, SpawnInterval, true);
		GetWorld()->GetTimerManager().SetTimer(DifficultyScalingTimer, this, &AObstacleGenerator::ScaleDifficulty, DifficultyInterval, true);
		GetWorld()->GetTimerManager().SetTimer(NPCSpawnTimer, this, &AObstacleGenerator::SpawnNPCs, SpawnInterval, true, SpawnInterval / 2.0f);
		GetWorld()->GetTimerManager().SetTimer(ScoringTimer, this, &AObstacleGenerator::UpdateScore, ScoringInterval, true, ScoringInterval);
	}

	if (GetWorld())
	{
		GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	}

	if (GameInstance)
	{
		GameInstance->SetSpawnCoordinates(SpawnLatitudes);
		FlockSize = GameInstance->GetNPCs().Num();
	}

	FMath::SRandInit(GetUniqueID());
}

// Called every frame
void AObstacleGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (Counter > SpawnInterval)
	{
		//If the usefulactorbp is valid
		if (ObstacleBP)
		{
			//Spawn parameters for the current spawn.
			//We can use this for a number of options like disable collision or adjust the spawn position
			//if a collision is happening in the spawn point etc..
			FActorSpawnParameters SpawnParams;

			FTransform Transform;
			FVector SpawnPoint = FVector(SpawnLatitude, 0.0f, 0.0f);
			Transform.SetLocation(SpawnPoint);

			//Actual Spawn. The following function returns a reference to the spawned actor
			AActor* ActorRef = GetWorld()->SpawnActor<AActor>(ObstacleBP, Transform, SpawnParams);

			//GLog->Log("Spawned the UsefulActor.");
		}
		Counter = 0.0f;
	}

	Counter += DeltaTime;*/

}

bool AObstacleGenerator::IsGenerated()
{
	float RandomNumber = FMath::SRand();

	if (RandomNumber > 0.35f) return true;
	else return false;
}

TArray<TArray<int32>> AObstacleGenerator::CleanSpawnMap()
{
	TArray<TArray<int32>> FinalSpawnMap;
	int ObstacleLength = 1;
	int ObstacleStreak = 0;
	bool LastIsObstacle = false;
	for (int i = 0; i < SpawnMap.Num(); i++)
	{
		TArray<int32> MapEntry;
		MapEntry.Init(0, 2);
		if (SpawnMap[i])
		{
			ObstacleStreak++;
			if (i == SpawnMap.Num() - 1)
			{
				LastIsObstacle = true;
			}
		}
		else
		{
			if (ObstacleStreak == 0)
			{
				MapEntry[0] = 0;
				MapEntry[1] = 1;
				FinalSpawnMap.Add(MapEntry);

				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("FALSE"));
				}
			}
			for (int j = 0; j < ObstacleStreak; j++)
			{
				if (j == ObstacleStreak - 1)
				{
					MapEntry[0] = 1;
					MapEntry[1] = ObstacleLength;
					FinalSpawnMap.Add(MapEntry);
					ObstacleLength = 1;

					if (GEngine)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TRUE"));
					}
				}
				else
				{
					if (FMath::SRand() > 0.5f)
					{
						ObstacleLength++;
					}
					else
					{
						MapEntry[0] = 1;
						MapEntry[1] = ObstacleLength;
						FinalSpawnMap.Add(MapEntry);
						ObstacleLength = 1;

						if (GEngine)
						{
							//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TRUE"));
						}
					}
				}
			}
			if (ObstacleStreak != 0)
			{
				MapEntry[0] = 0;
				MapEntry[1] = 1;
				FinalSpawnMap.Add(MapEntry);

				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("FALSE"));
				}
			}
			ObstacleStreak = 0;
		}
	}
	if (LastIsObstacle)
	{
		TArray<int32> MapEntry;
		MapEntry.Init(0, 2);
		for (int j = 0; j < ObstacleStreak; j++)
		{
			if (j == ObstacleStreak - 1)
			{
				MapEntry[0] = 1;
				MapEntry[1] = ObstacleLength;
				FinalSpawnMap.Add(MapEntry);
				ObstacleLength = 1;

				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TRUE"));
				}
			}
			else
			{
				if (FMath::SRand() > 0.5f)
				{
					ObstacleLength++;
				}
				else
				{
					MapEntry[0] = 1;
					MapEntry[1] = ObstacleLength;
					FinalSpawnMap.Add(MapEntry);
					ObstacleLength = 1;

					if (GEngine)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TRUE"));
					}
				}
			}
		}
	}
	return FinalSpawnMap;
}

void AObstacleGenerator::GenerateSpawnMap()
{
	TArray<int32> SpawnOrder;

	// Determine the current center of all NPCs
	float MeanLatitude = 0.0f;
	for (int i = 0; i < GameInstance->GetNPCs().Num(); i++)
	{
		if (GameInstance->GetNPCs().Num() > 0)
		{
			if (GameInstance->GetNPCs()[i])
			{
				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(GameInstance->GetNPCs().Num()));
				}
				if (GameInstance->GetNPCs().Num() > 0)
				{
					if (GameInstance->GetNPCs()[i])
					{
						MeanLatitude += GameInstance->GetNPCs()[i]->GetActorLocation().X;
					}
				}
				//MeanLatitude += GameInstance->GetNPCs()[i]->GetActorLocation().X;
			}
		}
	}
	MeanLatitude = MeanLatitude / GameInstance->GetNPCs().Num();

	// Determine from the center just computed the coordinate from which to start spawning
	for (int i = 0; i < SpawnLatitudes.Num(); i++)
	{
		if (MeanLatitude < SpawnLatitudes[i])
		{
			SpawnOrder.Add(i);
			break;
		}
	}

	// If the center is under all spawn longitudes, spawn from the bottom to the top.
	if (SpawnOrder.Num() == 0)
	{
		SpawnOrder.Add(0);
	}

	if (GEngine && SpawnOrder.Num() > 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(SpawnOrder.Num()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(SpawnOrder[0]));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(MeanLatitude));
	}

	for (int i = 1; i <= SpawnLatitudes.Num() - SpawnLatitudes[0] - 1; i++)
	{
		int32 index = i;
		float FirstCheck = FMath::SRand();
		if (FirstCheck > 0.5f) index *= -1;

		if (SpawnOrder[0] + index < SpawnLatitudes.Num() && SpawnOrder[0] + index >= 0)
		{
			SpawnOrder.Add(SpawnOrder[0] + index);
		}
		if (SpawnOrder[0] - index < SpawnLatitudes.Num() && SpawnOrder[0] - index >= 0)
		{
			SpawnOrder.Add(SpawnOrder[0] - index);
		}
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(i));
		}
	}
	if (GEngine && SpawnOrder.Num() > 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(SpawnOrder.Num()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(SpawnOrder[0]));
	}
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(GameInstance->GetNPCs().Num()));
	}*/

	if (ObstacleBP)
	{
		int Spawned = 0;
		for (int i = 0; i < SpawnLatitudes.Num(); i++)
		{
			if (Spawned < SpawnableNumber)
			{
				SpawnMap[SpawnOrder[i]] = IsGenerated();
				if (SpawnMap[SpawnOrder[i]])
				{
					Spawned++;
				}
			}
			else
			{
				SpawnMap[SpawnOrder[i]] = false;
			}
		}

		if (Spawned == 0)
		{
			SpawnMap[(int)(FMath::SRand() * (5.0f - TNumericLimits< float >::Min()))] = true;
		}

		if (GameInstance)
		{
			GameInstance->SetSpawnMap(SpawnMap);
		}

		////////////////////////////////////////////////////////
		TArray<TArray<int32>> testArray = CleanSpawnMap();

		int spaces = 0;
		for (int i = 0; i < testArray.Num(); i++)
		{
			spaces += testArray[i][1];
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(spaces));
		}
		////////////////////////////////////////////////////////
		int index = 0;
		for (int i = 0; i < testArray.Num(); i++)
		{
			if (testArray[i][0] == 1)
			{
				FActorSpawnParameters SpawnParams;
				FTransform Transform;

				float RandomOffset = FMath::SRand() * 300.0f;
				if (FMath::SRand() > 0.5f)
				{
					RandomOffset = -RandomOffset;
				}
				float SpawnLatitude = 0;
				for (int j = index; j < index + testArray[i][1]; j++)
				{
					SpawnLatitude += SpawnLatitudes[j];
				}
				SpawnLatitude = SpawnLatitude / testArray[i][1];
				FVector SpawnPoint = FVector(SpawnLatitude, SpawnLongitude + RandomOffset, 51.2f);
				Transform.SetLocation(SpawnPoint);

				//Actual Spawn. The following function returns a reference to the spawned actor
				AActor* ActorRef;
				switch (testArray[i][1])
				{
				case 1:
					if (GameInstance)
					{
						ObstacleBP = Cast<UBiomeBuilder>(GameInstance->GetBiomeBuilder())->ChooseAssetToSpawn(1, FMath::SRand());
					}
					ActorRef = GetWorld()->SpawnActor<AActor>(ObstacleBP, Transform, SpawnParams);
					break;
				case 2:
					if (GameInstance)
					{
						DoubleObstacleBP = Cast<UBiomeBuilder>(GameInstance->GetBiomeBuilder())->ChooseAssetToSpawn(2, FMath::SRand());
					}
					ActorRef = GetWorld()->SpawnActor<AActor>(DoubleObstacleBP, Transform, SpawnParams);
					break;
				case 3:
					ActorRef = GetWorld()->SpawnActor<AActor>(TripleObstacleBP, Transform, SpawnParams);
					break;
				case 4:
					ActorRef = GetWorld()->SpawnActor<AActor>(QuadrupleObstacleBP, Transform, SpawnParams);
					break;
				default:
					break;
				}
				//AActor* ActorRef = GetWorld()->SpawnActor<AActor>(ObstacleBP, Transform, SpawnParams);

			}
			index += testArray[i][1];
			/*if (SpawnMap[i])
			{
				//Spawn parameters for the current spawn.
				//We can use this for a number of options like disable collision or adjust the spawn position
				//if a collision is happening in the spawn point etc..
				FActorSpawnParameters SpawnParams;

				FTransform Transform;
				
				float RandomOffset = FMath::SRand() * 300.0f;
				if (FMath::SRand() > 0.5f)
				{
					RandomOffset = -RandomOffset;
				}
				FVector SpawnPoint = FVector(SpawnLatitudes[i], SpawnLongitude + RandomOffset, 0.0f);
				Transform.SetLocation(SpawnPoint);

				//Actual Spawn. The following function returns a reference to the spawned actor
				AActor* ActorRef = GetWorld()->SpawnActor<AActor>(ObstacleBP, Transform, SpawnParams);
			}*/
		}

	}
}

void AObstacleGenerator::SpawnNPCs()
{
	if (HumansBP)
	{
		if (FMath::SRand() > 0.7f)
		{
			FActorSpawnParameters SpawnParams;
			FTransform Transform;
			FVector SpawnPoint = FVector(FMath::SRand() * 300.0f, 1000.0f, 0.0f);
			if (FMath::SRand() > 0.5f)
			{
				SpawnPoint.X *= -1;
			}
			Transform.SetLocation(SpawnPoint);
			Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			ACharacter* ActorRef = GetWorld()->SpawnActor<ACharacter>(HumansBP, Transform, SpawnParams);
			ABaseNPC* pNPC = Cast<ABaseNPC>(ActorRef);
			if (pNPC)
			{
				pNPC->GetStateMachine()->ChangeState(State_Stroll::Instance());
				pNPC->bIsInFlock = false;
			}
			if (GetWorld() && GameInstance)
			{
				GameInstance->RemoveNPC(pNPC);
			}
		}
	}
	
}

void AObstacleGenerator::SpawnObstacles()
{
	
}

void AObstacleGenerator::ScaleDifficulty()
{
	if (SpawnInterval >= LowestSpawnInterval)
	{
		SpawnInterval -= SpawnIncrements;
	}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AObstacleGenerator::GenerateSpawnMap, SpawnInterval, true);
		GetWorld()->GetTimerManager().SetTimer(NPCSpawnTimer, this, &AObstacleGenerator::SpawnNPCs, SpawnInterval, true, SpawnInterval / 2.0f);
	}

	if ((InitialSpawnInterval - SpawnInterval) / (InitialSpawnInterval - LowestSpawnInterval) > ProgressRatio)
	{
		if (SpawnableNumber != 4)
		{
			SpawnableNumber++;
			ProgressRatio = 1.0f;
			DifficultyMultiplier += DifficultyMultiplierIncrement;
		}
	}
}

void AObstacleGenerator::UpdateScore()
{
	if (GameInstance)
	{
		int FlockSizeDifference = GameInstance->GetNPCs().Num() - FlockSize;
		FlockSizeMultiplier += FlockSizeDifference * FlockSizeMultiplierIncrement;
		FlockSize = GameInstance->GetNPCs().Num();
	}
	Score += BaseScoreUnits * FlockSizeMultiplier * DifficultyMultiplier;

	GameInstance->score = FMath::Floor(Score);
}

