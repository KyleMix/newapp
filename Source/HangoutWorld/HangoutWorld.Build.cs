using UnrealBuildTool;

public class HangoutWorld : ModuleRules
{
    public HangoutWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "NetCore",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Json",
            "JsonUtilities"
        });
    }
}
