// Fill out your copyright notice in the Description page of Project Settings.

#include "State_Global.h"
#include "State_KnockedOut.h"

void State_Global::ToKnockedOutState(ABaseNPC* pNPC)
{
	// Access the state machine to change the state to the desired one
	pNPC->GetStateMachine()->ChangeState(State_KnockedOut::Instance());
}
