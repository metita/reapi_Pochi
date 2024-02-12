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
    return (cell)g_ReGameFuncs->LoadNavigationMap();
}

/*
* Destroys map navigation file
*
* @return           true
*
* native rg_destroy_navigation_map();
*/
cell AMX_NATIVE_CALL rg_destroy_navigation_map(AMX *amx, cell *params)
{
    g_ReGameFuncs->DestroyNavigationMap();
    return TRUE;
}

/*
* Get nearest nav area
*
* @param vOrigin            Reference origin
* @param anyZ               if z (up down) is ignored
*
* @return                   nearest area (0 if it wasn't found)
*
* @native rg_get_nearest_nav_area(Float:vOrigin[3], bool:anyZ)
*/
cell AMX_NATIVE_CALL rg_get_nearest_nav_area(AMX *amx, cell *params)
{
    enum args_e { arg_count, arg_origin, arg_anyz };

    Vector* origin = (Vector *)getAmxAddr(amx, params[arg_origin]);

    CNavArea* area = g_ReGameFuncs->GetNearestNavArea(origin, params[arg_anyz]);
    return reinterpret_cast<cell>(area);
}

/*
* Get closest point in area from current origin
*
* @param pArea              Area
* @param vOrigin            Current origin
* @param vPosition          Vector Output
*
* @return                   true if area was valid, false otherwise
*
* native rg_get_closes_point_in_area(const pArea, Float:vOrigin[3], Float:vPosition[3])
*/
cell AMX_NATIVE_CALL rg_get_closest_point_in_area(AMX *amx, cell *params)
{
    enum args_e { arg_count, arg_area, arg_origin, arg_position};

    CAmxArgs args(amx, params);
    CNavArea *area = args[arg_area];

    // Invalid
    if(!area)
        return FALSE;

    Vector* origin = (Vector *)getAmxAddr(amx, params[arg_origin]);
    Vector* position = (Vector *)getAmxAddr(amx, params[arg_position]);

    g_ReGameFuncs->GetClosestPointOnArea(area, origin, position);
    return TRUE;
}

/*
* Generates a new path from current position to goal
* 
* @param connectinfo        connect info pointer (can be 0)
* @param startArea          starting area
* @param vStart             starting vector
* @param goalarea           goal area (not needed, but should be provided)
* @param vGoal              goal vector
* @param route              route type, see RouteType
* @param length             max length of the path
*
* @return                   number of paths 
*
* native ConnectInfo:rg_compute_path(cons ConnectInfo:cInfo, const startArea, const Float:vStart[3], goalarea, const Float:vGoal[3], RouteType:route)
*/
cell AMX_NATIVE_CALL rg_compute_path(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_data, arg_startarea, arg_vecstart, arg_goalarea, arg_vecgoal, arg_routetype };
    CAmxArgs args(amx, params);

    // get areas, goalarea can be invalid if computepath was not provided
    CNavArea *startarea = args[arg_startarea];
    
    // invalid
    if (!startarea)
        return 0;
    
    int length = 0;
    CNavArea *goalarea = args[arg_goalarea];
    Vector* startvec = (Vector *)getAmxAddr(amx, params[arg_vecstart]);
    Vector* goalvec = (Vector *)getAmxAddr(amx, params[arg_vecgoal]);

    RouteType route = static_cast<RouteType>(params[arg_routetype]);

    // Get pointer
    ConnectInfoData *data = nullptr;
    
    if (params[arg_data])
        data = reinterpret_cast<ConnectInfoData*>(params[arg_data]);

    data = g_ReGameFuncs->ComputePath(data, startarea, startvec, goalarea, goalvec, route);
    return reinterpret_cast<cell>(data);
}

/*
* Creates a connect info pointer
*
* @return                   connect info pointer
*
* native rg_create_connect_info();
*/
cell AMX_NATIVE_CALL rg_create_connect_info(AMX* amx, cell *params)
{
    ConnectInfoData *data = g_ReGameFuncs->AddConnectInfoList();
    return reinterpret_cast<cell>(data);
}

/*
* Deletes a connect info from the path list
* 
* @param pPath              ConnectInfo pointer
*
* @return                   true if was valid, false otherwise
*
* native rg_remove_connect_info(ConnectInfo:&pPath);
*/
cell AMX_NATIVE_CALL rg_remove_connect_info(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_data };

    ConnectInfoData *data = nullptr;
    
    if(params[arg_data])
        data = reinterpret_cast<ConnectInfoData*>(params[arg_data]);

    bool bDestroyed = g_ReGameFuncs->RemoveConnectInfoList(data);

    if(bDestroyed)
    {
        // update param to avoid errors
        cell *valRet = getAmxAddr(amx, params[arg_data]);
        *valRet = 0;
    }

    return bDestroyed ? TRUE : FALSE;
}

/*
* Destroys the connect info list, freeing memory
*
* @note This also happens when DestroyNavigationMap is called too!
* 
* native rg_destroy_connect_info_list();
*/
cell AMX_NATIVE_CALL rg_destroy_connect_info_list(AMX* amx, cell *params)
{
    g_ReGameFuncs->DestroyConnectInfoList();
    return TRUE;
}

/*
* Gets connect info data
*
* @param connectInfo                connect info pointer
* @param ConnectInfoData            see nav_api.h
* @param any                        
*
* @return                           value if was an integer
*
* native rg_get_connect_info_data(const connectInfo, ConnectInfoData_e:data, any:...)
*/
cell AMX_NATIVE_CALL rg_get_connect_info_data(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_pointer, arg_data_enum, arg_return, arg_index };

    ConnectInfoData *data = nullptr;
    
    if(params[arg_pointer])
        data = reinterpret_cast<ConnectInfoData*>(params[arg_pointer]);

    if(!data)
    {
        AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: Invalid connect info provided", __FUNCTION__);
        return 0;
    }

    ConnectInfoData_e data_enum = static_cast<ConnectInfoData_e>(params[arg_data_enum]);

    switch(data_enum)
    {
        case e_path:
        {
            if(PARAMS_COUNT < arg_index)
            {
                AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: Not enough parameters (using: %d) (needed: %d)", __FUNCTION__, PARAMS_COUNT, arg_index);
                return 0;
            }

            int index = params[arg_index];

            if(index < 0 || index > MAX_PATH_LENGTH_API)
            {
                AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: index out of bounds (%d)", __FUNCTION__, index);
                return 0;
            }

            int length = data->length;
            Vector* vecRet = (Vector *)getAmxAddr(amx, params[arg_return]);

            if(length < index)
            {
                // Avoids unitialized errors
                vecRet[0] = vecRet[1] = vecRet[2] = Vector(0, 0, 0);
                return 0;
            }

            // copy vector
            *vecRet = data->path->pos;
            return 1;
        }
        case e_length:
        {
            return (cell)data->length;
        }
        case e_index:
        {
            return (cell)data->index;
        }
        case e_update:
        {
            cell *fRet = getAmxAddr(amx, params[arg_return]);
            *fRet = *((cell*)&data->update);
            return 1;
        }
    }

    return 0;
}

/*
* Sets connect info data
*
* @param connectInfo                connect info pointer
* @param ConnectInfoData            see nav_api.h
* @param any                        
*
* @return                           true if changed
*
* @note                             Only index & update are allowed, the rest are forbidden
*
* native rg_set_connect_info_data(const connectInfo, ConnectInfoData_e, any:...)
*/
cell AMX_NATIVE_CALL rg_set_connect_info_data(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_pointer, arg_data_enum, arg_value, arg_index };

    ConnectInfoData *data = nullptr;
    
    if(params[arg_pointer])
        data = reinterpret_cast<ConnectInfoData*>(params[arg_pointer]);

    if(!data)
    {
        AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: Invalid connect info provided", __FUNCTION__);
        return 0;
    }

    ConnectInfoData_e data_enum = static_cast<ConnectInfoData_e>(params[arg_data_enum]);

    switch(data_enum)
    {
        case e_update:
        {
            data->update = *((float*)&params[arg_value]);
            return 1;
        }
        case e_index:
        {
            data->index = params[arg_value];
            return 1;
        }
        default:
        {
            AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: %d new values are forbidden", __FUNCTION__, data_enum);
            return 0;
        }
    }

    return 0;
}

AMX_NATIVE_INFO Nav_Natives_RG[] =
{
	{ "rg_load_navigation_map",         rg_load_navigation_map          },
    { "rg_destroy_navigation_map",      rg_destroy_navigation_map       },

    { "rg_get_nearest_nav_area",        rg_get_nearest_nav_area         },
    { "rg_get_closest_point_in_area",   rg_get_closest_point_in_area    },

    { "rg_compute_path",                rg_compute_path                 },
    { "rg_create_connect_info",         rg_create_connect_info          },
    { "rg_remove_connect_info",         rg_remove_connect_info          },
    { "rg_destroy_connect_info_list",   rg_destroy_connect_info_list    },

    { "rg_get_connect_info_data",       rg_get_connect_info_data        },
    { "rg_set_connect_info_data",       rg_set_connect_info_data        },

	{ nullptr, nullptr }
};

void RegisterNatives_Nav()
{
    if (!api_cfg.hasReGameDLL())
  		fillNatives(Nav_Natives_RG, [](AMX *amx, cell *params) -> cell { AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: isn't available", "ReGameDll"); return FALSE; });

    g_amxxapi.AddNatives(Nav_Natives_RG);
}