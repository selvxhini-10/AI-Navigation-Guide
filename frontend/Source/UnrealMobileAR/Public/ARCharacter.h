// Copyright © 2024 AR App. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "ARContentManager.h"
#include "ARCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AARPlacedObject;

/**
 * Player character for the AR application
 */
UCLASS()
class UNREALMOBILEAR_API AARCharacter : public APawn
{
	GENERATED_BODY()

public:
	AARCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AR")
	UCameraComponent* CameraComponent;

	// Input System
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UEnhancedInputComponent* EnhancedInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	// Backend configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backend")
	FString BackendURL = TEXT("http://localhost:8000");

	// AR Objects
	UPROPERTY(BlueprintReadWrite, Category = "AR")
	TArray<AARPlacedObject*> PlacedObjects;

	// Input callbacks
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnFetchContentComplete(bool bSuccess, const FString& ErrorMessage);

public:
	UFUNCTION(BlueprintCallable, Category = "AR")
	void FetchARContent();

	UFUNCTION(BlueprintCallable, Category = "AR")
	void CreateARContent(const FString& Name, const FString& Description);

	UFUNCTION(BlueprintCallable, Category = "AR")
	void DeleteARContent(int32 ContentId);
};
