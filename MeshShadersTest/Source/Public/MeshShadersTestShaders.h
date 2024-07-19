#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterUtils.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"

class FSimpleMeshShaderMS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSimpleMeshShaderMS);
	SHADER_USE_PARAMETER_STRUCT(FSimpleMeshShaderMS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FMatrix44f, ModelViewProjection)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

class FSimpleMeshShaderPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FSimpleMeshShaderPS);
	SHADER_USE_PARAMETER_STRUCT(FSimpleMeshShaderPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

BEGIN_SHADER_PARAMETER_STRUCT(FSimpleMeshShaderParameters, )
	SHADER_PARAMETER_STRUCT_INCLUDE(FSimpleMeshShaderMS::FParameters, MSParameters)
	SHADER_PARAMETER_STRUCT_INCLUDE(FSimpleMeshShaderPS::FParameters, PSParameters)
END_SHADER_PARAMETER_STRUCT()