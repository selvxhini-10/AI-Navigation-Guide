// Copyright © 2024 AR App. All Rights Reserved.

#include "UnrealMobileARGameMode.h"

AUnrealMobileARGameMode::AUnrealMobileARGameMode()
{
	// Set default pawn class - use a simple pawn or character as needed
	bUseClassicMovementModel = false;
}

void AUnrealMobileARGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			TEXT("AR Application Started!")
		);
	}
}

void AUnrealMobileARGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Red,
			TEXT("AR Application Ended!")
		);
	}
}
