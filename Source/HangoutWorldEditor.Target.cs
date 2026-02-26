using UnrealBuildTool;
using System.Collections.Generic;

public class HangoutWorldEditorTarget : TargetRules
{
    public HangoutWorldEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
        ExtraModuleNames.Add("HangoutWorld");
    }
}
