// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "BaseNPC.h"
#include "Engine.h"

/**
 * 
 */
class HUMANFLOCK_API State_Global : public State<ABaseNPC>
{

private:
	// since the state is a singleton, constructor is private
	State_Global() {}

	// copy-ctor and assignment should be private
	State_Global(const State_Global &);
	State_Global& operator= (const State_Global &);

	// turn towards actor if heard and not seen
	//void TurnToActor(ABaseNPC* pNPC);

	// go to follow state
	//void ToFollowState(ABaseNPC* pNPC);

	// go to knocked out state
	void ToKnockedOutState(ABaseNPC* pNPC);

	// the alpha value for the interpolation for the turning around over time of the NPC
	static float DeltaAlpha;

public:
	~State_Global() {}

	static State_Global* State_Global::Instance()
	{
		static State_Global instance;
		return &instance;
	}

	void Enter(ABaseNPC* pNPC) override
	{

	}

	void Execute(ABaseNPC* pNPC) override
	{
		if (pNPC->GetIsDying() && !pNPC->GetHasFallen())
		{
			ToKnockedOutState(pNPC);
			pNPC->SetHasFallen(true);
		}
	}

	void Exit(ABaseNPC* pNPC) override
	{

	}
};
