// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(SourenaBK, Log, All);


class SOURENABK_API FSourenaBKModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	//static function to get the singlton
	static FSourenaBKModule& Get();

	FSourenaBKModule();
	~FSourenaBKModule();

};

#define SOURENA_ERROR_CODE_TIMEOUT 3
#define SOURENA_ERROR_CODE_NULL_REFRENCE 4
#define SOURENA_ERROR_CODE_NULL_CONNECTION 5
#define SOURENA_ERROR_CODE_KILLED 6




