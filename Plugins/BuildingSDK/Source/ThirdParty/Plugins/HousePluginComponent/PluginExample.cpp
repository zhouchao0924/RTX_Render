

#include "PluginMain.h"

extern "C" __declspec(dllexport) void* GetPluginInterface()
{
	static PluginMain  s_PluginMain;
	return &s_PluginMain;
}


