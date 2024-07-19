#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class MESHSHADERSTEST_API FMeshShadersTestModule : public IModuleInterface
{
public:
	static inline FMeshShadersTestModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMeshShadersTestModule>("MeshShadersTestModule");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("MeshShadersTestModule");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override { return true; }
};