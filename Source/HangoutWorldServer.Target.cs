using UnrealBuildTool;
using System.Collections.Generic;

public class HangoutWorldServerTarget : TargetRules
{
    public HangoutWorldServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange(new string[] { "HangoutWorld" });
    }
}
