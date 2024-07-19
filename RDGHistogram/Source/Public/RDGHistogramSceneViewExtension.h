#pragma once
#include "SceneViewExtension.h"
#include "RHI.h"
#include "RHIResources.h"

class UColorCorrectRegionsSubsystem;
class UMaterialInterface;
class FRDGTexture;

class FRDGHistogramSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FRDGHistogramSceneViewExtension(const FAutoRegister& AutoRegister);
	
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
};
	