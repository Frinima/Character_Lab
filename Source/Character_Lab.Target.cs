using UnrealBuildTool;
using System.Collections.Generic;
public class Character_LabTarget : TargetRules
{
    public Character_LabTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Character_Lab");
    }
}
