using UnrealBuildTool;
public class Character_LabEditor : ModuleRules
{
    public Character_LabEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "NavigationSystem", "Character_Lab" });
    }
}
