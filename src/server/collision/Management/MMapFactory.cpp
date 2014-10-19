#include "MMapFactory.h"
#include "World.h"
#include "Config.h"
#include "DisableMgr.h"
#include <set>

namespace MMAP
{
    // ######################## MMapFactory ########################
    // our global singleton copy
    MMapManager* g_MMapManager = NULL;

    MMapManager* MMapFactory::createOrGetMMapManager()
    {
        if (g_MMapManager == NULL)
            g_MMapManager = new MMapManager();

        return g_MMapManager;
    }

    bool MMapFactory::IsPathfindingEnabled(uint32 mapId)
    {
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_MMAP, mapId, NULL))
            return false;

        return sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS);
    }

    void MMapFactory::clear()
    {
        if (g_MMapManager)
        {
            delete g_MMapManager;
            g_MMapManager = NULL;
        }
    }
}