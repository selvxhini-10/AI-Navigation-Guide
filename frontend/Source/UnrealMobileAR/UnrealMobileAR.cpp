// Copyright © 2024 AR App. All Rights Reserved.

#include "UnrealMobileAR.h"
#include "Modules/ModuleManager.h"

class FUnrealMobileARModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		// This code will execute after your module is loaded into memory
	}

	virtual void ShutdownModule() override
	{
		// This function may be called during shutdown to clean up your module
	}
};

IMPLEMENT_MODULE(FUnrealMobileARModule, UnrealMobileAR)
