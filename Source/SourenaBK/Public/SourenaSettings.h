#pragma once

#include "Engine/DeveloperSettings.h"

#include "SourenaSettings.generated.h"



UCLASS(config = Game, defaultconfig)
class SOURENABK_API USourenaSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	USourenaSetting();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, config)
	FString AdminClientURL;
	UPROPERTY(EditAnywhere, config)
	FString AdminClientDefaultUsername;
	UPROPERTY(EditAnywhere, config)
	FString AdminClientDefaultPassword;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config)
	FString GameClientURL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config)
	FString GameServerURL;

};