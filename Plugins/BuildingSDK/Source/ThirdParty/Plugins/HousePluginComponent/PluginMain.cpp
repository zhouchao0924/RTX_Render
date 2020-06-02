
#include "PluginMain.h"
#include "Class/Pole.h"
#include "Class/Tube.h"
#include "Class/Pillar.h"
#include "Class/Sewer.h"
#include "PoleEditor.h"
#include "TubeEditor.h"
#include "PillarEditor.h"
#include "SewerEditor.h"

PoleDesc		g_PoleDesc;
TubeDesc        g_TubeDesc;
PillarDesc      g_PillarDesc;
SewerDesc       g_SewerDesc;
PoleEditor		g_PoleEditor;
TubeEditor      g_TubeEditor;
PillarEditor    g_PillarEditor;
SewerEditor     g_SewerEditor;

IBuildingSDK	*GSDK = nullptr;

bool PluginMain::Init(IBuildingSDK *SDK)
{
	IClassLibrary *pLibaray = SDK->GetClassLibrary();
	if (pLibaray)
	{
		pLibaray->Register(&g_PoleDesc);
		pLibaray->Register(&g_TubeDesc);
		pLibaray->Register(&g_PillarDesc);
		pLibaray->Register(&g_SewerDesc);
	}
	GSDK = SDK;
	return true;
}

bool PluginMain::Tick(float DeltaTime)
{
	return true;
}

void PluginMain::UnInitialize()
{
	GSDK = nullptr;
}

void *PluginMain::QueryInterface(const char *Name)
{
	if (Name)
	{
		std::string InterfaceName = Name;
		if (InterfaceName == "PoleEditor")
		{
			return &g_PoleEditor;
		}
		else if (InterfaceName == "TubeEditor")
		{
			return &g_TubeEditor;
		}
		else if (InterfaceName == "PillarEditor")
		{
			return &g_PillarEditor;
		}
		else if (InterfaceName=="SewerEditor")
		{
			return &g_SewerEditor;
		}
	}
	return nullptr;
}


