// Copyright © 2024 AR App. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Http.h"
#include "ARContentManager.generated.h"

/**
 * Structure to hold AR content information
 */
USTRUCT(BlueprintType)
struct FARContent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AR")
	int32 Id;

	UPROPERTY(BlueprintReadWrite, Category = "AR")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "AR")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Category = "AR")
	FString ContentType;

	UPROPERTY(BlueprintReadWrite, Category = "AR")
	FString CreatedAt;
};

/**
 * Delegate for AR content fetch completion
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnARContentFetched,
	bool, bSuccess,
	const FString&, ErrorMessage
);

/**
 * Manager for interacting with the AR backend API
 */
UCLASS()
class UNREALMOBILEAR_API UARContentManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the AR Content Manager with backend URL
	 * @param BackendUrl - The base URL of the FastAPI backend (e.g., http://localhost:8000)
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void InitializeARManager(const FString& BackendUrl);

	/**
	 * Check the health of the backend API
	 * @param OnComplete - Callback when health check completes
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void CheckBackendHealth(FOnARContentFetched OnComplete);

	/**
	 * Fetch all AR content from the backend
	 * @param OnComplete - Callback when fetch completes
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void FetchARContent(FOnARContentFetched OnComplete);

	/**
	 * Fetch specific AR content by ID
	 * @param ContentId - The ID of the content to fetch
	 * @param OnComplete - Callback when fetch completes
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void FetchARContentById(int32 ContentId, FOnARContentFetched OnComplete);

	/**
	 * Create new AR content on the backend
	 * @param Name - Name of the content
	 * @param Description - Description of the content
	 * @param ContentType - Type of AR content
	 * @param OnComplete - Callback when creation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void CreateARContent(
		const FString& Name,
		const FString& Description,
		const FString& ContentType,
		FOnARContentFetched OnComplete
	);

	/**
	 * Delete AR content from the backend
	 * @param ContentId - The ID of the content to delete
	 * @param OnComplete - Callback when deletion completes
	 */
	UFUNCTION(BlueprintCallable, Category = "AR")
	static void DeleteARContent(int32 ContentId, FOnARContentFetched OnComplete);

private:
	static FString BackendBaseUrl;

	/**
	 * Helper function to make HTTP GET requests
	 */
	static void MakeHTTPGetRequest(
		const FString& Endpoint,
		FOnARContentFetched OnComplete
	);

	/**
	 * Helper function to make HTTP POST requests
	 */
	static void MakeHTTPPostRequest(
		const FString& Endpoint,
		const FString& ContentJson,
		FOnARContentFetched OnComplete
	);

	/**
	 * Helper function to make HTTP DELETE requests
	 */
	static void MakeHTTPDeleteRequest(
		const FString& Endpoint,
		FOnARContentFetched OnComplete
	);
};
