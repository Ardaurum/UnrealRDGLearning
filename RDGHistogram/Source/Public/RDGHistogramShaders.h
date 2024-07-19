#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterUtils.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"

namespace HistogramConstants
{
	static constexpr uint32_t cNumBins = 256;
	static constexpr uint32_t cTileSizeX = 32;
	static constexpr uint32_t cTileSizeY = 16;
}

class FHistogramTileComputeShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FHistogramTileComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FHistogramTileComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FUintVector4>, HistogramTileOutput)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER(FUintVector2, TextureSize)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("NUM_BINS"), HistogramConstants::cNumBins);
		OutEnvironment.SetDefine(TEXT("TILE_SIZE_X"), HistogramConstants::cTileSizeX);
		OutEnvironment.SetDefine(TEXT("TILE_SIZE_Y"), HistogramConstants::cTileSizeY);
	}
};

class FHistogramMergeShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FHistogramMergeShader);
	SHADER_USE_PARAMETER_STRUCT(FHistogramMergeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(uint32, TileCount)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FUintVector4>, HistogramTileInput)
		SHADER_PARAMETER_RDG_BUFFER_UAV(StructuredBuffer<FUintVector4>, HistogramOutput)
		SHADER_PARAMETER_RDG_BUFFER_UAV(StructuredBuffer<FUintVector4>, HistogramMaxOutput)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("NUM_BINS"), HistogramConstants::cNumBins);
		OutEnvironment.SetDefine(TEXT("NUM_THREADS_X"), cNumThreads);
	}

	constexpr static uint32_t cNumThreads = 64;
};

class FHistogramRenderShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FHistogramRenderShader);
	SHADER_USE_PARAMETER_STRUCT(FHistogramRenderShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FUintVector4>, Histogram)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FUintVector4>, HistogramMax)
		SHADER_PARAMETER(uint32, RenderMode)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, RenderTarget)
		SHADER_PARAMETER(FUintVector2, RenderTargetSize)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("NUM_BINS"), HistogramConstants::cNumBins);
		OutEnvironment.SetDefine(TEXT("NUM_THREADS_X"), cNumThreadsX);
		OutEnvironment.SetDefine(TEXT("NUM_THREADS_Y"), cNumThreadsX);
	}

	constexpr static uint32_t cNumThreadsX = 8;
	constexpr static uint32_t cNumThreadsY = 8;
};
