#include "MeshShadersTestShaders.h"

IMPLEMENT_GLOBAL_SHADER(FSimpleMeshShaderMS, "/MeshShaders/Private/SimpleMeshShader.usf", "mainMS", SF_Mesh);
IMPLEMENT_GLOBAL_SHADER(FSimpleMeshShaderPS, "/MeshShaders/Private/SimpleMeshShader.usf", "mainPS", SF_Pixel);

