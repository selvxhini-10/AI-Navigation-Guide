// Copyright © 2024 Smart Navigation Cane. All Rights Reserved.

#include "SNCBackendManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

FString USNCBackendManager::BackendBaseUrl = TEXT("http://localhost:8000");

void USNCBackendManager::InitializeSNCBackend(const FString& BackendUrl)
{
	BackendBaseUrl = BackendUrl;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			FString::Printf(TEXT("SNC Backend initialized: %s"), *BackendBaseUrl)
		);
	}
}

// ========== DETECTION METHODS ==========

void USNCBackendManager::GetLatestDetection(FOnDetectionReceived OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + TEXT("/api/detection/latest"));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			TArray<FDetectedObject> DetectedObjects;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 200)
				{
					bSuccess = true;
					// Parse detection objects from response
					FString ResponseStr = HttpResponse->GetContentAsString();
					// JSON parsing would go here
				}
			}

			OnComplete.ExecuteIfBound(bSuccess, DetectedObjects);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::GetDetectionFrame(const FString& FrameId, FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/detection/%s"), *FrameId);
	MakeHTTPGetRequest(Endpoint, OnComplete);
}

void USNCBackendManager::GetDetectionStreamStatus(FOnSNCOperationComplete OnComplete)
{
	MakeHTTPGetRequest(TEXT("/api/detection/stream/latest"), OnComplete);
}

// ========== NAVIGATION METHODS ==========

void USNCBackendManager::StartNavigation(
	double OriginLatitude,
	double OriginLongitude,
	double DestinationLatitude,
	double DestinationLongitude,
	FOnSNCOperationComplete OnComplete)
{
	// Build JSON payload
	TSharedPtr<FJsonObject> OriginObj = MakeShareable(new FJsonObject());
	OriginObj->SetNumberField(TEXT("latitude"), OriginLatitude);
	OriginObj->SetNumberField(TEXT("longitude"), OriginLongitude);

	TSharedPtr<FJsonObject> DestinationObj = MakeShareable(new FJsonObject());
	DestinationObj->SetNumberField(TEXT("latitude"), DestinationLatitude);
	DestinationObj->SetNumberField(TEXT("longitude"), DestinationLongitude);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetObjectField(TEXT("origin"), OriginObj);
	JsonObject->SetObjectField(TEXT("destination"), DestinationObj);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	MakeHTTPPostRequest(TEXT("/api/navigation/start-route"), OutputString, OnComplete);
}

void USNCBackendManager::GetRouteStatus(const FString& RouteId, FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/navigation/route/%s"), *RouteId);
	MakeHTTPGetRequest(Endpoint, OnComplete);
}

void USNCBackendManager::UpdateUserLocation(
	const FString& RouteId,
	double CurrentLatitude,
	double CurrentLongitude,
	FOnNavigationUpdate OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/navigation/route/%s/update-location"), *RouteId);

	TSharedPtr<FJsonObject> LocationObj = MakeShareable(new FJsonObject());
	LocationObj->SetNumberField(TEXT("latitude"), CurrentLatitude);
	LocationObj->SetNumberField(TEXT("longitude"), CurrentLongitude);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(LocationObj.ToSharedRef(), Writer);

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(OutputString);

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FNavigationInstruction Instruction;

			if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				bSuccess = true;
				// Parse instruction from response
				FString ResponseStr = HttpResponse->GetContentAsString();
				// JSON parsing would go here
				Instruction.Instruction = TEXT("Continue to next waypoint");
			}

			OnComplete.ExecuteIfBound(bSuccess, Instruction);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::EndNavigation(const FString& RouteId, FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/navigation/route/%s/end"), *RouteId);
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

void USNCBackendManager::ReportObstacle(
	const FString& AlertType,
	const FString& Severity,
	const FString& Description,
	FOnSNCOperationComplete OnComplete)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("alert_type"), AlertType);
	JsonObject->SetStringField(TEXT("severity"), Severity);
	JsonObject->SetStringField(TEXT("description"), Description);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	MakeHTTPPostRequest(TEXT("/api/navigation/obstacle-alert"), OutputString, OnComplete);
}

void USNCBackendManager::GetActiveObstacles(FOnSNCOperationComplete OnComplete)
{
	MakeHTTPGetRequest(TEXT("/api/navigation/obstacles"), OnComplete);
}

// ========== DEVICE METHODS ==========

void USNCBackendManager::RegisterDevice(
	const FString& DeviceId,
	const FString& DeviceType,
	FOnSNCOperationComplete OnComplete)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("device_id"), DeviceId);
	JsonObject->SetStringField(TEXT("device_type"), DeviceType);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	MakeHTTPPostRequest(TEXT("/api/device/register"), OutputString, OnComplete);
}

void USNCBackendManager::SendDeviceHeartbeat(
	const FString& DeviceId,
	float BatteryLevel,
	FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/device/heartbeat/%s?battery_level=%.1f"), *DeviceId, BatteryLevel);
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

void USNCBackendManager::GetDeviceStatus(const FString& DeviceId, FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/device/%s"), *DeviceId);
	MakeHTTPGetRequest(Endpoint, OnComplete);
}

void USNCBackendManager::ListDevices(FOnSNCOperationComplete OnComplete)
{
	MakeHTTPGetRequest(TEXT("/api/device/list"), OnComplete);
}

void USNCBackendManager::ReportDeviceError(
	const FString& DeviceId,
	const FString& ErrorMessage,
	FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/device/%s/error?error_message=%s"), *DeviceId, *FGenericPlatformHttp::UrlEncode(ErrorMessage));
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

// ========== SESSION METHODS ==========

void USNCBackendManager::StartSession(
	const FString& UserId,
	FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/session/start?user_id=%s"), *UserId);
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

void USNCBackendManager::EndSession(
	const FString& SessionId,
	FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/session/%s/end"), *SessionId);
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

void USNCBackendManager::AddDeviceToSession(
	const FString& SessionId,
	const FString& DeviceId,
	FOnSNCOperationComplete OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/session/%s/add-device?device_id=%s"), *SessionId, *DeviceId);
	MakeHTTPPostRequest(Endpoint, TEXT("{}"), OnComplete);
}

// ========== HTTP HELPER METHODS ==========

void USNCBackendManager::MakeHTTPGetRequest(
	const FString& Endpoint,
	FOnSNCOperationComplete OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString Message;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 200 || HttpResponse->GetResponseCode() == 201)
				{
					bSuccess = true;
					Message = TEXT("Request successful");
				}
				else
				{
					Message = FString::Printf(TEXT("HTTP %d: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());
				}
			}
			else
			{
				Message = TEXT("Request failed or invalid response");
			}

			OnComplete.ExecuteIfBound(bSuccess, Message);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::MakeHTTPPostRequest(
	const FString& Endpoint,
	const FString& ContentJson,
	FOnSNCOperationComplete OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(ContentJson);

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString Message;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 201 || HttpResponse->GetResponseCode() == 200)
				{
					bSuccess = true;
					Message = TEXT("Operation successful");
				}
				else
				{
					Message = FString::Printf(TEXT("HTTP %d: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());
				}
			}
			else
			{
				Message = TEXT("Request failed");
			}

			OnComplete.ExecuteIfBound(bSuccess, Message);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::MakeHTTPPutRequest(
	const FString& Endpoint,
	const FString& ContentJson,
	FOnSNCOperationComplete OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(ContentJson);

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString Message;

			if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				bSuccess = true;
				Message = TEXT("Update successful");
			}
			else
			{
				Message = TEXT("Update failed");
			}

			OnComplete.ExecuteIfBound(bSuccess, Message);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::MakeHTTPDeleteRequest(
	const FString& Endpoint,
	FOnSNCOperationComplete OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("DELETE"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString Message;

			if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 204)
			{
				bSuccess = true;
				Message = TEXT("Deletion successful");
			}
			else
			{
				Message = TEXT("Deletion failed");
			}

			OnComplete.ExecuteIfBound(bSuccess, Message);
		});

	Request->ProcessRequest();
}

void USNCBackendManager::ParseDetectionResponse(
	const FString& ResponseString,
	FOnDetectionReceived OnComplete)
{
	// JSON parsing implementation for detection responses
	TArray<FDetectedObject> Objects;
	OnComplete.ExecuteIfBound(true, Objects);
}

void USNCBackendManager::ParseNavigationResponse(
	const FString& ResponseString,
	FOnNavigationUpdate OnComplete)
{
	// JSON parsing implementation for navigation responses
	FNavigationInstruction Instruction;
	OnComplete.ExecuteIfBound(true, Instruction);
}
