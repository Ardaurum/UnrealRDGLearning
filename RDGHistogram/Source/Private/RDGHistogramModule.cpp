// Author: Ardaurum

#include "RDGHistogramModule.h"
#include "Interfaces/IPluginManager.h"
#include "GlobalShader.h"
#include "ShaderCore.h" 
#include "RDGHistogramSceneViewExtension.h"

#define LOCTEXT_NAMESPACE "FRDGHistogramModule"

void FRDGHistogramModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("RDGHistogram"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/HistogramShaders"), PluginShaderDir);
}

void FRDGHistogramModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRDGHistogramModule, RDGHistogram)