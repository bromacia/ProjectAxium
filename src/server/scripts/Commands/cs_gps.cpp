/* ScriptData
Name: gps_commandscript
%Complete: 100
Comment: GPS/WPGPS commands
Category: commandscripts
EndScriptData */

#include "ObjectAccessor.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "CellImpl.h"
#include "MMapFactory.h"

class gps_commandscript : public CommandScript
{
public:
    gps_commandscript() : CommandScript("gps_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand commandTable[] =
        {
            { "gps",      SEC_GAMEMASTER,     false, &HandleGPSCommand,                  "", NULL },
            { "wpgps",    SEC_GAMEMASTER,     false, &HandleWPGPSCommand,                "", NULL },
            { NULL,       0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleGPSCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* object = NULL;
        if (*args)
        {
            uint64 guid = handler->extractGuidFromLink((char*)args);
            if (guid)
                object = (WorldObject*)ObjectAccessor::GetObjectByTypeMask(*handler->GetSession()->GetPlayer(), guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);

            if (!object)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            object = handler->getSelectedUnit();

            if (!object)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        CellCoord cellCoord = Axium::ComputeCellCoord(object->GetPositionX(), object->GetPositionY());
        Cell cell(cellCoord);

        uint32 zoneId, areaId;
        object->GetZoneAndAreaId(zoneId, areaId);
        uint32 mapId = object->GetMapId();

        MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
        AreaTableEntry const* zoneEntry = GetAreaEntryByAreaID(zoneId);
        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(areaId);

        float zoneX = object->GetPositionX();
        float zoneY = object->GetPositionY();

        Map2ZoneCoordinates(zoneX, zoneY, zoneId);

        Map const* map = object->GetMap();
        float groundZ = map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), MAX_HEIGHT);
        float floorZ = map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ());

        GridCoord gridCoord = Axium::ComputeGridCoord(object->GetPositionX(), object->GetPositionY());

        // Im guessing 63 has something to do with grid calc
        int gridX = 63 - gridCoord.x_coord;
        int gridY = 63 - gridCoord.y_coord;

        uint32 haveMap = Map::ExistMap(mapId, gridX, gridY) ? 1 : 0;
        uint32 haveVMap = Map::ExistVMap(mapId, gridX, gridY) ? 1 : 0;
        uint32 haveMMap = (MMAP::MMapFactory::IsPathfindingEnabled(mapId) && MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId())) ? 1 : 0;

        if (haveVMap)
        {
            if (map->IsOutdoors(object->GetPositionX(), object->GetPositionY(), object->GetPositionZ()))
                handler->PSendSysMessage("You are outdoors");
            else
                handler->PSendSysMessage("You are indoors");
        }
        else
            handler->PSendSysMessage("no VMAP available for area info");

        handler->PSendSysMessage("Map: %u (%s) Zone: %u (%s) Area: %u (%s) Phase: %u",
            mapId, (mapEntry ? mapEntry->name[handler->GetSessionDbcLocale()] : "<unknown>"),
            zoneId, (zoneEntry ? zoneEntry->area_name[handler->GetSessionDbcLocale()] : "<unknown>"),
            areaId, (areaEntry ? areaEntry->area_name[handler->GetSessionDbcLocale()] : "<unknown>"),
            object->GetPhaseMask());
        handler->PSendSysMessage("X: %f Y: %f Z: %f Orientation: %f", object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), object->GetOrientation());
        handler->PSendSysMessage("grid[%u,%u]cell[%u,%u] InstanceID: %u ZoneX: %f ZoneY: %f", cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), object->GetInstanceId(), zoneX, zoneY);
        handler->PSendSysMessage("GroundZ: %f FloorZ: %f Have height data (Map: %u VMap: %u MMap: %u)", groundZ, floorZ, haveMap, haveVMap, haveMMap);

        LiquidData liquidStatus;
        ZLiquidStatus status = map->getLiquidStatus(object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), MAP_ALL_LIQUIDS, &liquidStatus);

        if (status)
            handler->PSendSysMessage(LANG_LIQUID_STATUS, liquidStatus.level, liquidStatus.depth_level, liquidStatus.entry, liquidStatus.type_flags, status);

        return true;
    }

    static bool HandleWPGPSCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        sLog->outSQLDev("(@PATH, XX, %.3f, %.3f, %.5f, 0,0, 0,100, 0),", player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        handler->PSendSysMessage("Waypoint SQL written to SQL Developer log");
        return true;
    }
};

void AddSC_gps_commandscript()
{
    new gps_commandscript();
}
