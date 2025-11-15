// Copyright © 2024 AR App. All Rights Reserved.

#include "ARCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputSystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ARContentManager.h"

AARCharacter::AARCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
}

void AARCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	// Initialize AR Manager with backend URL
	UARContentManager::InitializeARManager(BackendURL);

	// Check backend health
	FOnARContentFetched HealthCallback;
	HealthCallback.BindDynamic(this, &AARCharacter::OnFetchContentComplete);
	UARContentManager::CheckBackendHealth(HealthCallback);
}

void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARCharacter::Look);
	}
}

void AARCharacter::Move(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() > 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value.Get<float>());
	}
}

void AARCharacter::Look(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() > 0.f)
	{
		AddControllerYawInput(Value.Get<float>());
	}
}

void AARCharacter::OnFetchContentComplete(bool bSuccess, const FString& ErrorMessage)
{
	if (GEngine)
	{
		FColor MessageColor = bSuccess ? FColor::Green : FColor::Red;
		FString Message = bSuccess ? TEXT("Backend Connected!") : ErrorMessage;
		
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			MessageColor,
			Message
		);
	}
}

void AARCharacter::FetchARContent()
{
	FOnARContentFetched Callback;
	Callback.BindDynamic(this, &AARCharacter::OnFetchContentComplete);
	UARContentManager::FetchARContent(Callback);
}

void AARCharacter::CreateARContent(const FString& Name, const FString& Description)
{
	FOnARContentFetched Callback;
	Callback.BindDynamic(this, &AARCharacter::OnFetchContentComplete);
	UARContentManager::CreateARContent(Name, Description, TEXT("model"), Callback);
}

void AARCharacter::DeleteARContent(int32 ContentId)
{
	FOnARContentFetched Callback;
	Callback.BindDynamic(this, &AARCharacter::OnFetchContentComplete);
	UARContentManager::DeleteARContent(ContentId, Callback);
}
