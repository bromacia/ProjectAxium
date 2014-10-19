#include "ScriptPCH.h"

enum TeleportOption
{
    TELEPORT_MALL = 2,
    TELEPORT_DUEL,
    TELEPORT_GURUBASHI,
};

class Teleporter : public CreatureScript
{
    public:
        Teleporter() : CreatureScript("Teleporter") {}

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->PlayerTalkClass->ClearMenus();

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_boss_zuramat:30|t Mall", GOSSIP_SENDER_MAIN, TELEPORT_MALL);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_zone_howlingfjord_03:30|t Dueling Zone", GOSSIP_SENDER_MAIN, TELEPORT_DUEL);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_armorkit_14:30|t Gurubashi Arena", GOSSIP_SENDER_MAIN, TELEPORT_GURUBASHI);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
        {
            switch (action)
            {
                case TELEPORT_MALL:
                {
                    player->RemoveAura(SPELL_GURUBASHI_BANISH);
                    player->TeleportTo(558, 72.5f, -163.0f, 15.5f, 3.1f);
                    break;
                }
                case TELEPORT_DUEL:
                {
                    player->RemoveAura(SPELL_GURUBASHI_BANISH);
                    player->TeleportTo(571, 524.0f, -3237.0f, 27.5f, 3.3f);
                    break;
                }
                case TELEPORT_GURUBASHI:
                {
                    player->Dismount();
                    player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                    player->RemoveAllControlled();
                    player->AddAura(SPELL_GURUBASHI_BANISH, player);
                    player->TeleportTo(0, -13315.0f, 80.0f, 22.15f, 1.0f);
                    break;
                }
            }

            return true;
        }
};

void AddSC_Teleporter()
{
    new Teleporter();
}
