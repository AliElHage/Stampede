// Fill out your copyright notice in the Description page of Project Settings.

#include "Human.h"
#include "State_Run.h"
#include "State_Global.h"
#include "Engine.h"


// Sets default values
AHuman::AHuman()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating components
	HumanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	// Give the human a state machine
	//m_pStateMachine = new StateMachine<AHuman>(this);

	// Set the first and global states
	//m_pStateMachine->SetCurrentState(State_Run::Instance());
	//m_pStateMachine->SetGlobalState(State_Global::Instance());

}

void AHuman::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AHuman::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHuman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Assign DeltaTime to our member variable
	this->DeltaTime = DeltaTime;

	// Update the State Machine
	//m_pStateMachine->Update();
}

void AHuman::OnClickSensed(FVector ClickLocation)
{
	bClick = true;
	InitialPosition = GetActorLocation();
	this->ClickLocation = ClickLocation;
}

