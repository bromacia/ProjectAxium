#include "ScriptMgr.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PointMovementGenerator.h"
#include "MMapFactory.h"
#include "Map.h"
#include "TargetedMovementGenerator.h"
#include "PathFinderMovementGenerator.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

class mmaps_commandscript : public CommandScript
{
    public:
        mmaps_commandscript() : CommandScript("mmaps_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand mmapsCommandTable[] =
            {
                { "path",           SEC_GAMEMASTER,         false, &HandleMmapsPathCommand,             "", NULL },
                { "loc",            SEC_GAMEMASTER,         false, &HandleMmapsLocCommand,              "", NULL },
                { "loadedtiles",    SEC_GAMEMASTER,         false, &HandleMmapsLoadedTilesCommand,      "", NULL },
                { "stats",          SEC_GAMEMASTER,         false, &HandleMmapsStatsCommand,            "", NULL },
                { "testarea",       SEC_GAMEMASTER,         false, &HandleMmapsTestArea,                "", NULL },
                { "movebypath",     SEC_GAMEMASTER,         false, &HandleMmapsMoveByPath,              "", NULL },
                { NULL,             0,                      false, NULL,                                "", NULL }
            };

            static ChatCommand commandTable[] =
            {
                { "mmaps",          SEC_GAMEMASTER,         true,  NULL,                  "", mmapsCommandTable  },
                { NULL,             0,                      false, NULL,                                "", NULL }
            };
            return commandTable;
        }

        static bool HandleMmapsPathCommand(ChatHandler* handler, char const* args)
        {
            if (!MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId()))
            {
                handler->PSendSysMessage("NavMesh not loaded for current map.");
                return true;
            }

            handler->PSendSysMessage("mmap path:");

            Player* player = handler->GetSession()->GetPlayer();
            Unit* target = handler->getSelectedUnit();
            if (!player || !target)
            {
                handler->PSendSysMessage("Invalid target/source selection.");
                return true;
            }

            char* para = strtok((char*)args, " ");

            bool useStraightPath = false;
            if (para && strcmp(para, "true") == 0)
                useStraightPath = true;

            float x, y, z;
            target->GetPosition(x, y, z);

            PathFinderMovementGenerator path(player);
            path.SetUseStrightPath(useStraightPath);
            bool result = path.Calculate(x, y, z);

            Movement::PointsArray const& pointPath = path.GetPath();
            handler->PSendSysMessage("%s's path to %s:", player->GetName(), target->GetName());
            handler->PSendSysMessage("Building: %s", useStraightPath ? "StraightPath" : "SmoothPath");
            handler->PSendSysMessage("Result: %s - Length: " SIZEFMTD " - Type: %u", (result ? "true" : "false"), pointPath.size(), path.GetPathType());

            G3D::Vector3 const &start = path.GetStartPosition();
            G3D::Vector3 const &end = path.GetEndPosition();
            G3D::Vector3 const &actualEnd = path.GetActualEndPosition();

            handler->PSendSysMessage("StartPosition     (%.3f, %.3f, %.3f)", start.x, start.y, start.z);
            handler->PSendSysMessage("EndPosition       (%.3f, %.3f, %.3f)", end.x, end.y, end.z);
            handler->PSendSysMessage("ActualEndPosition (%.3f, %.3f, %.3f)", actualEnd.x, actualEnd.y, actualEnd.z);

            if (!player->HasGameMasterTagOn())
                handler->PSendSysMessage("Enable GM mode to see the path points.");

            for (uint32 i = 0; i < pointPath.size(); ++i)
                player->SummonCreature(VISUAL_WAYPOINT, pointPath[i].x, pointPath[i].y, pointPath[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 9000);

            return true;
        }

        static bool HandleMmapsLocCommand(ChatHandler* handler, char const* /*args*/)
        {
            handler->PSendSysMessage("mmap tileloc:");

            // grid tile location
            Player* player = handler->GetSession()->GetPlayer();

            int32 gx = 32 - player->GetPositionX() / SIZE_OF_GRIDS;
            int32 gy = 32 - player->GetPositionY() / SIZE_OF_GRIDS;

            handler->PSendSysMessage("%03u%02i%02i.mmtile", player->GetMapId(), gy, gx);
            handler->PSendSysMessage("Grid Location [%i, %i]", gy, gx);

            // calculate navmesh tile location
            dtNavMesh const* navmesh = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(handler->GetSession()->GetPlayer()->GetMapId());
            dtNavMeshQuery const* navmeshquery = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMeshQuery(handler->GetSession()->GetPlayer()->GetMapId(), player->GetInstanceId());
            if (!navmesh || !navmeshquery)
            {
                handler->PSendSysMessage("NavMesh not loaded for current map.");
                return true;
            }

            float const* min = navmesh->getParams()->orig;
            float x, y, z;
            player->GetPosition(x, y, z);
            float location[VERTEX_SIZE] = {y, z, x};
            float extents[VERTEX_SIZE] = {3.0f, 5.0f, 3.0f};

            int32 tilex = int32((y - min[0]) / SIZE_OF_GRIDS);
            int32 tiley = int32((x - min[2]) / SIZE_OF_GRIDS);

            handler->PSendSysMessage("Tile [%02i, %02i]", tiley, tilex);

            // Navmesh Poly -> Navmesh Tile location
            dtQueryFilter filter = dtQueryFilter();
            dtPolyRef polyRef = INVALID_POLYREF;
            navmeshquery->findNearestPoly(location, extents, &filter, &polyRef, NULL);

            if (polyRef == INVALID_POLYREF)
                handler->PSendSysMessage("dtPolyRef [??, ??] (Invalid Poly, probably no Tile loaded)");
            else
            {
                dtMeshTile const* tile;
                dtPoly const* poly;
                navmesh->getTileAndPolyByRef(polyRef, &tile, &poly);
                if (tile)
                    handler->PSendSysMessage("dtMeshTile [%02i, %02i]", tile->header->y, tile->header->x);
                else
                    handler->PSendSysMessage("dtMeshTile [??, ??] (No Tile loaded)");
            }

            return true;
        }

        static bool HandleMmapsLoadedTilesCommand(ChatHandler* handler, char const* /*args*/)
        {
            uint32 mapid = handler->GetSession()->GetPlayer()->GetMapId();
            dtNavMesh const* navmesh = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMesh(mapid);
            dtNavMeshQuery const* navmeshquery = MMAP::MMapFactory::createOrGetMMapManager()->GetNavMeshQuery(mapid, handler->GetSession()->GetPlayer()->GetInstanceId());
            if (!navmesh || !navmeshquery)
            {
                handler->PSendSysMessage("NavMesh not loaded for current map.");
                return true;
            }

            handler->PSendSysMessage("mmap loadedtiles:");

            for (int32 i = 0; i < navmesh->getMaxTiles(); ++i)
            {
                dtMeshTile const* tile = navmesh->getTile(i);
                if (!tile || !tile->header)
                    continue;

                handler->PSendSysMessage("[%02i, %02i]", tile->header->x, tile->header->y);
            }

            return true;
        }

        static bool HandleMmapsStatsCommand(ChatHandler* handler, char const* /*args*/)
        {
            uint32 mapId = handler->GetSession()->GetPlayer()->GetMapId();
            handler->PSendSysMessage("mmap stats:");
            handler->PSendSysMessage("global mmap pathfinding is %sabled", MMAP::MMapFactory::IsPathfindingEnabled(mapId) ? "en" : "dis");

            MMAP::MMapManager* manager = MMAP::MMapFactory::createOrGetMMapManager();
            handler->PSendSysMessage("%u maps loaded with %u tiles overall", manager->getLoadedMapsCount(), manager->getLoadedTilesCount());

            uint32 tileCount = 0;
            uint32 nodeCount = 0;
            uint32 polyCount = 0;
            uint32 vertCount = 0;
            uint32 triCount = 0;
            uint32 triVertCount = 0;
            uint32 dataSize = 0;
            for (MMAP::MMapDataSet::iterator i = manager->loadedMMaps.begin(); i != manager->loadedMMaps.end(); ++i)
            {
                dtNavMesh const* navmesh = i->second->navMesh;
                if (!navmesh)
                    continue;

                for (int32 i = 0; i < navmesh->getMaxTiles(); ++i)
                {
                    dtMeshTile const* tile = navmesh->getTile(i);
                    if (!tile || !tile->header)
                        continue;

                    tileCount++;
                    nodeCount += tile->header->bvNodeCount;
                    polyCount += tile->header->polyCount;
                    vertCount += tile->header->vertCount;
                    triCount += tile->header->detailTriCount;
                    triVertCount += tile->header->detailVertCount;
                    dataSize += tile->dataSize;
                }
            }

            handler->PSendSysMessage("Navmesh stats:");
            handler->PSendSysMessage(" %u tiles loaded", tileCount);
            handler->PSendSysMessage(" %u BVTree nodes", nodeCount);
            handler->PSendSysMessage(" %u polygons (%u vertices)", polyCount, vertCount);
            handler->PSendSysMessage(" %u triangles (%u vertices)", triCount, triVertCount);
            handler->PSendSysMessage(" %.2f MB of data (not including pointers)", ((float)dataSize / sizeof(unsigned char)) / 1048576);

            return true;
        }

        static bool HandleMmapsTestArea(ChatHandler* handler, char const* /*args*/)
        {
            float radius = 40.0f;
            WorldObject* object = handler->GetSession()->GetPlayer();

            CellCoord pair(Axium::ComputeCellCoord(object->GetPositionX(), object->GetPositionY()));
            Cell cell(pair);
            cell.SetNoCreate();

            std::list<Creature*> creatureList;

            Axium::AnyUnitInObjectRangeCheck go_check(object, radius);
            Axium::CreatureListSearcher<Axium::AnyUnitInObjectRangeCheck> go_search(object, creatureList, go_check);
            TypeContainerVisitor<Axium::CreatureListSearcher<Axium::AnyUnitInObjectRangeCheck>, GridTypeMapContainer> go_visit(go_search);

            // Get Creatures
            cell.Visit(pair, go_visit, *(object->GetMap()), *object, radius);

            if (!creatureList.empty())
            {
                handler->PSendSysMessage("Found " SIZEFMTD " Creatures.", creatureList.size());

                uint32 paths = 0;
                uint32 uStartTime = getMSTime();

                float gx, gy, gz;
                object->GetPosition(gx, gy, gz);
                for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                {
                    PathFinderMovementGenerator path(*itr);
                    path.Calculate(gx, gy, gz);
                    ++paths;
                }

                uint32 uPathLoadTime = getMSTimeDiff(uStartTime, getMSTime());
                handler->PSendSysMessage("Generated %i paths in %i ms", paths, uPathLoadTime);
            }
            else
                handler->PSendSysMessage("No creatures in %f yard range.", radius);

            return true;
        }

        static bool HandleMmapsMoveByPath(ChatHandler* handler, char const* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            Unit* target = handler->getSelectedUnit();
            if (!target)
            {
                handler->PSendSysMessage("No target selected");
                return true;
            }

            char* speedArg = strtok((char*)args, " ");
            float speed = 0.0f;
            if (speedArg)
                speed = atof(speedArg);

            char* reverseArg = strtok(NULL, " ");
            bool reverse = false;
            if (reverseArg && strcmp(reverseArg, "true") == 0)
                reverse = true;

            if (reverse)
            {
                float x, y, z;
                player->GetPosition(x, y, z);

                PathFinderMovementGenerator path(target);
                path.Calculate(x, y, z);
                Movement::MoveSplineInit init(*target);
                init.MovebyPath(path.GetPath());
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }
            else
            {
                float x, y, z;
                target->GetPosition(x, y, z);

                PathFinderMovementGenerator path(player);
                path.Calculate(x, y, z);
                Movement::MoveSplineInit init(*player);
                init.MovebyPath(path.GetPath());
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }
            return true;
        }
};

void AddSC_mmaps_commandscript()
{
    new mmaps_commandscript();
}
