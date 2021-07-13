using UnrealBuildTool;

public class TestTaskTarget : TargetRules
{
	public TestTaskTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ProjectCore" } );
	}
}
