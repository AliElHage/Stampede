// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCharacter.h"
#include "BaseNPC.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"

int AMyPlayerCharacter::FlockSize = 0;

// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass> ActorClassFinder(TEXT("Blueprint'/Game/Blueprints/BP_Lightning.BP_Lightning_C'"));
	lightning = ActorClassFinder.Object;
}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	}

	OnPlayerClick.AddDynamic(this, &AMyPlayerCharacter::OnWorldClick);
	if (AllHumans.Num() > 0)
	{
		for (int i = 0; i < AllHumans.Num(); i++)
		{
			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ZANZIBAR"));
			}
			if (IsValidLowLevel() && AllHumans[i])
			{
				OnPlayerClick.AddDynamic(AllHumans[i], &ABaseNPC::OnClickSensed);
			}
		}
	}

	FlockSize = AllHumans.Num();
	//this->OnPlayerClick.AddDynamic(AHuman::PlayerActor, &AHuman::OnClickSensed);

	if (GetWorld())
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	//Set up UI widget elements
	//Game Over screen
	if (wGameOverMenu) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		GameOverMenu = CreateWidget<UUserWidget>(this->PlayerController, wGameOverMenu);
	}
	
	//Ingame UI screen
	if (wInGameUI) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		InGameUI = CreateWidget<UUserWidget>(this->PlayerController, wInGameUI);
	}

	if (InGameUI)
	{
		//let add it to the view port
		InGameUI->AddToViewport();
	}

}

// Called every frame
void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton) && !bHasBeenClicked)
	{
		FVector mouseLocation, mouseDirection;
		UCameraComponent* activeCam;
		if (FindActiveCameraInWorld(activeCam))
		{
			if (activeCam->ProjectionMode == ECameraProjectionMode::Perspective)
				PlayerController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);
			else
			{
				FVector2D mouseXY;
				PlayerController->GetMousePosition(mouseXY.X, mouseXY.Y);
				FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

				OrthoDeprojectMousePositionToWorld(activeCam, mouseXY, viewportSize, mouseLocation, mouseDirection);
			}
			OnPlayerClick.Broadcast(Intersection(mouseLocation, mouseDirection, 0.0f));
			bHasBeenClicked = true;
		}
	}
	if (!PlayerController->IsInputKeyDown(EKeys::LeftMouseButton) && bHasBeenClicked)
	{
		bHasBeenClicked = false;
	}

	if (GameInstance)
	{
		AllHumans = GameInstance->GetNPCs();
		if (AllHumans.Num() > FlockSize)
		{
			if (IsValidLowLevel() && AllHumans[AllHumans.Num() - 1])
			{
				OnPlayerClick.AddDynamic(AllHumans[AllHumans.Num() - 1], &ABaseNPC::OnClickSensed);
			}
		}
		FlockSize = AllHumans.Num();
		if (FlockSize == 0 && !bIsGameOver)
		{
			bIsGameOver = true;
			if (GameOverMenu->IsValidLowLevel())
			{
				//let add it to the view port
				GameOverMenu->AddToViewport();
			}

			//Show the Cursor.
			/*if (GetWorld()->GetFirstPlayerController())
			{
				GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			}*/
		}
	}
}

// Called to bind functionality to input
void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AMyPlayerCharacter::OnWorldClick(FVector ClickLocation)
{
	FString myString = ClickLocation.ToString();
	AActor* lightningActor = GetWorld()->SpawnActor<AActor>(lightning, ClickLocation, FRotator::ZeroRotator);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *myString);
	}
}

FVector AMyPlayerCharacter::Intersection(FVector start, FVector dir, float Z)
{
	FVector ProjectedVector;
	float CoordX, CoordY, CoordZ, t;

	CoordZ = Z;

	t = -start.Z / dir.Z;

	CoordX = start.X + dir.X * t;
	CoordY = start.Y + dir.Y * t;
	
	ProjectedVector.X = CoordX;
	ProjectedVector.Y = CoordY;
	ProjectedVector.Z = CoordZ;
	return ProjectedVector;
}

bool AMyPlayerCharacter::FindActiveCameraInWorld(UCameraComponent* &camComp)
{
	TArray<AActor*> camActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), camActors);

	for (AActor* actor : camActors)
	{
		TInlineComponentArray<UCameraComponent*> cameras;
		actor->GetComponents<UCameraComponent>(cameras);

		for (UCameraComponent* camComponent : cameras)
		{
			if (camComponent->bIsActive)
			{
				camComp = camComponent;
				return true;
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Green, "No active camera found");

	return false;
}

bool AMyPlayerCharacter::OrthoDeprojectMousePositionToWorld(UCameraComponent* camComp, FVector2D mouseXY, FVector2D viewportSize, FVector& position, FVector& dir)
{
	if (camComp->ProjectionMode != ECameraProjectionMode::Orthographic)
		return false;
	FTransform cameraTransform = camComp->GetComponentTransform();
	float camWidth = camComp->OrthoWidth,
		camHeight = camWidth / camComp->AspectRatio;
	FVector2D mousePos2D = FVector2D(mouseXY.X / viewportSize.X * camWidth - camWidth / 2, 
		mouseXY.Y / viewportSize.Y * camHeight - camHeight / 2);

	FVector rayStart = camComp->GetComponentLocation() + mousePos2D.X * cameraTransform.GetUnitAxis(EAxis::Y).GetSafeNormal() - mousePos2D.Y * cameraTransform.GetUnitAxis(EAxis::Z).GetSafeNormal(),
		rayEnd = rayStart;

	rayStart += camComp->OrthoNearClipPlane * cameraTransform.GetUnitAxis(EAxis::X);
	rayEnd += camComp->OrthoFarClipPlane * cameraTransform.GetUnitAxis(EAxis::X);

	position = rayStart;
	dir = rayEnd - rayStart;
	return true;
}