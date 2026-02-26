using UnrealBuildTool;
using System.Collections.Generic;

public class HangoutWorldTarget : TargetRules
{
    public HangoutWorldTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("HangoutWorld");
    }
}
