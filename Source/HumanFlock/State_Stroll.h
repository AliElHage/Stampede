// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstance.h"
#include "State.h"
#include "BaseNPC.h"
#include "NPC_Controller.h"
#include "Engine.h"

/**
 * 
 */
class HUMANFLOCK_API State_Stroll : public State<ABaseNPC>
{
public:
private:
	// since the state is a singleton, constructor is private
	State_Stroll() {}

	// copy-ctor and assignment should be private
	State_Stroll(const State_Stroll &);
	State_Stroll& operator= (const State_Stroll &);

	// avoid incoming obstacles while strolling
	void AvoidObstacle(ABaseNPC* pNPC);

	// go to dodge state
	void ToDodgeState(ABaseNPC* pNPC);

	TArray<float> SpawnLatitudes;

	static float Alpha;

public:
	~State_Stroll() {}

	static State_Stroll* State_Stroll::Instance()
	{
		static State_Stroll instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{
		pNPC->SetIsRunning(true);
		if (pNPC->GetWorld())
		{
			UMyGameInstance* GameInstance;
			GameInstance = Cast<UMyGameInstance>(pNPC->GetWorld()->GetGameInstance());
			if (GameInstance)
			{
				SpawnLatitudes = GameInstance->GetSpawnCoordinates();
			}
		}
	}

	void Execute(ABaseNPC* pNPC) override
	{
		AvoidObstacle(pNPC);
	}

	void Exit(ABaseNPC* pNPC) override
	{
		pNPC->SetIsRunning(false);
	}

};
