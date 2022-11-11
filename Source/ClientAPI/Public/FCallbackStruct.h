#pragma once
#include "SIOJsonValue.h"

#include "FCallbackStruct.generated.h"

USTRUCT(BlueprintType)
struct FCallbackStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool success;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USIOJsonValue* data;
};
