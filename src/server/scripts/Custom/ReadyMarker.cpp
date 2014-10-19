#include "ScriptPCH.h"
#include "Battleground.h"
#include "Chat.h"

class ReadyMarker : public GameObjectScript
{
    public:
        ReadyMarker() : GameObjectScript("ReadyMarker") {}

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if (Battleground* bg = player->GetBattleground())
                bg->AddEarlyStartPlayer(player->GetGUID());

            return true;
        }
};

void AddSC_ReadyMarker()
{
    new ReadyMarker();
}
