// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketIOClientComponent.h"
#include "Structs/FCallbackStruct.h"
#include "SIOJsonValue.h"
#include "Client.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FCallbackSignature, FCallbackStruct, callbackStruct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConnectionProblemsSignature, int32, Attempts, int32,  NextAttemptInMs, float, TimeSinceConnected);


UCLASS()
class CLIENTAPI_API AClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetupCallbacks();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable) 
	void Emit(FString eventName, USIOJsonValue* data, FCallbackSignature callbackDelegate); 

	UFUNCTION(BlueprintCallable, Category = "SocketIO Functions")
	void BindEventTo(const FString& EventName,
					const FSIOJsonValueSignature& CallbackDelegate);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SocketIO")
	USocketIOClientComponent* SIOClientComponent;

	UPROPERTY(BlueprintAssignable, Category = "SocketIO Events")
	FSIOCOpenEventSignature OnConnected;
	
	UPROPERTY(BlueprintAssignable, Category = "SocketIO Events")
	FSIOCCloseEventSignature OnDisconnected;
	
	UPROPERTY(BlueprintAssignable, Category = "SocketIO Events")
	FSIOConnectionProblemSignature OnConnectionProblems;
	

private:
	UPROPERTY(EditAnywhere, Category = "SocketIO")
	FString serverIP;
	UPROPERTY(EditAnywhere, Category = "SocketIO")
	FString serverPort;
	UPROPERTY(EditAnywhere, Category = "SocketIO")
	FString type;
	
	TQueue <FCallbackSignature> callbackQueue;
	void ProcessCallbackQueue(const FCallbackStruct&);
};
