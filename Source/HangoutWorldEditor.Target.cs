using UnrealBuildTool;
using System.Collections.Generic;

public class HangoutWorldEditorTarget : TargetRules
{
    public HangoutWorldEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("HangoutWorld");
    }
}
