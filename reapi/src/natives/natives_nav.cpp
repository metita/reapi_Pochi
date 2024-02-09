#include "precompiled.h"

/*
* Loads map navigation file
*
* @return          NavErrorType, see nav.h
*
* native rg_load_navigation_map();
*/
cell AMX_NATIVE_CALL rg_load_navigation_map(AMX *amx, cell *params)
{
    return (cell)g_ReGameBotFuncs->LoadNavigationMap();
}

AMX_NATIVE_INFO Nav_Natives_RG[] =
{
	{ "rg_load_navigation_map",      rg_load_navigation_map      },

	{ nullptr, nullptr }
};

void RegisterNatives_Nav()
{
    if (!api_cfg.hasReGameDLL())
		fillNatives(Nav_Natives_RG, [](AMX *amx, cell *params) -> cell { AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: isn't available", "ReGameDll"); return FALSE; });

    g_amxxapi.AddNatives(Nav_Natives_RG);
}