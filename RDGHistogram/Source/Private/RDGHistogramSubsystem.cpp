#include "RDGHistogramSubsystem.h"
#include "RDGHistogramSceneViewExtension.h"

void URDGHistogramSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (GetWorld()->WorldType == EWorldType::Game || GetWorld()->WorldType == EWorldType::PIE)
	{
		HistogramSceneViewExtension = FSceneViewExtensions::NewExtension<FRDGHistogramSceneViewExtension>();
	}
}

void URDGHistogramSubsystem::Deinitialize()
{
	if (HistogramSceneViewExtension)
	{
		HistogramSceneViewExtension.Reset();
		HistogramSceneViewExtension = nullptr;
	}
}
