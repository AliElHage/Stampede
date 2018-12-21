// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template <class NPC_type>
class HUMANFLOCK_API State
{
public:
	State(){}
	~State(){}

	// this will execute when the state is entered
	virtual void Enter(NPC_type*) = 0;

	// this is called by the miner’s update function each update step
	virtual void Execute(NPC_type*) = 0;

	// this will execute when the state is exited
	virtual void Exit(NPC_type*) = 0;
};
