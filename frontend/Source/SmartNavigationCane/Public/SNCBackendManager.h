// Copyright © 2024 Smart Navigation Cane. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Http.h"
#include "SNCBackendManager.generated.h"

/**
 * Structure for detected objects from camera
 */
USTRUCT(BlueprintType)
struct FDetectedObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	FString ObjectId;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	FString Label;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	float Confidence;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	float Distance;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	FString PositionX;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Detection")
	FString PositionY;
};

/**
 * Structure for GPS location
 */
USTRUCT(BlueprintType)
struct FGPSLocation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	double Latitude;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	double Longitude;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	float Altitude;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	float Heading;
};

/**
 * Structure for navigation instruction
 */
USTRUCT(BlueprintType)
struct FNavigationInstruction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	FString Instruction;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	float DistanceMeters;

	UPROPERTY(BlueprintReadWrite, Category = "SNC|Navigation")
	int32 DurationSeconds;
};

/**
 * Callback delegates
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnSNCOperationComplete,
	bool, bSuccess,
	const FString&, ResponseMessage
);

DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnDetectionReceived,
	bool, bSuccess,
	const TArray<FDetectedObject>&, DetectedObjects
);

DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnNavigationUpdate,
	bool, bSuccess,
	const FNavigationInstruction&, NextInstruction
);

/**
 * Smart Navigation Cane Backend Manager
 * Handles all communication with the SNC backend API
 */
UCLASS()
class SMARTNAVIGATIONCANE_API USNCBackendManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the SNC Backend Manager with server URL
	 * @param BackendUrl - The base URL of the FastAPI backend
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Backend")
	static void InitializeSNCBackend(const FString& BackendUrl);

	// ========== DETECTION ENDPOINTS ==========

	/**
	 * Request latest object detection results
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Detection")
	static void GetLatestDetection(FOnDetectionReceived OnComplete);

	/**
	 * Get detection results for a specific frame
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Detection")
	static void GetDetectionFrame(const FString& FrameId, FOnSNCOperationComplete OnComplete);

	/**
	 * Get detection stream status
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Detection")
	static void GetDetectionStreamStatus(FOnSNCOperationComplete OnComplete);

	// ========== NAVIGATION ENDPOINTS ==========

	/**
	 * Start navigation to a destination
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void StartNavigation(
		double OriginLatitude,
		double OriginLongitude,
		double DestinationLatitude,
		double DestinationLongitude,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * Get current route status
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void GetRouteStatus(const FString& RouteId, FOnSNCOperationComplete OnComplete);

	/**
	 * Update user location and get next instruction
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void UpdateUserLocation(
		const FString& RouteId,
		double CurrentLatitude,
		double CurrentLongitude,
		FOnNavigationUpdate OnComplete
	);

	/**
	 * End navigation session
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void EndNavigation(const FString& RouteId, FOnSNCOperationComplete OnComplete);

	/**
	 * Report obstacle/hazard on route
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void ReportObstacle(
		const FString& AlertType,
		const FString& Severity,
		const FString& Description,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * Get active obstacles/hazards
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Navigation")
	static void GetActiveObstacles(FOnSNCOperationComplete OnComplete);

	// ========== DEVICE ENDPOINTS ==========

	/**
	 * Register a device (Arduino, ESP32-CAM)
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Device")
	static void RegisterDevice(
		const FString& DeviceId,
		const FString& DeviceType,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * Send device heartbeat
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Device")
	static void SendDeviceHeartbeat(
		const FString& DeviceId,
		float BatteryLevel,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * Get device status
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Device")
	static void GetDeviceStatus(const FString& DeviceId, FOnSNCOperationComplete OnComplete);

	/**
	 * List all connected devices
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Device")
	static void ListDevices(FOnSNCOperationComplete OnComplete);

	/**
	 * Report device error
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Device")
	static void ReportDeviceError(
		const FString& DeviceId,
		const FString& ErrorMessage,
		FOnSNCOperationComplete OnComplete
	);

	// ========== SESSION ENDPOINTS ==========

	/**
	 * Start a navigation session
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Session")
	static void StartSession(
		const FString& UserId,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * End navigation session
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Session")
	static void EndSession(
		const FString& SessionId,
		FOnSNCOperationComplete OnComplete
	);

	/**
	 * Add device to session
	 */
	UFUNCTION(BlueprintCallable, Category = "SNC|Session")
	static void AddDeviceToSession(
		const FString& SessionId,
		const FString& DeviceId,
		FOnSNCOperationComplete OnComplete
	);

private:
	static FString BackendBaseUrl;

	// HTTP request helpers
	static void MakeHTTPGetRequest(
		const FString& Endpoint,
		FOnSNCOperationComplete OnComplete
	);

	static void MakeHTTPPostRequest(
		const FString& Endpoint,
		const FString& ContentJson,
		FOnSNCOperationComplete OnComplete
	);

	static void MakeHTTPPutRequest(
		const FString& Endpoint,
		const FString& ContentJson,
		FOnSNCOperationComplete OnComplete
	);

	static void MakeHTTPDeleteRequest(
		const FString& Endpoint,
		FOnSNCOperationComplete OnComplete
	);

	// JSON parsing helpers
	static void ParseDetectionResponse(
		const FString& ResponseString,
		FOnDetectionReceived OnComplete
	);

	static void ParseNavigationResponse(
		const FString& ResponseString,
		FOnNavigationUpdate OnComplete
	);
};
