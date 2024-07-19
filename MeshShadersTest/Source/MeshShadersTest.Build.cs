using UnrealBuildTool;

public class MeshShadersTest : ModuleRules
{
	public MeshShadersTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;				
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Renderer",
				"RenderCore",
				"RHI",
				"Projects"
			}
		);
	}
}
