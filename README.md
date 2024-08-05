To run the samples add the plugins to any Unreal project and enable them.

For Histogram write vis RDGHistogramRender. This will show a histogram of the current game (before post-processing).
You can also change the visualisation type by setting `r.RDGHistogram.DisplayMode {x}` where `{x}` is the number from 0-4 which corresponds to following modes:
0 - Luminance
1 - Red
2 - Green
3 - Blue
4 - Combined

Mesh shader tests if mesh shader is supported and how to add a pass. To test it you need to create a blueprint deriving from the MeshShaderActor and then provide a RenderTarget for it. You can also edit the rotation to view the mesh shader cube model from different angles.

https://github.com/user-attachments/assets/d4bb4723-239f-453e-8a34-df1f71133b71

