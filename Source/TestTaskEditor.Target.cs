using UnrealBuildTool;

public class TestTaskEditorTarget : TargetRules
{
	public TestTaskEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ProjectCore" } );
	}
}
