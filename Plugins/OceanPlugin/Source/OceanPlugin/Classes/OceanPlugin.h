/*=================================================
* FileName: OceanPlugin.h
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/03/20
*
* Last Edited on: 2015/08/09
* Last Edited by: DotCam
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#pragma once
 
#include "ModuleManager.h"
 
class OceanPluginImpl : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};
