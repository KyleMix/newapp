using UnrealBuildTool;
using System.Collections.Generic;

public class HangoutWorldServerTarget : TargetRules
{
    public HangoutWorldServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "HangoutWorld" });
    }
}
