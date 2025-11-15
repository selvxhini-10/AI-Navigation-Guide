// Copyright © 2024 AR App. All Rights Reserved.

#include "ARContentManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

FString UARContentManager::BackendBaseUrl = TEXT("http://localhost:8000");

void UARContentManager::InitializeARManager(const FString& BackendUrl)
{
	BackendBaseUrl = BackendUrl;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			FString::Printf(TEXT("AR Manager initialized with backend: %s"), *BackendBaseUrl)
		);
	}
}

void UARContentManager::CheckBackendHealth(FOnARContentFetched OnComplete)
{
	MakeHTTPGetRequest(TEXT("/health"), OnComplete);
}

void UARContentManager::FetchARContent(FOnARContentFetched OnComplete)
{
	MakeHTTPGetRequest(TEXT("/api/content"), OnComplete);
}

void UARContentManager::FetchARContentById(int32 ContentId, FOnARContentFetched OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/content/%d"), ContentId);
	MakeHTTPGetRequest(Endpoint, OnComplete);
}

void UARContentManager::CreateARContent(
	const FString& Name,
	const FString& Description,
	const FString& ContentType,
	FOnARContentFetched OnComplete)
{
	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("name"), Name);
	JsonObject->SetStringField(TEXT("description"), Description);
	JsonObject->SetStringField(TEXT("content_type"), ContentType);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	MakeHTTPPostRequest(TEXT("/api/content"), OutputString, OnComplete);
}

void UARContentManager::DeleteARContent(int32 ContentId, FOnARContentFetched OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/api/content/%d"), ContentId);
	MakeHTTPDeleteRequest(Endpoint, OnComplete);
}

void UARContentManager::MakeHTTPGetRequest(
	const FString& Endpoint,
	FOnARContentFetched OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString ErrorMessage;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 200)
				{
					bSuccess = true;
				}
				else
				{
					ErrorMessage = FString::Printf(
						TEXT("HTTP Error: %d - %s"),
						HttpResponse->GetResponseCode(),
						*HttpResponse->GetContentAsString()
					);
				}
			}
			else
			{
				ErrorMessage = TEXT("Request failed or invalid response");
			}

			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		});

	Request->ProcessRequest();
}

void UARContentManager::MakeHTTPPostRequest(
	const FString& Endpoint,
	const FString& ContentJson,
	FOnARContentFetched OnComplete)
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
			FString ErrorMessage;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 201)
				{
					bSuccess = true;
				}
				else
				{
					ErrorMessage = FString::Printf(
						TEXT("HTTP Error: %d - %s"),
						HttpResponse->GetResponseCode(),
						*HttpResponse->GetContentAsString()
					);
				}
			}
			else
			{
				ErrorMessage = TEXT("Request failed or invalid response");
			}

			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		});

	Request->ProcessRequest();
}

void UARContentManager::MakeHTTPDeleteRequest(
	const FString& Endpoint,
	FOnARContentFetched OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BackendBaseUrl + Endpoint);
	Request->SetVerb(TEXT("DELETE"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda(
		[OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			bool bSuccess = false;
			FString ErrorMessage;

			if (bSucceeded && HttpResponse.IsValid())
			{
				if (HttpResponse->GetResponseCode() == 204 || HttpResponse->GetResponseCode() == 200)
				{
					bSuccess = true;
				}
				else
				{
					ErrorMessage = FString::Printf(
						TEXT("HTTP Error: %d - %s"),
						HttpResponse->GetResponseCode(),
						*HttpResponse->GetContentAsString()
					);
				}
			}
			else
			{
				ErrorMessage = TEXT("Request failed or invalid response");
			}

			OnComplete.ExecuteIfBound(bSuccess, ErrorMessage);
		});

	Request->ProcessRequest();
}
