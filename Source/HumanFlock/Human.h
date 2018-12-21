// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "StateMachine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Human.generated.h"

UCLASS()
class HUMANFLOCK_API AHuman : public APawn
{
	GENERATED_BODY()
	
private:
	// An instance of the state machine class
	StateMachine<AHuman>* m_pStateMachine;

	// We must take the click coordinates as a member variable so that the states
	// can access them
	FVector ClickLocation;

	// The initial position of the Human needs to be kept for the dodge calculations
	FVector InitialPosition;

	// We must keep a reference to the time between frames
	float DeltaTime = 0.0f;

	// This allows to smoothly move from the initial position to the final position
	float Alpha = 0.0f;

	// This allows the State Machine to transition from State_Run to State_Dodge
	// without relying on a call from AHuman (a foreign class)
	bool bClick = false;

public:	
	// Sets default values for this actor's properties
	AHuman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnClickSensed(FVector ClickLocation);

	UFUNCTION()
	FVector GetClickCoordinates() { return ClickLocation; }

	UFUNCTION()
	FVector GetInitialPosition() { return InitialPosition; }

	UFUNCTION()
	void SetClickedState(bool isClicked) { bClick = isClicked; }

	UFUNCTION()
	float GetDeltaTime() { return DeltaTime; }

	UFUNCTION()
	float GetAlpha() { return Alpha; }

	UFUNCTION()
	void SetAlpha(float Alpha) { this->Alpha = Alpha; }

	UFUNCTION()
	bool GetClickedState() { return bClick; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* HumanMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes")
	float LengthMultiplier = 20.0f;

	// accessor for the member state machine
	StateMachine<AHuman>* GetStateMachine() const { return m_pStateMachine; }
	
};
