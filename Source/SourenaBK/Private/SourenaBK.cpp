// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SourenaBK.h"


void FSourenaBKModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module LoadStates();
}

void FSourenaBKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//SaveStates();
}

FSourenaBKModule& FSourenaBKModule::Get()
{
	return FModuleManager::LoadModuleChecked<FSourenaBKModule>("SourenaBK");
}


IMPLEMENT_MODULE(FSourenaBKModule, SourenaBK)


DEFINE_LOG_CATEGORY(SourenaBK)



FSourenaBKModule::FSourenaBKModule()
{

}

FSourenaBKModule::~FSourenaBKModule()
{

}

