// Copyright 2021 Yevhenii Selivanov.

using UnrealBuildTool;

public class BomberTarget : TargetRules
{
    public BomberTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange(new[] {"Bomber"});
    }
}
