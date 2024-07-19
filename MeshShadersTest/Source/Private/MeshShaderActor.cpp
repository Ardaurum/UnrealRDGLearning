#include "MeshShaderActor.h"
#include "Interfaces/IPluginManager.h"
#include "GlobalShader.h"
#include "ShaderCore.h" 
#include "MeshShadersTestShaders.h"
#include "RenderGraph.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ShaderParameterStruct.h"

AMeshShaderActor::AMeshShaderActor()
{
	PrimaryActorTick.bCanEverTick = true;
	Rotation = FQuat(FVector(0, 0, 0), FMath::DegreesToRadians(0.0));
}

void AMeshShaderActor::BeginPlay()
{
	Super::BeginPlay();

	if (!RenderTarget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("No Render Target"));
		this->SetActorTickEnabled(false);
	}
}

void AMeshShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(GetWorld());

	UTextureRenderTarget2D* renderTarget = RenderTarget;
	FQuat rotation = Rotation;

	ENQUEUE_RENDER_COMMAND(MeshShadersTest)(
	[
		renderTarget,
		rotation
	]
	(FRHICommandListImmediate& RHICmdList)
	{
		TShaderMapRef<FSimpleMeshShaderMS> simpleMS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		TShaderMapRef<FSimpleMeshShaderPS> simplePS(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FRDGBuilder GraphBuilder(RHICmdList);
		FSimpleMeshShaderParameters* parameters = GraphBuilder.AllocParameters<FSimpleMeshShaderParameters>();
		
		// Static projection and view matrix. We just want to rotate our mesh shaders cube.
		static const FIntPoint screenSize(renderTarget->SizeX, renderTarget->SizeY);
		static const float halfFOVRad = FMath::DegreesToRadians(45.0f);
		static const FMatrix projectionMatrix = FPerspectiveMatrix(halfFOVRad, screenSize.X, screenSize.Y, 0.01);
		static const FVector viewLocation = FVector(-2.0, 0.0, 0.0);
		static const FVector lookAt = FVector(1.0, 0.0, 0.0);
		static const FMatrix viewMatrix = FLookAtMatrix(viewLocation, lookAt, FVector(0.0, 0.0, 1.0));
		FMatrix modelMatrix = rotation.GetNormalized().ToMatrix();

		parameters->MSParameters.ModelViewProjection = FMatrix44f(modelMatrix * viewMatrix * projectionMatrix);
		FRDGTexture* renderTargetRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(renderTarget->GetRenderTargetResource()->TextureRHI, TEXT("MeshShadersTestResultRT")));
		parameters->PSParameters.RenderTargets[0] = FRenderTargetBinding(renderTargetRDG, ERenderTargetLoadAction::EClear);

		GraphBuilder.AddPass(
			RDG_EVENT_NAME("MeshShadersTest"),
			parameters,
			ERDGPassFlags::Raster,
			[simpleMS, simplePS, parameters](FRHICommandList& RHICmdList)
			{
				FGraphicsPipelineStateInitializer graphicsPSOInit;
				RHICmdList.ApplyCachedRenderTargets(graphicsPSOInit);
				graphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				graphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_InverseSourceAlpha, BO_Add, BF_Zero, BF_One>::GetRHI();
				graphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_CCW>::GetRHI();
				graphicsPSOInit.PrimitiveType = PT_TriangleList;
				graphicsPSOInit.BoundShaderState.SetMeshShader(simpleMS.GetMeshShader());
				graphicsPSOInit.BoundShaderState.PixelShaderRHI = simplePS.GetPixelShader();
				SetGraphicsPipelineState(RHICmdList, graphicsPSOInit, 0);

				SetShaderParameters(RHICmdList, simpleMS, simpleMS.GetMeshShader(), parameters->MSParameters);
				SetShaderParameters(RHICmdList, simplePS, simplePS.GetPixelShader(), parameters->PSParameters);
				RHICmdList.DispatchMeshShader(1, 1, 1);
			});

		GraphBuilder.Execute();
	});
}