#include "RDGHistogramShaders.h"

IMPLEMENT_GLOBAL_SHADER(FHistogramTileComputeShader, "/HistogramShaders/Private/HistogramTileCompute.usf", "main", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FHistogramMergeShader, "/HistogramShaders/Private/HistogramMerge.usf", "main", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FHistogramRenderShader, "/HistogramShaders/Private/HistogramRender.usf", "main", SF_Compute);