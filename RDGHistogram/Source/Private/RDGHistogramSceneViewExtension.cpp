#include "RDGHistogramSceneViewExtension.h"
#include "RDGHistogramShaders.h"
#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "DynamicResolutionState.h"
#include "EngineUtils.h"
#include "FXRenderingUtils.h"
#include "PostProcess/PostProcessInputs.h"
#include "RHI.h"
#include "SceneRenderTargetParameters.h"
#include "SceneView.h"
#include "ScreenPass.h"

static TAutoConsoleVariable<int32> CVarHistogramDisplayMode(
	TEXT("r.RDGHistogram.DisplayMode"),	0, TEXT("Changes the displey mode for RDGHistogram"));

DECLARE_GPU_STAT_NAMED(RDGHistogram, TEXT("RDGHistogram"));

FRDGHistogramSceneViewExtension::FRDGHistogramSceneViewExtension(const FAutoRegister& AutoRegister) :
	FSceneViewExtensionBase(AutoRegister)
{
}

void FRDGHistogramSceneViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	Inputs.Validate();

	const FSceneViewFamily& ViewFamily = *View.Family;
	const FIntRect PrimaryViewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(View);
	FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, PrimaryViewRect);

	if (!SceneColor.IsValid())
	{
		return;
	}

	FIntVector sceneColorDescSize = SceneColor.Texture->Desc.GetSize();
	RDG_EVENT_SCOPE(GraphBuilder, "RDGHistogram %dx%d", sceneColorDescSize.X, sceneColorDescSize.Y);
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	FUintVector2 sceneColorSize = FUintVector2(sceneColorDescSize.X, sceneColorDescSize.Y);
	uint32_t tileCountX = ceil(static_cast<float>(sceneColorSize.X) / HistogramConstants::cTileSizeX);
	uint32_t tileCountY = ceil(static_cast<float>(sceneColorSize.Y) / HistogramConstants::cTileSizeY);
	uint32_t histogramTileCount = tileCountX * tileCountY;
	FRDGBufferDesc histogramTileBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FUintVector4), histogramTileCount * HistogramConstants::cNumBins);
	FRDGBufferRef histogramTileBuffer = GraphBuilder.CreateBuffer(histogramTileBufferDesc, TEXT("HistogramTileBuffer"));

	{
		RDG_EVENT_SCOPE(GraphBuilder, "RDGHistogram TileCompute");

		FHistogramTileComputeShader::FParameters* tileParameters = GraphBuilder.AllocParameters<FHistogramTileComputeShader::FParameters>();
		tileParameters->InputTexture = SceneColor.Texture;
		tileParameters->TextureSize = sceneColorSize;
		tileParameters->HistogramTileOutput = GraphBuilder.CreateUAV(histogramTileBuffer);

		TShaderMapRef<FHistogramTileComputeShader> histogramTileCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("RDGHistogram TileCompute"),
			ERDGPassFlags::Compute,
			histogramTileCS,
			tileParameters,
			FIntVector(tileCountX, tileCountY, 1)
		);
	}

	FRDGBufferDesc histogramMergeBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FUintVector4), HistogramConstants::cNumBins);
	FRDGBufferDesc histogramMaxMergeBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FUintVector4), 1);
	FRDGBufferRef histogramMergeBuffer = GraphBuilder.CreateBuffer(histogramMergeBufferDesc, TEXT("HistogramMergeBuffer"));
	FRDGBufferRef histogramMaxMergeBuffer = GraphBuilder.CreateBuffer(histogramMaxMergeBufferDesc, TEXT("HistogramMaxMergeBuffer"));

	{
		RDG_EVENT_SCOPE(GraphBuilder, "RDGHistogram Merge");

		FHistogramMergeShader::FParameters* mergeParameters = GraphBuilder.AllocParameters<FHistogramMergeShader::FParameters>();
		mergeParameters->HistogramTileInput = GraphBuilder.CreateSRV(histogramTileBuffer);
		mergeParameters->HistogramOutput = GraphBuilder.CreateUAV(histogramMergeBuffer);
		mergeParameters->HistogramMaxOutput = GraphBuilder.CreateUAV(histogramMaxMergeBuffer);
		mergeParameters->TileCount = histogramTileCount;

		TShaderMapRef<FHistogramMergeShader> histogramMergeCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("RDGHistogram Merge"),
			ERDGPassFlags::Compute,
			histogramMergeCS,
			mergeParameters,
			FIntVector(ceil(HistogramConstants::cNumBins / FHistogramMergeShader::cNumThreads), 1, 1)
		);
	}

	{
		RDG_EVENT_SCOPE(GraphBuilder, "RDGHistogram Render");

		FUintVector2 renderTargetSize = FUintVector2(640, 640);
		FHistogramRenderShader::FParameters* renderParameters = GraphBuilder.AllocParameters<FHistogramRenderShader::FParameters>();
		renderParameters->Histogram = GraphBuilder.CreateSRV(histogramMergeBuffer);
		renderParameters->HistogramMax = GraphBuilder.CreateSRV(histogramMaxMergeBuffer);
		renderParameters->RenderMode = CVarHistogramDisplayMode.GetValueOnRenderThread();
		renderParameters->RenderTarget = GraphBuilder.CreateUAV(GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(FIntPoint(renderTargetSize.X, renderTargetSize.Y), EPixelFormat::PF_R8G8B8A8, FClearValueBinding::Transparent, ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable), TEXT("RDGHistogramRender")));
		renderParameters->RenderTargetSize = renderTargetSize;

		TShaderMapRef<FHistogramRenderShader> histogramRenderCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("RDGHistogram Render"),
			ERDGPassFlags::Compute,
			histogramRenderCS,
			renderParameters,
			FIntVector(ceil(renderTargetSize.X / FHistogramRenderShader::cNumThreadsX), ceil(renderTargetSize.Y / FHistogramRenderShader::cNumThreadsY), 1)
		);
	}
}