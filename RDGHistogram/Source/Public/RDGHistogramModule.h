#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class RDGHISTOGRAM_API FRDGHistogramModule : public IModuleInterface
{
public:
	static inline FRDGHistogramModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FRDGHistogramModule>("RDGHistogram");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RDGHistogram");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override { return true; }
};