using UnrealBuildTool;
using System.Collections.Generic;
public class Character_LabEditorTarget : TargetRules
{
    public Character_LabEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new[] { "Character_Lab", "Character_LabEditor" });
    }
}
