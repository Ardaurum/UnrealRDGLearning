// Author: Ardaurum

#include "MeshShadersTestModule.h"
#include "Interfaces/IPluginManager.h"
#include "GlobalShader.h"
#include "ShaderCore.h" 

#define LOCTEXT_NAMESPACE "FMeshShadersTestModule"

void FMeshShadersTestModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MeshShadersTest"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/MeshShaders"), PluginShaderDir);
}

void FMeshShadersTestModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMeshShadersTestModule, MeshShaders)