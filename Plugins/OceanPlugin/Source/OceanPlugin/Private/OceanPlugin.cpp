// OceanPlugin version

#include "OceanPluginPrivatePCH.h"

class FOceanPlugin : public IOceanPlugin {

	/** IModuleInterface implementation */
	virtual void StartupModule() override {

		}

	virtual void ShutdownModule() override {

		}
	};


IMPLEMENT_MODULE(FOceanPlugin, OceanPlugin)

//DEFINE_LOG_CATEGORY(LogOcean);