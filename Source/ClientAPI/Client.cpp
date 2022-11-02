// Fill out your copyright notice in the Description page of Project Settings.


#include "Client.h"

#include "JsonObjectConverter.h"
#include "SocketIOClientComponent.h"

// Sets default values
AClient::AClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SIOClientComponent = CreateDefaultSubobject<USocketIOClientComponent>(TEXT("SocketIOClientComponent"));
	SetupCallbacks();
}

// Called when the game starts or when spawned
void AClient::BeginPlay()
{
	SetupCallbacks();
	Super::BeginPlay();
	SetupCallbacks();
}

void AClient::SetupCallbacks()
{
	OnConnected = SIOClientComponent->OnConnected;
	OnDisconnected = SIOClientComponent->OnDisconnected;
	OnConnectionProblems = SIOClientComponent->OnConnectionProblems;
}

// Called every frame
void AClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClient::Emit(FString eventName, USIOJsonValue* data, FCallbackSignature callbackDelegate)
{
	callbackDelegate.BindUFunction(callbackDelegate.GetUObject(), callbackDelegate.GetFunctionName());
	callbackQueue.Enqueue(callbackDelegate);

	if (data == nullptr)
	{
		data = NewObject<USIOJsonValue>();
	}

	//Emit event "callbackTest" expecting an echo callback with the object we sent
	SIOClientComponent->EmitNative(eventName, data->AsString(), [&](const TArray<TSharedPtr<FJsonValue>>& response)
	{
		FCallbackStruct callbackStruct = FCallbackStruct();
		
		if (response.Num() >  0)
		{
			const TSharedPtr<FJsonObject> jsonObject = response[0]->AsObject();	
			callbackStruct.success = jsonObject->GetBoolField("success");
			callbackStruct.message = jsonObject->GetStringField("message");
			
			const FString dataString = jsonObject->GetStringField("data");
			callbackStruct.data = USIOJsonValue::ValueFromJsonString(nullptr, dataString);
		}
		else 
		{
			callbackStruct.success = false;
			callbackStruct.message = "No response server";
			callbackStruct.data = nullptr;
		}
		
		// Convert the struct ptr to a reference
		const FCallbackStruct& callbackStructRef = callbackStruct;

		// Call the function we passed in
		ProcessCallbackQueue(callbackStructRef);
	});
}

void AClient::ProcessCallbackQueue(const FCallbackStruct& callbackStruct)
{
	FCallbackSignature callbackDelegate;
	if (callbackQueue.Dequeue(callbackDelegate))
	{
		if (callbackDelegate.IsBound())
		{
			callbackDelegate.Execute(callbackStruct);
		}
	}
}

void AClient::BindEventTo(const FString& EventName, const FSIOJsonValueSignature& CallbackDelegate)
{
	const FSIOJsonValueSignature SafeCallback = CallbackDelegate;	//copy for lambda ref
	SIOClientComponent->OnNativeEvent(EventName, [&, SafeCallback](const FString& Event, const TSharedPtr<FJsonValue>& Message)
	{
		USIOJsonValue* Value = NewObject<USIOJsonValue>();
		TSharedPtr<FJsonValue> NonConstValue = Message;
		Value->SetRootValue(NonConstValue);

		SafeCallback.ExecuteIfBound(Value);
	});
}