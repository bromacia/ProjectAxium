#include "ScriptMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "ObjectMgr.h"
#include "PvPMgr.h"

enum TransmogOption
{
    TRANSMOG_ACTION_SHOW_INDIVIDUAL = 2,
    TRANSMOG_ACTION_SHOW_ENCHANTS,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_HEAD,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_SHOULDERS,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_CHEST,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_HANDS,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_LEGS,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_WRISTS,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_WAIST,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_FEET,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_MAINHAND,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_OFFHAND,
    TRANSMOG_ACTION_SHOW_INDIVIDUAL_RANGED,
    TRANSMOG_ACTION_SHOW_UNTRANSMOGRIFY_ITEM_TRANSMOG_OPTIONS,
    TRANSMOG_ACTION_SHOW_UNTRANSMOGRIFY_ENCHANT_TRANSMOG_OPTIONS,
    TRANSMOG_ACTION_SELECT_INDIVIDUAL,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_HEAD,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_SHOULDERS,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_CHEST,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_HANDS,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_LEGS,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_WRISTS,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_WAIST,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_FEET,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_MAINHAND,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_OFFHAND,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_RANGED,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_MAINHAND_ENCHANT,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_OFFHAND_ENCHANT,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_ALL_ARMOR,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_ALL_WEAPON,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_ALL_ENCHANT,
    TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_EVERYTHING,
};

enum TransmogEnchantId
{
    TRANSMOG_ENCHANT_MONGOOSE       = 2673,
    TRANSMOG_ENCHANT_EXECUTIONER    = 3225,
    TRANSMOG_ENCHANT_SUNFIRE        = 2671,
    TRANSMOG_ENCHANT_SOULFROST      = 2672,
    TRANSMOG_ENCHANT_BLOOD_DRAINING = 3870,
    TRANSMOG_ENCHANT_RAZORICE       = 3370,
    TRANSMOG_ENCHANT_BERSERKING     = 3789,
    TRANSMOG_ENCHANT_SPELLSURGE     = 2674,
    TRANSMOG_ENCHANT_POTENCY        = 2668,
    TRANSMOG_ENCHANT_DEATHFROST     = 3273,
    TRANSMOG_ENCHANT_SPARKLES       = 2669,
    TRANSMOG_ENCHANT_GLOW_RED       = 853,
    TRANSMOG_ENCHANT_GLOW_YELLOW    = 2567,
    TRANSMOG_ENCHANT_GLOW_BLUE      = 1897,
    TRANSMOG_ENCHANT_GLOW_PURPLE    = 1898,
    TRANSMOG_ENCHANT_GLOW_ORANGE    = 803,
    TRANSMOG_ENCHANT_GLOW_GREEN     = 2564,
    TRANSMOG_ENCHANT_GLOW_GREY      = 1900,
    TRANSMOG_ENCHANT_GLOW_WHITE     = 1894,
    TRANSMOG_ENCHANT_NO_VISUAL      = 10000
};

class Transmogrifier : public CreatureScript
{
    public:
        Transmogrifier();
        bool OnGossipHello(Player* player, Creature* creature);
        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action);
        void ShowIndividualTransmogOptions(Player* player, Creature* creature);
        void ShowTransmogEnchants(Player* player, Creature* creature);
        void ShowRemoveTransmogItemOptions(Player* player, Creature* creature);
        void ShowRemoveTransmogEnchantOptions(Player* player, Creature* creature);
        void SelectIndividualTransmog(Player* player, Creature* creature, uint16 action);
        void TransmogrifyEnchant(Player* player, Creature* creature, uint16 action);
        void UntransmogrifyItem(Player* player, Creature* creature, uint16 action);
        void UntransmogrifyEnchant(Player* player, Creature* creature, uint16 action);
        void UntransmogrifyArmor(Player* player, Creature* creature);
        void UntransmogrifyWeapons(Player* player, Creature* creature);
        void UntransmogrifyEnchants(Player* player, Creature* creature);
        void UntransmogrifyEverything(Player* player, Creature* creature);

    private:
        uint16 GetTeamById(uint8 Id)
        {
            switch (Id)
            {
                case 1: 
                    return ALLIANCE;
                case 2: 
                    return HORDE;
                default:
                    return 0;
            }
        }
        uint8 GetItemSlotByAction(uint16 action)
        {
            switch (action)
            {
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_HEAD:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_HEAD:
                    return EQUIPMENT_SLOT_HEAD;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_SHOULDERS:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_SHOULDERS:
                    return EQUIPMENT_SLOT_SHOULDERS;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_CHEST:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_CHEST:
                    return EQUIPMENT_SLOT_CHEST;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_HANDS:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_HANDS:
                    return EQUIPMENT_SLOT_HANDS;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_LEGS:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_LEGS:
                    return EQUIPMENT_SLOT_LEGS;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_WRISTS:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_WRISTS:
                    return EQUIPMENT_SLOT_WRISTS;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_WAIST:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_WAIST:
                    return EQUIPMENT_SLOT_WAIST;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_FEET:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_FEET:
                    return EQUIPMENT_SLOT_FEET;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_MAINHAND:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_MAINHAND:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_MAINHAND_ENCHANT:
                    return EQUIPMENT_SLOT_MAINHAND;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_OFFHAND:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_OFFHAND:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_OFFHAND_ENCHANT:
                    return EQUIPMENT_SLOT_OFFHAND;
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL_RANGED:
                case TRANSMOG_ACTION_SELECT_UNTRANSMOGRIFY_RANGED:
                    return EQUIPMENT_SLOT_RANGED;
                default:
                    return 0;
            }
        }
};
