#include "ProjectCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void CProjectCore::StartupModule()
{
	UE_LOG(LogProjectCore, Log, TEXT("ProjectCore module starting up"));
}

void CProjectCore::ShutdownModule()
{
	UE_LOG(LogProjectCore, Log, TEXT("ProjectCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(CProjectCore, ProjectCore, "ProjectCore");
