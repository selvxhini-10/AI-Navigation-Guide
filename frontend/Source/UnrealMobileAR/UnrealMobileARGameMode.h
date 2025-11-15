// Copyright © 2024 AR App. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealMobileARGameMode.generated.h"

/**
 * Game mode for the AR application
 */
UCLASS()
class UNREALMOBILEAR_API AUnrealMobileARGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUnrealMobileARGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
