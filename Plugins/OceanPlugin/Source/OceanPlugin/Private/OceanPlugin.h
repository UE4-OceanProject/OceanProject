#pragma once
 
#include "ModuleManager.h"
 
class OceanPluginImpl : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};