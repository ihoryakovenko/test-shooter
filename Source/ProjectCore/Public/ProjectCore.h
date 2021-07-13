#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class CProjectCore : public IModuleInterface
{
public:
	static inline CProjectCore & Get()
	{
		return FModuleManager::LoadModuleChecked<CProjectCore>("ProjectCore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ProjectCore");
	}

	void StartupModule() override;
	void ShutdownModule() override;
};
