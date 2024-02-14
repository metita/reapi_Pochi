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

    cell *pArea = getAmxAddr(amx, params[arg_area]);

    if(!pArea)
        return FALSE;

    CNavArea *area = reinterpret_cast<CNavArea*>(params[arg_area]);
    
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
* @param entity             entity index (used for unique data)
* @param connectinfo        connect info pointer (can be 0)
* @param startArea          starting area
* @param vStart             starting vector
* @param goalarea           goal area (not needed, but should be provided)
* @param vGoal              goal vector
* @param route              route type, see RouteType
*
* @return                   connect info pointer 
*
* native ConnectInfo:rg_compute_path(const entity, ConnectInfo:cInfo, const startArea, const Float:vStart[3], goalarea, const Float:vGoal[3], RouteType:route)
*/
cell AMX_NATIVE_CALL rg_compute_path(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_entity, arg_data, arg_startarea, arg_vecstart, arg_goalarea, arg_vecgoal, arg_routetype };
    
    CAmxArgs args(amx, params);
    CBaseEntity *entity = nullptr;
    ConnectInfoData *data = nullptr;

    // if an entity was provided, do this
    if(args[arg_entity])
    {
        CHECK_ISENTITY(arg_entity);
        entity = args[arg_entity];
    }

    // cast data pointer
    cell *ptr = getAmxAddr(amx, params[arg_data]);

    if(ptr)
        data = reinterpret_cast<ConnectInfoData*>(params[arg_data]);

    // get areas, goalarea can be invalid if computepath was not provided
    cell *pArea = getAmxAddr(amx, params[arg_startarea]);

    if(!pArea)
    {
        UTIL_ServerPrint("Invalid start area provided\n");
        return reinterpret_cast<cell>(data);
    }

    CNavArea *startarea = reinterpret_cast<CNavArea*>(params[arg_startarea]);

    // invalid
    if (!startarea)
    {
        UTIL_ServerPrint("Invalid start area cast provided\n");
        return reinterpret_cast<cell>(data);
    }

    CNavArea *goalarea = nullptr;
    pArea = getAmxAddr(amx, params[arg_goalarea]);

    if(pArea)
        goalarea = reinterpret_cast<CNavArea*>(params[arg_goalarea]);

    Vector* startvec = (Vector *)getAmxAddr(amx, params[arg_vecstart]);
    Vector* goalvec = (Vector *)getAmxAddr(amx, params[arg_vecgoal]);

    RouteType route = static_cast<RouteType>(params[arg_routetype]);

    ConnectInfoData *retData;
    retData = g_ReGameFuncs->ComputePath(entity, data, startarea, startvec, goalarea, goalvec, route);
    UTIL_ServerPrint("Path computed, data: %d\n", reinterpret_cast<cell>(retData));
    return reinterpret_cast<cell>(retData);
}

/*
* Creates a connect info pointer
*
* @param                    entity (can be null)
*
* @return                   connect info pointer
*
* native rg_create_connect_info(const entity);
*/
cell AMX_NATIVE_CALL rg_create_connect_info(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_entity };    
    
    CHECK_ISENTITY(arg_entity);
    
    CAmxArgs args(amx, params);
    CBaseEntity *entity = args[arg_entity];

    ConnectInfoData *data = g_ReGameFuncs->AddConnectInfoList(entity);
    return reinterpret_cast<cell>(data);
}

/*
* Deletes a connect info from entity
* 
* @param entity             entity (can be null)
*
* @return                   true if was valid, false otherwise
*
* native rg_remove_connect_info(const entity);
*/
cell AMX_NATIVE_CALL rg_remove_connect_info(AMX* amx, cell *params)
{
    enum args_e { arg_count, arg_entity, arg_data };    
    
    CHECK_ISENTITY(arg_entity);
    
    CAmxArgs args(amx, params);
    CBaseEntity *entity = args[arg_entity];
    
    bool bDestroyed = g_ReGameFuncs->RemoveConnectInfoList(entity);

    if(bDestroyed && PARAMS_COUNT == arg_data)
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

    //CAmxArgs args(amx, params);
    cell *ptr = getAmxAddr(amx, params[arg_pointer]);

    if(!ptr)
    {
        AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: Invalid connect info provided", __FUNCTION__);
        return 0;
    }

    ConnectInfoData *data = reinterpret_cast<ConnectInfoData*>(params[arg_pointer]);
    ConnectInfoData_e data_enum = static_cast<ConnectInfoData_e>(params[arg_data_enum]);

    switch(data_enum)
    {
        case e_path:
        {
            int index;

            if(PARAMS_COUNT < arg_index)
                index = data->index;
            else
                index = params[arg_index];

            //cell *cRet = getAmxAddr(amx, params[arg_index]);

            if(index < 0 || index >= MAX_PATH_LENGTH_API)
            {
                AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: e_path->index out of bounds (%d)", __FUNCTION__, index);
                return 0;
            }

            int length = data->length;
            
            // get latest then
            if(index >= length)
                index = length - 1;
                
            cell *vecRet = getAmxAddr(amx, params[arg_return]);
            Vector pos = data->path[index].pos;

            // copy vector
            vecRet[0] = amx_ftoc(pos.x);
            vecRet[1] = amx_ftoc(pos.y);
            vecRet[2] = amx_ftoc(pos.z);
            return 1;
        }
        case e_goal:
        {
            cell *vecRet = getAmxAddr(amx, params[arg_return]);
            Vector pos = data->currentGoal;

            // copy vector
            vecRet[0] = amx_ftoc(pos.x);
            vecRet[1] = amx_ftoc(pos.y);
            vecRet[2] = amx_ftoc(pos.z);
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

    CAmxArgs args(amx, params);
    cell *ptr = getAmxAddr(amx, params[arg_pointer]);

    if(!ptr)
    {
        AMXX_LogError(amx, AMX_ERR_NATIVE, "%s: Invalid connect info provided", __FUNCTION__);
        return 0;
    }

    ConnectInfoData *data = reinterpret_cast<ConnectInfoData*>(params[arg_pointer]);
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

/*
* Check progress in current path
*
* @param entity                     entity
* @param ConnectInfoData            path pointer (can be null)
* @param tolerance                  dist tolerance
* @param check2D                    if z (up down) is ignored                        
*
* @return                           true if changed
*
* native rg_update_path_movement(const entity, const ConnectInfo:connectInfo, Float:tolerance, bool:check2D);
*/
cell AMX_NATIVE_CALL rg_update_path_movement(AMX *amx, cell *params)
{
    enum args_e {  arg_count, arg_entity, arg_data, arg_tolerance, arg_check2d     };

    CHECK_ISENTITY(arg_entity);
    
    CAmxArgs args(amx, params);
    CBaseEntity *entity = args[arg_entity];

    cell *ptr = getAmxAddr(amx, params[arg_data]);
    ConnectInfoData *data = nullptr;
    
    if(ptr)
        data = reinterpret_cast<ConnectInfoData*>(params[arg_data]);

    return (cell)g_ReGameFuncs->UpdatePathMovement(entity, data, args[arg_tolerance], args[arg_check2d]);
}

AMX_NATIVE_INFO Nav_Natives_RG[] =
{
	{ "rg_load_navigation_map",         rg_load_navigation_map          },
    { "rg_destroy_navigation_map",      rg_destroy_navigation_map       },

    { "rg_get_nearest_nav_area",        rg_get_nearest_nav_area         },
    { "rg_get_closest_point_in_area",   rg_get_closest_point_in_area    },

    { "rg_compute_path",                rg_compute_path                 },
    { "rg_update_path_movement",        rg_update_path_movement         },

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