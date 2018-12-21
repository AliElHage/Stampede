// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "assert.h"

/**
 * 
 */
template<class entity_type>
class HUMANFLOCK_API StateMachine
{
private:

	// a pointer to the agent that owns this instance
	entity_type* m_pOwner;

	State<entity_type>* m_pCurrentState;

	// a record of the last state the agent was in
	State<entity_type>* m_pPreviousState;

	// this state logic is called everytime the state machine is updated
	State<entity_type>* m_pGlobalState;

public:
	StateMachine(entity_type* owner) :m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}

	~StateMachine();

	// use these methods to initialize the FSM
	void SetCurrentState(State<entity_type>* state) { m_pCurrentState = state; }
	void SetGlobalState(State<entity_type>* state) { m_pGlobalState = state; }
	void SetPreviousState(State<entity_type>* state) { m_pPreviousState = state; }

	// call this to update the FSM
	void Update() const
	{
		// if a global state exists, call its execute method
		if (m_pGlobalState) m_pGlobalState->Execute(m_pOwner);

		// same for the current state
		if (m_pCurrentState) m_pCurrentState->Execute(m_pOwner);
	}

	// change to a new state
	void ChangeState(State<entity_type>* pNewState)
	{
		assert(pNewState && "<StateMachine::ChangeState>: trying to change to a null state");

		// keep a record of the previous state
		m_pPreviousState = m_pCurrentState;

		// call the exit method of the existing state
		m_pCurrentState->Exit(m_pOwner);

		// change state to the new state
		m_pCurrentState = pNewState;

		// call the entry method to the new state
		m_pCurrentState->Enter(m_pOwner);
	}

	// change state back to previous state
	void RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}

	// accessors
	State<entity_type>* GetCurrentState() const { return m_pCurrentState; }
	State<entity_type>* GetGlobalState() const { return m_pGlobalState; }
	State<entity_type>* GetPreviousState() const { return m_pPreviousState; }

	// returns true if the current state's type is equal to the type of the
	// class passed as parameter
	bool isInState(const State<entity_type>& st) const
	{
		return std::is_same<st, m_pCurrentState>::value;
	}
};
