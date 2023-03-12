// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MultiplayerSurvivalEditorTarget : TargetRules
{
	public MultiplayerSurvivalEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new[] { "MultiplayerSurvival" });
	}
}