﻿// Copyright 2021 Yevhenii Selivanov.

using UnrealBuildTool;

public class BomberEditor : ModuleRules
{
	public BomberEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Bomber", // Bomber Editor sources
			"Core", "CoreUObject", "Engine", "InputCore", // Default
			"EditorFramework", // FEditorDelegates::FToolkitManager
			"Slate", "SlateCore", "PropertyEditor", "EditorStyle", // Property types customizations
			"EditorWidgets" // SSearchableComboBox etc
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		//Include Public and Private folder paths
		PublicIncludePaths.AddRange(
			new[]
			{
				"BomberEditor/Public"
			});

		PrivateIncludePaths.AddRange(
			new[]
			{
				"BomberEditor/Private"
			});
	}
}
