#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RDGHistogramSubsystem.generated.h"

UCLASS()
class RDGHISTOGRAM_API URDGHistogramSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TSharedPtr< class FRDGHistogramSceneViewExtension, ESPMode::ThreadSafe > HistogramSceneViewExtension;
};