using UnrealBuildTool;
public class Character_Lab : ModuleRules
{
    public Character_Lab(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "UMG", "Slate", "SlateCore", "GameplayAbilities", "GameplayTags", "GameplayTasks" });
    }
}
