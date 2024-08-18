# Unreal Engine 5.4 RDG/RHI and rendering learning project

## Installing plugins

To run the samples add the plugins to any Unreal project and enable them.

For Histogram write vis RDGHistogramRender. This will show a histogram of the current game (before post-processing).
You can also change the visualisation type by setting `r.RDGHistogram.DisplayMode {x}` where `{x}` is the number from 0-4 which corresponds to following modes:
0 - Luminance  
1 - Red  
2 - Green  
3 - Blue  
4 - Combined  

Mesh shader tests if mesh shader is supported and how to add a pass. To test it you need to create a blueprint deriving from the MeshShaderActor and then provide a RenderTarget for it. You can also edit the rotation to view the mesh 
shader cube model from different angles.

### Example video

https://github.com/user-attachments/assets/d97610d9-cc03-44f6-b1c8-eee13047f552

## RDG & RHI - Learning Notes

### Glossary

| Abbreviation | Full Name                 | Definition |
| ------------ | ------------------------- | ---------- |
| RHI          | Render Hardware Interface | Abstraction layer over platform dependent graphics API, such as DirectX, Vulkan, OpenGL |
| RDG          | Render Dependency Graph   | Graph-based API for scheduling graphic operations, which allows to specify dependencies between nodes in the graph |

### Notes

#### Extending Rendering without engine modifications - Plugin

The purpose of the project was to learn how to extend the graphic rendering capabilities of Unreal without engine modifications. We do this by creating a new plugin which allows us to add new shaders and low level code to the project. It's important to set __"LoadingPhase"__ to __"PostConfigInit"__ in the `.uplugin` file of the plugin . Without it the shader compilation will fail.

In the plugin you need to create a module that on startup adds shaders directory, e.g.:
```cpp
void FRDGHistogramModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("RDGHistogram"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/HistogramShaders"), PluginShaderDir);
}
```

#### Creating render calls

There are several ways of creating render calls depending on the needs:

#### General extensions - Compute / Custom Pass / Post-Process

- Get the renderer module and add a callback to draw in render thread
```cpp
IRendererModule* RendererModulePtr = FModuleManager::GetModulePtr<IRendererModule>("Renderer");
if (RendererModulePtr)
{
    Handle = RendererModulePtr->GetResolvedSceneColorCallbacks().AddRaw(this, &ThisModule::PostResolveSceneColor_Render);
}
```

- Extend `FSceneViewExtensionBase` and extend a method you want to add your render calls to. This I used to create the Histogram example.
  - https://github.com/Ardaurum/UnrealRDGLearning/blob/main/RDGHistogram/Source/Public/RDGHistogramSceneViewExtension.h
  - https://github.com/Ardaurum/UnrealRDGLearning/blob/main/RDGHistogram/Source/Private/RDGHistogramSceneViewExtension.cpp

- Use `ENQUEUE_RENDER_COMMAND`. Allows to create a function that will execute on render thread, but doesn't allow for better control over when it's executed. This I used to create the Mesh Shader Test example.
  - https://github.com/Ardaurum/UnrealRDGLearning/blob/main/MeshShadersTest/Source/Private/MeshShaderActor.cpp

#### Geometry extensions 

- Extend `FPrimitiveSceneProxy`. Useful for adding new custom render elements that have geometry data.
- Extend `FMeshPassProcessor`. Allows to add new ways to process already existing primitive proxies.
- You can use both previous elements to create a totally custom geometry processing pipeline!

#### Subsystem note

If you use Renderer module or `FSceneViewExtensionBase`, you are responsible for registering your render methods. What I found useful is to use `UWorldSubsystem` which gives you a good control over when you want to register and unregister your rendering system.
- https://github.com/Ardaurum/UnrealRDGLearning/blob/main/RDGHistogram/Source/Public/RDGHistogramSubsystem.h
- https://github.com/Ardaurum/UnrealRDGLearning/blob/main/RDGHistogram/Source/Private/RDGHistogramSubsystem.cpp

### Shaders

RDG tutorials go to great lengths to explain how to create shaders and how to pass data, so I recommend checking the [Additional Resources](#additional-resources)

### Engine <-> Renderer communication

Unreal uses _proxies_ or _views_ to communicate with the Renderer. The table below shows relationships between elements existing in the Engine Module to those in the Renderer Module and also on which thread they are processed.

| Engine Module <br> Game Thread | Engine Module <br> Game Thread | Renderer Module <br> Render Thread |
| ------------------- | -------------------- | ------------------- |
| UWorld              |                      | FScene              |
| UPrimitiveComponent | FPrimitiveSceneProxy | FPrimitiveSceneInfo |
|                     | FSceneView           | FViewInfo           |
| ULocalPlayer        |                      | FSceneViewState     |
| ULightComponent     | FLightSceneProxy     | FLightSceneInfo     |
| UMaterialInterface  | FMaterial            |                     |

### Additional Resources

The best example of rendering extension you can find by searching the `ColorCorrectRegions` plugin, which is a plugin bundled together with the Unreal Engine.

- https://dev.epicgames.com/documentation/en-us/unreal-engine/graphics-programming-overview-for-unreal-engine
- https://dev.epicgames.com/community/learning/knowledge-base/0ql6/unreal-engine-using-sceneviewextension-to-extend-the-rendering-system
- https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-shaders-in-plugins-unreal-engine
- https://drive.google.com/file/d/16UZsfL2KZ21YbpQBEjzm1br6CCfVYCLD/view
- https://docs.unrealengine.com/5.3/en-US/render-dependency-graph-in-unreal-engine/
- https://docs.unrealengine.com/5.3/Attachments/designing-visuals-rendering-and-graphics/rendering-programming/RenderDependencyGraph/rdg-insights-presentation.pdf
- https://unrealartoptimization.github.io/book/profiling/passes/
- https://medium.com/@lordned/unreal-engine-4-rendering-part-2-shaders-and-vertex-data-80317e1ae5f3 - !OUTDATED! There are parts that are still valid, but some parts are no longer valid, e.g. There's no `FMeshDrawingPolicy` now. Instead we have `FMeshPassProcessor`
- https://thegraphicguysquall.wordpress.com/2021/11/15/unreal-4-custom-mesh-pass-in-deferred-shading/
- https://mcro.de/c/rdg
- https://epicgames.ent.box.com/s/ul1h44ozs0t2850ug0hrohlzm53kxwrz
