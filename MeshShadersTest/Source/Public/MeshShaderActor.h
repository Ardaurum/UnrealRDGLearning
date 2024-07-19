#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshShadersTestModule.h"

#include "MeshShaderActor.generated.h"

UCLASS()
class MESHSHADERSTEST_API AMeshShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshShaderActor();

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};