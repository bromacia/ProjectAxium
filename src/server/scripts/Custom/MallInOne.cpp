#include "MallInOne.h"

bool MallInOne::OnGossipHello(Player* player, Creature* creature)
{
    bool IsHunter = player->getClass() == CLASS_HUNTER ? true : false;

    player->PlayerTalkClass->ClearMenus();

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_crate_01:30|t General Goods", GOSSIP_SENDER_MAIN, MALL_MENU_GENERAL_GOODS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_warrior_improveddisciplines:30|t Gear", GOSSIP_SENDER_MAIN, MALL_MENU_GEAR);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_gizmo_gnomishflameturret:30|t Professions", GOSSIP_SENDER_MAIN, MALL_MENU_PROFESSIONS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_rod_enchantedadamantite:30|t Enchants", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelcrafting_pyrestone_02:30|t Gems", GOSSIP_SENDER_MAIN, MALL_MENU_GEMS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_mount_bigblizzardbear:30|t Mounts", GOSSIP_SENDER_MAIN, MALL_MENU_MOUNTS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_inscription_minorglyph00:30|t Glyphs", GOSSIP_SENDER_MAIN, MALL_MENU_GLYPHS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_bannerpvp_03:30|t Guild", GOSSIP_SENDER_MAIN, MALL_MENU_GUILD);

    if (IsHunter)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_hunter_beasttaming:30|t Pets", GOSSIP_SENDER_MAIN, MALL_MENU_PETS);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_box_petcarrier_01:30|t Stable", GOSSIP_SENDER_MAIN, MALL_MENU_STABLE);
    }

    if (player->getClass() == CLASS_ROGUE && player->HasSpell(51713))
        if (player->HasAura(51713) && player->GetAura(51713, player->GetGUID())->GetDuration() == -1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_reputation_01:30|t I'm Finished", GOSSIP_SENDER_MAIN, MALL_MENU_FINISH_SHADOW_DANCE);
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_rogue_shadowdance:30|t Setup Shadow Dance Bar", GOSSIP_SENDER_MAIN, MALL_MENU_SETUP_SHADOW_DANCE);

    if (player->GetSpecsCount() < 2)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_general:30|t Learn Dual Specialization", GOSSIP_SENDER_MAIN, MALL_MENU_LEARN_DUAL_SPEC);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\spell_arcane_mindmastery:30|t Reset Talents", GOSSIP_SENDER_MAIN, MALL_MENU_RESET_TALENTS);

    if (IsHunter)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_hunter_longevity:30|t Reset Pet Talents", GOSSIP_SENDER_MAIN, MALL_MENU_RESET_PET_TALENTS);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool MallInOne::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    switch (sender)
    {
        case GOSSIP_SENDER_MAIN:
        {
            switch (action)
            {
                case GOSSIP_SENDER_MAIN:            OnGossipHello(player, creature);                               break;
                case MALL_MENU_GENERAL_GOODS:       ShowInventory(player, creature, INVENTORY_LIST_GENERAL_GOODS); break;
                case MALL_MENU_GEAR:                HandleGear(player, creature, MALL_MENU_GEAR);                  break;
                case MALL_MENU_PROFESSIONS:         HandleProfessions(player, creature);                           break;
                case MALL_MENU_ENCHANTS:            HandleEnchants(player, creature, MALL_MENU_ENCHANTS);          break;
                case MALL_MENU_GEMS:                ShowInventory(player, creature, INVENTORY_LIST_GEMS);          break;
                case MALL_MENU_MOUNTS:              ShowInventory(player, creature, INVENTORY_LIST_MOUNTS);        break;
                case MALL_MENU_GLYPHS:              ShowInventory(player, creature, INVENTORY_LIST_GLYPHS);        break;
                case MALL_MENU_GUILD:               HandleGuild(player, creature, MALL_MENU_GUILD);                break;
                case MALL_MENU_PETS:                ShowPets(player, creature, MALL_MENU_PETS);                    break;
                case MALL_MENU_STABLE:              HandleStable(player, creature);                                break;
                case MALL_MENU_SETUP_SHADOW_DANCE:  HandleSetupShadowDanceBar(player, creature);                   break;
                case MALL_MENU_FINISH_SHADOW_DANCE: HandleFinishShadowDanceBar(player, creature);                  break;
                case MALL_MENU_LEARN_DUAL_SPEC:     HandleLearnDualSpecialization(player, creature);               break;
                case MALL_MENU_RESET_TALENTS:       HandleResetTalents(player, creature);                          break;
                case MALL_MENU_RESET_PET_TALENTS:   HandleResetPetTalents(player);                                 break;
            }
            break;
        }
        case MALL_MENU_GEAR:
        {
            switch (action)
            {
                case MALL_GEAR_RELENTLESS_GLADIATOR:  HandleGear(player, creature, MALL_GEAR_RELENTLESS_GLADIATOR);  break;
                case MALL_GEAR_TRIAL_OF_THE_CRUSADER: HandleGear(player, creature, MALL_GEAR_TRIAL_OF_THE_CRUSADER); break;
                case MALL_GEAR_WRATHFUL_GLADIATOR:    HandleGear(player, creature, MALL_GEAR_WRATHFUL_GLADIATOR);    break;
                case MALL_GEAR_ICECROWN_CITADEL:      HandleGear(player, creature, MALL_GEAR_ICECROWN_CITADEL);      break;
                case MALL_GEAR_APPAREL:               HandleGear(player, creature, MALL_GEAR_APPAREL);               break;
            }
            break;
        }
        case MALL_MENU_PROFESSIONS:
        {
            LearnSkill(player, creature, action);
            break;
        }
        case MALL_MENU_ENCHANTS:
        {
            HandleEnchants(player, creature, action);
            break;
        }
        case MALL_GEAR_RELENTLESS_GLADIATOR:
        {
            switch (action)
            {
                case GEAR_OPTION_MAINSETS_AND_OFFSETS: ShowInventory(player, creature, INVENTORY_LIST_RELENTLESS_GLADIATOR_MAINSETS_AND_OFFSETS); break;
                case GEAR_OPTION_ACCESSORIES:          ShowInventory(player, creature, INVENTORY_LIST_RELENTLESS_GLADIATOR_ACCESSORIES);          break;
                case GEAR_OPTION_WEAPONS:              ShowInventory(player, creature, INVENTORY_LIST_RELENTLESS_GLADIATOR_WEAPONS);              break;
                case GEAR_OPTION_TRINKETS:             ShowInventory(player, creature, INVENTORY_LIST_245_TRINKETS);                              break;
            }
            break;
        }
        case MALL_GEAR_TRIAL_OF_THE_CRUSADER:
        {
            switch (action)
            {
                case GEAR_OPTION_MAINSETS_AND_OFFSETS: SelectArmorType(player, creature, sender);                                                          break;
                case GEAR_OPTION_CLOTH:                ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_CLOTH);   break;
                case GEAR_OPTION_LEATHER:              ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_LEATHER); break;
                case GEAR_OPTION_MAIL:                 ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_MAIL);    break;
                case GEAR_OPTION_PLATE:                ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_PLATE);   break;
                case GEAR_OPTION_ACCESSORIES:          ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_ACCESSORIES);                  break;
                case GEAR_OPTION_WEAPONS:              ShowInventory(player, creature, INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_WEAPONS);                      break;
                case GEAR_OPTION_TRINKETS:             ShowInventory(player, creature, INVENTORY_LIST_245_TRINKETS);                                       break;
            }
            break;
        }
        case MALL_GEAR_WRATHFUL_GLADIATOR:
        {
            switch (action)
            {
                case GEAR_OPTION_MAINSETS_AND_OFFSETS: ShowInventory(player, creature, INVENTORY_LIST_WRATHFUL_GLADIATOR_MAINSETS_AND_OFFSETS); break;
                case GEAR_OPTION_ACCESSORIES:          ShowInventory(player, creature, INVENTORY_LIST_WRATHFUL_GLADIATOR_ACCESSORIES);          break;
                case GEAR_OPTION_WEAPONS:              ShowInventory(player, creature, INVENTORY_LIST_WRATHFUL_GLADIATOR_WEAPONS);              break;
                case GEAR_OPTION_TRINKETS:             ShowInventory(player, creature, INVENTORY_LIST_264_TRINKETS);                            break;
            }
            break;
        }
        case MALL_GEAR_ICECROWN_CITADEL:
        {
            switch (action)
            {
                case GEAR_OPTION_MAINSETS_AND_OFFSETS: SelectArmorType(player, creature, sender);                                                     break;
                case GEAR_OPTION_CLOTH:                ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_CLOTH);   break;
                case GEAR_OPTION_LEATHER:              ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_LEATHER); break;
                case GEAR_OPTION_MAIL:                 ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_MAIL);    break;
                case GEAR_OPTION_PLATE:                ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_PLATE);   break;
                case GEAR_OPTION_ACCESSORIES:          ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_ACCESSORIES);                  break;
                case GEAR_OPTION_WEAPONS:              ShowInventory(player, creature, INVENTORY_LIST_ICECROWN_CITADEL_WEAPONS);                      break;
                case GEAR_OPTION_TRINKETS:             ShowInventory(player, creature, INVENTORY_LIST_264_TRINKETS);                                  break;
            }
            break;
        }
        case ENCHANT_OPTION_HEAD:
        case ENCHANT_OPTION_SHOULDERS:
        case ENCHANT_OPTION_CHEST:
        case ENCHANT_OPTION_GLOVES:
        case ENCHANT_OPTION_LEGS:
        case ENCHANT_OPTION_BRACERS:
        case ENCHANT_OPTION_BELT:
        case ENCHANT_OPTION_BOOTS:
        case ENCHANT_OPTION_CLOAK:
        case ENCHANT_OPTION_RING1:
        case ENCHANT_OPTION_RING2:
        case ENCHANT_OPTION_MAIN_HAND:
        case ENCHANT_OPTION_OFF_HAND:
        case ENCHANT_OPTION_RANGED:
        {
            EnchantItem(player, creature, action, sender);
            break;
        }
        case MALL_MENU_PETS:
        {
            ShowPets(player, creature, action);
            break;
        }
        case PET_OPTION_CUNNING:
        case PET_OPTION_FEROCITY:
        case PET_OPTION_TENACITY:
        {
            CreatePet(player, creature, action);
            break;
        }
        case MALL_MENU_GUILD:
        {
            HandleGuild(player, creature, action);
            break;
        }
    }

    return true;
}

bool MallInOne::HandleGear(Player* player, Creature* creature, uint32 gearOption)
{
    switch (gearOption)
    {
        case MALL_MENU_GEAR:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_helmet_147:30|t Relentless Gladiator", MALL_MENU_GEAR, MALL_GEAR_RELENTLESS_GLADIATOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_gauntlets_78a:30|t Trial of the Crusader", MALL_MENU_GEAR, MALL_GEAR_TRIAL_OF_THE_CRUSADER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_cloth_83:30|t Wrathful Gladiator", MALL_MENU_GEAR, MALL_GEAR_WRATHFUL_GLADIATOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_shoulder_116black:30|t Icecrown Citadel", MALL_MENU_GEAR, MALL_GEAR_ICECROWN_CITADEL);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_shirt_12:30|t Apparel", MALL_MENU_GEAR, MALL_GEAR_APPAREL);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case MALL_GEAR_RELENTLESS_GLADIATOR:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_cloth_74:30|t Mainsets & Offsets", MALL_GEAR_RELENTLESS_GLADIATOR, GEAR_OPTION_MAINSETS_AND_OFFSETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_ring_60:30|t Accessories", MALL_GEAR_RELENTLESS_GLADIATOR, GEAR_OPTION_ACCESSORIES);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_mace_111:30|t Weapons", MALL_GEAR_RELENTLESS_GLADIATOR, GEAR_OPTION_WEAPONS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_necklace_37:30|t Trinkets", MALL_GEAR_RELENTLESS_GLADIATOR, GEAR_OPTION_TRINKETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_GEAR);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case MALL_GEAR_TRIAL_OF_THE_CRUSADER:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_cloth_75:30|t Mainsets & Offsets", MALL_GEAR_TRIAL_OF_THE_CRUSADER, GEAR_OPTION_MAINSETS_AND_OFFSETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_necklace_45:30|t Accessories", MALL_GEAR_TRIAL_OF_THE_CRUSADER, GEAR_OPTION_ACCESSORIES);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_sword_142:30|t Weapons", MALL_GEAR_TRIAL_OF_THE_CRUSADER, GEAR_OPTION_WEAPONS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_scarab_crystal:30|t Trinkets", MALL_GEAR_TRIAL_OF_THE_CRUSADER, GEAR_OPTION_TRINKETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_GEAR);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case MALL_GEAR_WRATHFUL_GLADIATOR:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_leather_24:30|t Mainsets & Offsets", MALL_GEAR_WRATHFUL_GLADIATOR, GEAR_OPTION_MAINSETS_AND_OFFSETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_necklace_36:30|t Accessories", MALL_GEAR_WRATHFUL_GLADIATOR, GEAR_OPTION_ACCESSORIES);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_axe_115:30|t Weapons", MALL_GEAR_WRATHFUL_GLADIATOR, GEAR_OPTION_WEAPONS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_trinketpvp_02:30|t Trinkets", MALL_GEAR_WRATHFUL_GLADIATOR, GEAR_OPTION_TRINKETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_GEAR);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case MALL_GEAR_ICECROWN_CITADEL:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_plate_25:30|t Mainsets & Offsets", MALL_GEAR_ICECROWN_CITADEL, GEAR_OPTION_MAINSETS_AND_OFFSETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_ring_81:30|t Accessories", MALL_GEAR_ICECROWN_CITADEL, GEAR_OPTION_ACCESSORIES);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_axe_112:30|t Weapons", MALL_GEAR_ICECROWN_CITADEL, GEAR_OPTION_WEAPONS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_trinket_04:30|t Trinkets", MALL_GEAR_ICECROWN_CITADEL, GEAR_OPTION_TRINKETS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_GEAR);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case MALL_GEAR_APPAREL:
        {
            ShowInventory(player, creature, INVENTORY_LIST_APPAREL);
            break;
        }
    }

    return true;
}

bool MallInOne::SelectArmorType(Player* player, Creature* creature, uint32 sender)
{
    if (player->HasSpell(9078))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_cloth_21:30|t Cloth", sender, GEAR_OPTION_CLOTH);

    if (player->HasSpell(9077))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_leather_09:30|t Leather", sender, GEAR_OPTION_LEATHER);

    if (player->HasSpell(8737))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_chain_05:30|t Mail", sender, GEAR_OPTION_MAIL);

    if (player->HasSpell(750))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_chest_plate01:30|t Plate", sender, GEAR_OPTION_PLATE);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", MALL_MENU_GEAR, sender);
    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool MallInOne::HandleProfessions(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_alchemy:30|t Alchemy", MALL_MENU_PROFESSIONS, SKILL_ALCHEMY);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_blacksmithing:30|t Blacksmithing", MALL_MENU_PROFESSIONS, SKILL_BLACKSMITHING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_engraving:30|t Enchanting", MALL_MENU_PROFESSIONS, SKILL_ENCHANTING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_engineering:30|t Engineering", MALL_MENU_PROFESSIONS, SKILL_ENGINEERING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_herbalism:30|t Herbalism", MALL_MENU_PROFESSIONS, SKILL_HERBALISM);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_inscription_tradeskill01:30|t Inscription", MALL_MENU_PROFESSIONS, SKILL_INSCRIPTION);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_gem_01:30|t Jewelcrafting", MALL_MENU_PROFESSIONS, SKILL_JEWELCRAFTING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_armorkit_17:30|t Leatherworking", MALL_MENU_PROFESSIONS, SKILL_LEATHERWORKING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\trade_tailoring:30|t Tailoring", MALL_MENU_PROFESSIONS, SKILL_TAILORING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool MallInOne::HandleEnchants(Player* player, Creature* creature, uint32 enchantOption)
{
    switch (enchantOption)
    {
        case MALL_MENU_ENCHANTS:
        {
            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platehelm:30|t Head", MALL_MENU_ENCHANTS, ENCHANT_OPTION_HEAD);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plateshoulder:30|t Shoulders", MALL_MENU_ENCHANTS, ENCHANT_OPTION_SHOULDERS);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platechest:30|t Chest", MALL_MENU_ENCHANTS, ENCHANT_OPTION_CHEST);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plategloves:30|t Gloves", MALL_MENU_ENCHANTS, ENCHANT_OPTION_GLOVES);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platepants:30|t Legs", MALL_MENU_ENCHANTS, ENCHANT_OPTION_LEGS);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platebracer:30|t Bracers", MALL_MENU_ENCHANTS, ENCHANT_OPTION_BRACERS);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platebelt:30|t Belt", MALL_MENU_ENCHANTS, ENCHANT_OPTION_BELT);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plateboots:30|t Boots", MALL_MENU_ENCHANTS, ENCHANT_OPTION_BOOTS);

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_cape_11:30|t Cloak", MALL_MENU_ENCHANTS, ENCHANT_OPTION_CLOAK);

            if (player->GetSkillValue(SKILL_ENCHANTING) >= 450)
            {
                if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_ring_22:30|t Ring 1", MALL_MENU_ENCHANTS, ENCHANT_OPTION_RING1);

                if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_jewelry_ring_22:30|t Ring 2", MALL_MENU_ENCHANTS, ENCHANT_OPTION_RING2);
            }

            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_sword_04:30|t Main Hand", MALL_MENU_ENCHANTS, ENCHANT_OPTION_MAIN_HAND);

            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                if (item->GetTemplate()->InventoryType != INVTYPE_HOLDABLE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_shield_04:30|t Off Hand", MALL_MENU_ENCHANTS, ENCHANT_OPTION_OFF_HAND);

            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                if (item->GetTemplate()->InventoryType != INVTYPE_RELIC && item->GetTemplate()->SubClass != ITEM_SUBCLASS_WEAPON_WAND)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_weapon_crossbow_04:30|t Ranged", MALL_MENU_ENCHANTS, ENCHANT_OPTION_RANGED);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_HEAD:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your head slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Attack Power & +20 Resilience", ENCHANT_OPTION_HEAD, ENCHANT_HEAD_ARCANUM_OF_TRIUMPH);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+29 Spell Power & +20 Resilience", ENCHANT_OPTION_HEAD, ENCHANT_HEAD_ARCANUM_OF_DOMINANCE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Attack Power & +20 Critical Strike", ENCHANT_OPTION_HEAD, ENCHANT_HEAD_ARCANUM_OF_TORMENT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "30 Spell Power & +20 Critical Strike", ENCHANT_OPTION_HEAD, ENCHANT_HEAD_ARCANUM_OF_BURNING_MYSTERIES);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "30 Spell Power & +10 MP5", ENCHANT_OPTION_HEAD, ENCHANT_HEAD_ARCANUM_OF_BLISSFUL_MENDING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_SHOULDERS:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your shoulder slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Attack Power & +15 Resilience", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDER_INSCRIPTION_OF_TRIUMPH);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+23 Spell Power & +15 Resilience", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDER_INSCRIPTION_OF_DOMINANCE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Attack Power & +15 Critical Strike", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_AXE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+24 Spell Power & +15 Critical Strike", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_STORM);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+24 Spell Power & +8 MP5", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_CRAG);

            if (player->GetSkillValue(SKILL_INSCRIPTION) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+120 Attack Power & +15 Critical Strike", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_AXE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+70 Spell Power & +15 Critical Strike", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_STORM);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+70 Spell Power & +8 MP5", ENCHANT_OPTION_SHOULDERS, ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_CRAG);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_CHEST:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your chest slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+20 Resilience", ENCHANT_OPTION_CHEST, ENCHANT_CHEST_EXCEPTIONAL_RESILIENCE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+10 Stats", ENCHANT_OPTION_CHEST, ENCHANT_CHEST_POWERFUL_STATS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+275 Health", ENCHANT_OPTION_CHEST, ENCHANT_CHEST_SUPER_HEALTH);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_GLOVES:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your hands slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+44 Attack Power", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_CRUSHER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+28 Spell Power", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_EXCEPTIONAL_SPELLPOWER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+20 Agility", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_MAJOR_AGILITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+20 Hit Rating", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_PERCISION);

            if (player->GetSkillValue(SKILL_BLACKSMITHING) >= 450)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Socket Gloves", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_SOCKET);

            if (player->GetSkillValue(SKILL_ENGINEERING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hand-Mounted Pyro Rocket", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_HAND_MOUNTED_PRYO_ROCKET);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hyperspeed Accelerators", ENCHANT_OPTION_GLOVES, ENCHANT_GLOVES_HYPERSPEED_ACCELERATORS);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_LEGS:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your legs slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+75 Attack Power & +22 Critical Strike", ENCHANT_OPTION_LEGS, ENCHANT_LEGS_ICESCALE_LEG_ARMOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Spell Power & +30 Stamina", ENCHANT_OPTION_LEGS, ENCHANT_LEGS_SAPPHIRE_SPELLTHREAD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Spell Power & +20 Spirit", ENCHANT_OPTION_LEGS, ENCHANT_LEGS_BRILLIANT_SPELLTHREAD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+55 Stamina & +22 Agility", ENCHANT_OPTION_LEGS, ENCHANT_LEGS_FROSTHIDE_LEG_ARMOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Resilience & +28 Stamina", ENCHANT_OPTION_LEGS, ENCHANT_LEGS_EARTHEN_LEG_ARMOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_BRACERS:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your wrists slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Attack Power", ENCHANT_OPTION_BRACERS, ENCHANT_BRACERS_GREATER_ASSAULT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+30 Spell Power", ENCHANT_OPTION_BRACERS, ENCHANT_BRACERS_SUPERIOR_SPELLPOWER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+16 Intellect", ENCHANT_OPTION_BRACERS, ENCHANT_BRACERS_EXCEPTIONAL_INTELLECT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+18 Spirit", ENCHANT_OPTION_BRACERS, ENCHANT_BRACERS_MAJOR_SPIRIT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Stamina", ENCHANT_OPTION_BRACERS, ENCHANT_BRACERS_MAJOR_STAMINA);

            if (player->GetSkillValue(SKILL_BLACKSMITHING) >= 450)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Socket Bracer", ENCHANT_OPTION_BRACERS, ENCHANT_BRACER_SOCKET);

            if (player->GetSkillValue(SKILL_LEATHERWORKING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+130 Attack Power", ENCHANT_OPTION_BRACERS, ENCHANT_BRACER_FUR_LINING_ATTACKPOWER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+76 Spell Power", ENCHANT_OPTION_BRACERS, ENCHANT_BRACER_FUR_LINING_SPELLPOWER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+57 Agility", ENCHANT_OPTION_BRACERS, ENCHANT_BRACER_FUR_LINING_AGILITY);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+102 Stamina", ENCHANT_OPTION_BRACERS, ENCHANT_BRACER_FUR_LINING_STAMINA);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_BELT:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your waist slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Eternal Belt Buckle", ENCHANT_OPTION_BELT, ENCHANT_BELT_ETERNAL_BELT_BUCKLE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_BOOTS:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your feet slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+32 Attack Power", ENCHANT_OPTION_BOOTS, ENCHANT_BOOTS_GREATER_ASSULT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+16 Agility", ENCHANT_OPTION_BOOTS, ENCHANT_BOOTS_SUPERIOR_AGILITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+12 Hit Rating & +12 Critical", ENCHANT_OPTION_BOOTS, ENCHANT_BOOTS_ICEWALKER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+15 Stamina & +8% Movement Speed", ENCHANT_OPTION_BOOTS, ENCHANT_BOOTS_TUSKARR_VITALITY);

            if (player->GetSkillValue(SKILL_ENGINEERING) >= 450)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nitro Boosts", ENCHANT_OPTION_BOOTS, ENCHANT_BOOTS_NITRO_BOOSTS);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_CLOAK:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your back slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+35 Spell Penetration", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_SPELL_PIERCING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+22 Agility", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_MAJOR_AGILITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+22 Haste", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_GREATER_SPEED);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+10 Agility & +1 Steath Level", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_SHADOW_ARMOR);

            if (player->GetSkillValue(SKILL_ENGINEERING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Flexweave Underlay", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_FLEXWEAVE_UNDERLAY);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Springy Arachnoweave", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE);
            }

            if (player->GetSkillValue(SKILL_TAILORING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swordguard Embroidery", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lightweave Embroidery", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Darkglow Embroidery", ENCHANT_OPTION_CLOAK, ENCHANT_CLOAK_DARKGLOW_EMBROIDERY);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_RING1:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your finger1 slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (player->GetSkillValue(SKILL_ENCHANTING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Attack Power", ENCHANT_OPTION_RING1, ENCHANT_RING_ASSAULT);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+23 Spell Power", ENCHANT_OPTION_RING1, ENCHANT_RING_GREATER_SPELLPOWER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+30 Stamina", ENCHANT_OPTION_RING1, ENCHANT_RING_STAMINA);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
                player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            }
            break;
        }
        case ENCHANT_OPTION_RING2:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your finger2 slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (player->GetSkillValue(SKILL_ENCHANTING) >= 450)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Attack Power", ENCHANT_OPTION_RING2, ENCHANT_RING_ASSAULT);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+23 Spell Power", ENCHANT_OPTION_RING2, ENCHANT_RING_GREATER_SPELLPOWER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+30 Stamina", ENCHANT_OPTION_RING2, ENCHANT_RING_STAMINA);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
                player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            }
            break;
        }
        case ENCHANT_OPTION_MAIN_HAND:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your main hand slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            const ItemTemplate* pItemTemplate = pItem->GetTemplate();
            if (!pItemTemplate)
            {
                player->SendSysMessage("Unable to get template data for the item in your main hand slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (pItemTemplate->InventoryType == INVTYPE_2HWEAPON)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+110 Attack Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_2H_WEAPON_MASSACRE);

            if (pItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_STAFF)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+81 Spell Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_STAFF_GREATER_SPELLPOWER);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Berserking", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_BERSERKING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Black Magic", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_BLACK_MAGIC);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Draining", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_BLOOD_DRAINING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blade Warding", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_BLADE_WARDING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Titanium Weapon Chain", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_TITANIUM_WEAPON_CHAIN);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+65 Attack Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_SUPERIOR_POTENCY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+63 Spell Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_MIGHTY_SPELLPOWER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+26 Agility", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_EXCEPTIONAL_AGILITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+45 Spirit", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_EXCEPTIONAL_SPIRIT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Stamina", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_TITANGUARD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+25 Hit Rating & +25 Critical Strike", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_ACCURACY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mongoose", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_MONGOOSE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Executioner", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_EXECUTIONER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Deathfrost", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_DEATHFROST);

            if (player->getClass() == CLASS_DEATH_KNIGHT)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fallen Crusader", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_FALLEN_CRUSADER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cinderglacier", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_CINDERGLACIER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razorice", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_RAZORICE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lichbane", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_LICHBANE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Spellbreaking", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_SPELLBREAKING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Spellshattering", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_SPELLSHATTERING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swordbreaking", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_SWORDBREAKING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swordshattering", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_SWORDSHATTERING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nerubian Carapace", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_NERUBIAN_CARAPACE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stoneskin Gargoyle", ENCHANT_OPTION_MAIN_HAND, ENCHANT_WEAPON_STONESKIN_GARGOYLE);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_OFF_HAND:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your off hand slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            const ItemTemplate* pItemTemplate = pItem->GetTemplate();
            if (!pItemTemplate)
            {
                player->SendSysMessage("Unable to get template data for the item in your off hand slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (pItemTemplate->InventoryType == INVTYPE_HOLDABLE)
            {
                player->SendSysMessage("Incorrect weapon type equipped");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (pItemTemplate->InventoryType == INVTYPE_2HWEAPON)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+110 Attack Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_2H_WEAPON_MASSACRE);

            if (pItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_STAFF)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+81 Spell Power", ENCHANT_OPTION_MAIN_HAND, ENCHANT_STAFF_GREATER_SPELLPOWER);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Berserking", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_BERSERKING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Black Magic", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_BLACK_MAGIC);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Draining", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_BLOOD_DRAINING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blade Warding", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_BLADE_WARDING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Titanium Weapon Chain", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_TITANIUM_WEAPON_CHAIN);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+65 Attack Power", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_SUPERIOR_POTENCY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+63 Spell Power", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_MIGHTY_SPELLPOWER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+26 Agility", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_EXCEPTIONAL_AGILITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+45 Spirit", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_EXCEPTIONAL_SPIRIT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+50 Stamina", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_TITANGUARD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+25 Hit Rating & +25 Critical Strike", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_ACCURACY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mongoose", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_MONGOOSE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Executioner", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_EXECUTIONER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Deathfrost", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_DEATHFROST);

            if (player->getClass() == CLASS_DEATH_KNIGHT)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fallen Crusader", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_FALLEN_CRUSADER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cinderglacier", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_CINDERGLACIER);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razorice", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_RAZORICE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lichbane", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_LICHBANE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Spellbreaking", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_SPELLBREAKING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Spellshattering", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_SPELLSHATTERING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swordbreaking", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_SWORDBREAKING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swordshattering", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_SWORDSHATTERING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nerubian Carapace", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_NERUBIAN_CARAPACE);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stoneskin Gargoyle", ENCHANT_OPTION_OFF_HAND, ENCHANT_WEAPON_STONESKIN_GARGOYLE);
            }

            if (pItemTemplate->InventoryType == INVTYPE_SHIELD)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Titanium Plating", ENCHANT_OPTION_OFF_HAND, ENCHANT_SHIELD_TITANIUM_PLATING);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+25 Intellect", ENCHANT_OPTION_OFF_HAND, ENCHANT_SHIELD_GREATER_INTELLECT);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+12 Resilience", ENCHANT_OPTION_OFF_HAND, ENCHANT_SHIELD_RESILIENCE);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case ENCHANT_OPTION_RANGED:
        {
            Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
            if (!pItem)
            {
                player->SendSysMessage("You have no item equipped in your ranged slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            const ItemTemplate* pItemTemplate = pItem->GetTemplate();
            if (!pItemTemplate)
            {
                player->SendSysMessage("Unable to get template data for the item in your off hand slot");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            if (pItemTemplate->InventoryType == INVTYPE_RELIC || pItemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            {
                player->SendSysMessage("Incorrect weapon type equipped");
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Ranged Critical Strike", ENCHANT_OPTION_RANGED, ENCHANT_RANGED_HEARTSEEKER_SCOPE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+40 Ranged Haste", ENCHANT_OPTION_RANGED, ENCHANT_RANGED_SUN_SCOPE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "+15 Ranged Weapon Damage", ENCHANT_OPTION_RANGED, ENCHANT_RANGED_DIAMOND_CUT_REFRACTOR_SCOPE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_ENCHANTS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
    }
    return true;
}

bool MallInOne::HandleGuild(Player* player, Creature* creature, uint32 guildOption)
{
    switch (guildOption)
    {
        case MALL_MENU_GUILD:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_misc_groupneedmore:30|t Create a Guild", MALL_MENU_GUILD, GUILD_OPTION_CREATE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_shirt_guildtabard_01:30|t Edit Tabard Design", MALL_MENU_GUILD, GUILD_OPTION_DESIGN);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case GUILD_OPTION_CREATE:
        {
            player->CLOSE_GOSSIP_MENU();

            WorldPacket data(SMSG_PETITION_SHOWLIST, 33);
            data << creature->GetGUID();
            data << uint8(1);                                                   // count
            data << uint32(1);                                                  // index
            data << uint32(5863);                                               // charter entry
            data << uint32(16161);                                              // charter display id
            data << uint32(0);                                                  // charter cost
            data << uint32(0);                                                  // unknown
            data << uint32(sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS));    // required signs?
            player->GetSession()->SendPacket(&data);
            break;
        }
        case GUILD_OPTION_DESIGN:
        {
            player->CLOSE_GOSSIP_MENU();

            WorldPacket data(MSG_TABARDVENDOR_ACTIVATE, 8);
            data << creature->GetGUID();
            player->GetSession()->SendPacket(&data);
            break;
        }
    }

    return true;
}

bool MallInOne::HandleStable(Player* player, Creature* creature)
{
    player->CLOSE_GOSSIP_MENU();
    player->GetSession()->SendStablePet(creature->GetGUID());
    return true;
}

bool MallInOne::ShowPets(Player* player, Creature* creature, uint32 petOption)
{
    switch (petOption)
    {
        case MALL_MENU_PETS:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_hunter_combatexperience:30|t Create Cunning Pet", MALL_MENU_PETS, PET_OPTION_CUNNING);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_druid_swipe:30|t Create Ferocity Pet", MALL_MENU_PETS, PET_OPTION_FEROCITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Bear:30|t Create Tenacity Pet", MALL_MENU_PETS, PET_OPTION_TENACITY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case PET_OPTION_CUNNING:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Bat:30|t Bat", PET_OPTION_CUNNING, PET_CUNNING_BAT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Owl:30|t Bird of Prey", PET_OPTION_CUNNING, PET_CUNNING_BIRD_OF_PREY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Chimera:30|t Chimaera", PET_OPTION_CUNNING, PET_CUNNING_CHIMAERA);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_DragonHawk:30|t Dragonhawk", PET_OPTION_CUNNING, PET_CUNNING_DRAGONHAWK);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_NetherRay:30|t Nether Ray", PET_OPTION_CUNNING, PET_CUNNING_NETHER_RAY);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Ravager:30|t Ravager", PET_OPTION_CUNNING, PET_CUNNING_RAVAGER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\spell_Nature_GuardianWard:30|t Serpent", PET_OPTION_CUNNING, PET_CUNNING_SERPENT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Silithid:30|t Silithid", PET_OPTION_CUNNING, PET_CUNNING_SILITHID);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Spider:30|t Spider", PET_OPTION_CUNNING, PET_CUNNING_SPIDER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Sporebat:30|t Spore Bat", PET_OPTION_CUNNING, PET_CUNNING_SPORE_BAT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_WindSerpent:30|t Wind Serpent", PET_OPTION_CUNNING, PET_CUNNING_WIND_SERPENT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_PETS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case PET_OPTION_FEROCITY:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_racial_bearform:30|t Arcturis", PET_OPTION_FEROCITY, PET_FEROCITY_ARCTURIS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Cat:30|t Cat", PET_OPTION_FEROCITY, PET_FEROCITY_CAT);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Vulture:30|t Carrion Bird", PET_OPTION_FEROCITY, PET_FEROCITY_CARRION_BIRD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_CoreHound:30|t Core Hound", PET_OPTION_FEROCITY, PET_FEROCITY_CORE_HOUND);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Devilsaur:30|t Devilsaur", PET_OPTION_FEROCITY, PET_FEROCITY_DEVILSAUR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_druid_catform:30|t Gondria", PET_OPTION_FEROCITY, PET_FEROCITY_GONDRIA);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Hyena:30|t Hyena", PET_OPTION_FEROCITY, PET_FEROCITY_HYENA);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_druid_healinginstincts:30|t Loque'nahak", PET_OPTION_FEROCITY, PET_FEROCITY_LOQUENAHAK);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Moth:30|t Moth", PET_OPTION_FEROCITY, PET_FEROCITY_MOTH);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Raptor:30|t Raptor", PET_OPTION_FEROCITY, PET_FEROCITY_RAPTOR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\spell_shaman_feralspirit:30|t Skoll", PET_OPTION_FEROCITY, PET_FEROCITY_SKOLL);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_TallStrider:30|t Tallstrider", PET_OPTION_FEROCITY, PET_FEROCITY_TALLSTRIDER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Wasp:30|t Wasp", PET_OPTION_FEROCITY, PET_FEROCITY_WASP);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Wolf:30|t Wolf", PET_OPTION_FEROCITY, PET_FEROCITY_WOLF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_PETS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
        case PET_OPTION_TENACITY:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Bear:30|t Bear", PET_OPTION_TENACITY, PET_TENACITY_BEAR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Boar:30|t Boar", PET_OPTION_TENACITY, PET_TENACITY_BOAR);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Crab:30|t Crab", PET_OPTION_TENACITY, PET_TENACITY_CRAB);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Crocolisk:30|t Crocolisk", PET_OPTION_TENACITY, PET_TENACITY_CROCOLISK);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Gorilla:30|t Gorilla", PET_OPTION_TENACITY, PET_TENACITY_GORILLA);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Rhino:30|t Rhino", PET_OPTION_TENACITY, PET_TENACITY_RHINO);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Scorpid:30|t Scorpid", PET_OPTION_TENACITY, PET_TENACITY_SCORPID);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Turtle:30|t Turtle", PET_OPTION_TENACITY, PET_TENACITY_TURTLE);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_WarpStalker:30|t Warp Stalker", PET_OPTION_TENACITY, PET_TENACITY_WARP_STALKER);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_Hunter_Pet_Worm:30|t Worm", PET_OPTION_TENACITY, PET_TENACITY_WORM);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, MALL_MENU_PETS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            break;
        }
    }

    return true;
}

bool MallInOne::HandleSetupShadowDanceBar(Player* player, Creature* creature)
{
    if (player->HasSpell(51713))
    {
        player->InterruptMovement();
        player->GetMotionMaster()->MoveFall();
        player->SetControlled(true, UNIT_STATE_ROOT);
        player->AddAura(SPELL_SERVERSIDE_SILENCE, player);
        if (!player->HasAura(51713))
            player->AddAura(51713, player);

        if (Aura* ShadowDance = player->GetAura(51713, player->GetGUID()))
            ShadowDance->SetDurationAndMaxDuration(-1);

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_reputation_01:30|t I'm Finished", GOSSIP_SENDER_MAIN, MALL_MENU_FINISH_SHADOW_DANCE);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    }
    else
        player->CLOSE_GOSSIP_MENU();
    return true;
}

bool MallInOne::HandleFinishShadowDanceBar(Player* player, Creature* creature)
{
    player->SetControlled(false, UNIT_STATE_ROOT);
    player->RemoveAura(SPELL_SERVERSIDE_SILENCE);
    player->RemoveAura(51713);
    OnGossipHello(player, creature);
    return true;
}

bool MallInOne::HandleLearnDualSpecialization(Player* player, Creature* creature)
{
    player->SetSaveTimer(sWorld->getIntConfig(CONFIG_INTERVAL_SAVE));
    player->learnSpell(63644, false);
    player->learnSpell(63645, false);
    player->UpdateSpecCount(2);
    player->SaveToDB();
    OnGossipHello(player, creature);
    return true;
}

bool MallInOne::HandleResetTalents(Player* player, Creature* creature)
{
    player->CLOSE_GOSSIP_MENU();
    player->SendTalentWipeConfirm(creature->GetGUID());
    return true;
}

bool MallInOne::HandleResetPetTalents(Player* player)
{
    player->CLOSE_GOSSIP_MENU();

    Pet* pet = player->GetPet();
    if (!pet)
    {
        player->SendSysMessage("You don't have a pet");
        return false;
    }

    pet->resetTalents();
    player->SendTalentsInfoData(true);
    return true;
}

bool MallInOne::ShowInventory(Player* player, Creature* creature, uint32 invListOption)
{
    player->CLOSE_GOSSIP_MENU();

    const VendorItemData* items = creature->GetVendorItems();
    if (!items)
    {
        player->SendSysMessage("Unable to find item data");
        return false;
    }

    uint16 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << uint64(creature->GetGUID());
    size_t countPos = data.wpos();
    data << uint8(count);

    for (uint16 slot = 0; slot < itemCount; ++slot)
    {
        if (const VendorItem* vItem = items->GetItemByVendorSlot(slot))
        {
            if (const ItemTemplate* vItemTemplate = sObjectMgr->GetItemTemplate(vItem->item))
            {
                if (!CheckVendorItem(player, vItemTemplate, invListOption))
                    continue;

                ++count;
                data << uint32(count + 1);                      // Client expects counting to start at 1
                data << uint32(vItem->item);                    // Entry
                data << uint32(vItemTemplate->DisplayInfoID);   // DisplayId
                data << int32(0xFFFFFFFF);                      // Left in stock
                data << uint32(vItemTemplate->BuyPrice);        // Price
                data << uint32(vItemTemplate->MaxDurability);   // Durability
                data << uint32(vItemTemplate->BuyCount);        // Buy Count
                if (player->GetSession()->GetSecurity() >= SEC_VIP && creature->IsVipVendor())
                    data << uint32(0);
                else
                    data << uint32(vItem->ExtendedCost);
            }
        }
    }

    if (!count)
    {
        player->SendSysMessage("Unable to find item data");
        return false;
    }

    data.put<uint8>(countPos, count);
    player->GetSession()->SendPacket(&data);
    return true;
}

bool MallInOne::EnchantItem(Player* player, Creature* creature, uint16 enchantId, uint8 enchantOption)
{
    uint8 slot = GetItemSlotByEnchantOption(enchantOption);
    if (slot > EQUIPMENT_SLOT_RANGED)
    {
        player->SendSysMessage("Unable to find item slot based on enchant option: %u", enchantOption);
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!pItem)
    {
        player->SendSysMessage("You have no item equipped in your %s slot", SlotToSlotString(slot).c_str());
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    const ItemTemplate* pItemTemplate = pItem->GetTemplate();
    if (!pItemTemplate)
    {
        player->SendSysMessage("Unable to get template data for the item in your %s slot", SlotToSlotString(slot).c_str());
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    const SpellItemEnchantmentEntry* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId);
    if (!enchantEntry)
    {
        player->SendSysMessage("Unable to get data for enchant: %u", enchantId);
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    if (!CheckEnchantItem(player, pItemTemplate, enchantId, slot))
    {
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    switch (enchantId)
    {
        case ENCHANT_BELT_ETERNAL_BELT_BUCKLE:
        case ENCHANT_GLOVES_SOCKET:
        case ENCHANT_BRACER_SOCKET:
        {
            bool addSocket = false;

            for (uint8 i = 0; i < MAX_ITEM_ENCHANTMENT_EFFECTS; ++i)
                if (enchantEntry->type[i] == ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET)
                {
                    addSocket = true;
                    break;
                }

            if (!addSocket)
            {
                player->SendSysMessage("Unable to add socket to %s because enchant: %u doesn't have enchant entry type: ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET",
                pItemTemplate->Name1.c_str(), enchantId);
                player->CLOSE_GOSSIP_MENU();
                return false;
            }

            player->ApplyEnchantment(pItem, PRISMATIC_ENCHANTMENT_SLOT, false);
            pItem->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchantId, 0, 0);
            player->ApplyEnchantment(pItem, PRISMATIC_ENCHANTMENT_SLOT, true);
            pItem->ClearSoulboundTradeable(player);
            break;
        }
        default:
        {
            player->ApplyEnchantment(pItem, PERM_ENCHANTMENT_SLOT, false);
            pItem->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantId, 0, 0);
            player->ApplyEnchantment(pItem, PERM_ENCHANTMENT_SLOT, true);
            pItem->ClearSoulboundTradeable(player);
            break;
        }
    }

    player->CastSpell(player, 61335); // Disenchant Visual
    player->PlayerTalkClass->ClearMenus();
    HandleEnchants(player, creature, MALL_MENU_ENCHANTS);
    return true;
}

bool MallInOne::LearnSkill(Player* player, Creature* creature, uint16 skill)
{
    if (player->GetSkillValue(skill) == 450)
    {
        player->SendSysMessage("You already have that profession");
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    if (!CheckSkill(player, skill))
    {
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    const SkillLineEntry* skillLineEntry = sSkillLineStore.LookupEntry(skill);
    if (!skillLineEntry)
    {
        player->SendSysMessage("Unable to get skill line entry for profession: %s", SkillToSkillString(skill).c_str());
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    uint32 professionSpellId = 0;
    switch (skill)
    {
        case SKILL_ALCHEMY:        professionSpellId = 51304; break;
        case SKILL_BLACKSMITHING:  professionSpellId = 51300; break;
        case SKILL_ENCHANTING:     professionSpellId = 51313; break;
        case SKILL_ENGINEERING:    professionSpellId = 51306; break;
        case SKILL_HERBALISM:      professionSpellId = 50300; break;
        case SKILL_INSCRIPTION:    professionSpellId = 45363; break;
        case SKILL_JEWELCRAFTING:  professionSpellId = 51311; break;
        case SKILL_LEATHERWORKING: professionSpellId = 51302; break;
        case SKILL_TAILORING:      professionSpellId = 51309; break;
    }

    if (!professionSpellId)
    {
        player->SendSysMessage("Unable to get spell for profession: %s", SkillToSkillString(skill).c_str());
        player->CLOSE_GOSSIP_MENU();
        return false;
    }

    player->learnSpell(professionSpellId, false);
    player->SetSkill(skill, 6, 450, 450);
    player->PlayerTalkClass->ClearMenus();
    HandleProfessions(player, creature);
    return true;
}

bool MallInOne::CreatePet(Player* player, Creature* creature, uint32 petId)
{
    player->CLOSE_GOSSIP_MENU();

    if (IsExoticPet(petId))
        if (!player->CanTameExoticPets())
        {
            player->SendSysMessage("You can't create an exotic pet until you've specced into Beast Mastery.");
            return false;
        }

    Pet* pet = player->CreateTamedPetFrom(petId, 0);
    if (!pet)
    {
        player->SendSysMessage("Unable to create pet from Id: %u", petId);
        return false;
    }

    if (Pet* playerPet = player->GetPet())
    {
        player->StopCastingCharm();
        player->RemovePet(playerPet, PET_SAVE_AS_DELETED, false);
    }

    pet->GetMap()->AddToMap(pet->ToCreature());
    player->SetMinion(pet, true);
    pet->InitTalentForLevel();
    player->PetSpellInitialize();
    pet->SetPower(POWER_HAPPINESS, MAX_HAPPINESS);
    pet->UpdateAllStats();
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    pet->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, pet->GetFollowAngle());
    pet->GetCharmInfo()->SetCommandState(COMMAND_FOLLOW);

    player->SendSysMessage("Pet added.");
    return true;
}

bool MallInOne::CheckVendorItem(Player* player, const ItemTemplate* vItemTemplate, uint32 invListOption)
{
    // Don't allow Nibelung to be purchased until the proc is fixed
    if (IsPhraseInString(vItemTemplate->Name1, "Nibelung"))
        return false;

    // Faction specific items
    if ((vItemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && player->GetTeam() == HORDE) ||
        (vItemTemplate->Flags2 == ITEM_FLAGS_EXTRA_HORDE_ONLY && player->GetTeam() == ALLIANCE))
        return false;

    // Race specific items
    if (!(vItemTemplate->AllowableRace & player->getRaceMask()))
        return false;

    // Class specific items
    if (!(vItemTemplate->AllowableClass & player->getClassMask()))
        return false;

    switch (invListOption)
    {
        case INVENTORY_LIST_GENERAL_GOODS:
        {
            if (vItemTemplate->Class != ITEM_CLASS_CONSUMABLE &&
                vItemTemplate->Class != ITEM_CLASS_CONTAINER &&
                vItemTemplate->Class != ITEM_CLASS_REAGENT &&
                vItemTemplate->Class != ITEM_CLASS_PROJECTILE &&
                vItemTemplate->Class != ITEM_CLASS_TRADE_GOODS &&
                vItemTemplate->Class != ITEM_CLASS_GENERIC &&
                vItemTemplate->Class != ITEM_CLASS_MISC)
                return false;

            if (vItemTemplate->Class == ITEM_CLASS_MISC && vItemTemplate->SubClass == ITEM_SUBCLASS_JUNK_MOUNT)
                return false;

            break;
        }
        case INVENTORY_LIST_GEMS:
        {
            if (vItemTemplate->Class != ITEM_CLASS_GEM)
                return false;

            if (IsPhraseInString(vItemTemplate->Name1, "Dragon's Eye"))
                if (player->GetSkillValue(SKILL_JEWELCRAFTING) < 450)
                    return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_META &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_RED &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_YELLOW &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_BLUE &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_ORANGE &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_GREEN &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_PURPLE &&
                vItemTemplate->SubClass != ITEM_SUBCLASS_GEM_PRISMATIC)
                return false;

            break;
        }
        case INVENTORY_LIST_MOUNTS:
        {
            if (vItemTemplate->Class != ITEM_CLASS_MISC)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_JUNK_MOUNT)
                return false;

            break;
        }
        case INVENTORY_LIST_GLYPHS:
        {
            if (vItemTemplate->Class != ITEM_CLASS_GLYPH)
                return false;

            break;
        }
        case INVENTORY_LIST_RELENTLESS_GLADIATOR_MAINSETS_AND_OFFSETS:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Relentless") && !IsPhraseInString(vItemTemplate->Name1, "Titan-Forged"))
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != GetUsableArmorProficiencyByClass(player->getClass()))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_RELENTLESS_GLADIATOR_ACCESSORIES:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Relentless") && !IsPhraseInString(vItemTemplate->Name1, "Titan-Forged"))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_NECK &&
                vItemTemplate->InventoryType != INVTYPE_CLOAK &&
                vItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
        case INVENTORY_LIST_RELENTLESS_GLADIATOR_WEAPONS:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Relentless"))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_WEAPON &&
                vItemTemplate->InventoryType != INVTYPE_SHIELD &&
                vItemTemplate->InventoryType != INVTYPE_RANGED &&
                vItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                vItemTemplate->InventoryType != INVTYPE_HOLDABLE &&
                vItemTemplate->InventoryType != INVTYPE_THROWN &&
                vItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT &&
                vItemTemplate->InventoryType != INVTYPE_RELIC)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_CLOTH:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") || vItemTemplate->ItemLevel != 245)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_LEATHER:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") || vItemTemplate->ItemLevel != 245)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_MAIL:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") || vItemTemplate->ItemLevel != 245)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_MAINSETS_AND_OFFSETS_PLATE:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") || vItemTemplate->ItemLevel != 245)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_ACCESSORIES:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") || IsPhraseInString(vItemTemplate->Name1, "Titan-Forged") || vItemTemplate->ItemLevel != 245)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_NECK &&
                vItemTemplate->InventoryType != INVTYPE_CLOAK &&
                vItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
        case INVENTORY_LIST_TRIAL_OF_THE_CRUSADER_WEAPONS:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Relentless") ||
                (vItemTemplate->ItemLevel != 226 && vItemTemplate->ItemLevel != 232 &&
                vItemTemplate->ItemLevel != 245 && vItemTemplate->ItemLevel != 258))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_WEAPON &&
                vItemTemplate->InventoryType != INVTYPE_SHIELD &&
                vItemTemplate->InventoryType != INVTYPE_RANGED &&
                vItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                vItemTemplate->InventoryType != INVTYPE_HOLDABLE &&
                vItemTemplate->InventoryType != INVTYPE_THROWN &&
                vItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT &&
                vItemTemplate->InventoryType != INVTYPE_RELIC)
                return false;

            break;
        }
        case INVENTORY_LIST_245_TRINKETS:
        {
            if (vItemTemplate->InventoryType != INVTYPE_TRINKET || (vItemTemplate->ItemLevel != 245 && vItemTemplate->ItemLevel != 258))
                return false;

            break;
        }
        case INVENTORY_LIST_WRATHFUL_GLADIATOR_MAINSETS_AND_OFFSETS:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Wrathful"))
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != GetUsableArmorProficiencyByClass(player->getClass()))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_WRATHFUL_GLADIATOR_ACCESSORIES:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Wrathful"))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_NECK &&
                vItemTemplate->InventoryType != INVTYPE_CLOAK &&
                vItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
        case INVENTORY_LIST_WRATHFUL_GLADIATOR_WEAPONS:
        {
            if (!IsPhraseInString(vItemTemplate->Name1, "Wrathful"))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_WEAPON &&
                vItemTemplate->InventoryType != INVTYPE_SHIELD &&
                vItemTemplate->InventoryType != INVTYPE_RANGED &&
                vItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                vItemTemplate->InventoryType != INVTYPE_HOLDABLE &&
                vItemTemplate->InventoryType != INVTYPE_THROWN &&
                vItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT &&
                vItemTemplate->InventoryType != INVTYPE_RELIC)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_CLOTH:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || vItemTemplate->ItemLevel != 264)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_LEATHER:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || vItemTemplate->ItemLevel != 264)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_MAIL:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || vItemTemplate->ItemLevel != 264)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_MAINSETS_AND_OFFSETS_PLATE:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || vItemTemplate->ItemLevel != 264)
                return false;

            if (vItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (vItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_HEAD &&
                vItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                vItemTemplate->InventoryType != INVTYPE_CHEST &&
                vItemTemplate->InventoryType != INVTYPE_HANDS &&
                vItemTemplate->InventoryType != INVTYPE_LEGS &&
                vItemTemplate->InventoryType != INVTYPE_WRISTS &&
                vItemTemplate->InventoryType != INVTYPE_WAIST &&
                vItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_ACCESSORIES:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || (vItemTemplate->ItemLevel != 264 && vItemTemplate->ItemLevel != 268))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_NECK &&
                vItemTemplate->InventoryType != INVTYPE_CLOAK &&
                vItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
        case INVENTORY_LIST_ICECROWN_CITADEL_WEAPONS:
        {
            if (IsPhraseInString(vItemTemplate->Name1, "Wrathful") || (vItemTemplate->ItemLevel != 264 && vItemTemplate->ItemLevel != 277))
                return false;

            if (vItemTemplate->InventoryType != INVTYPE_WEAPON &&
                vItemTemplate->InventoryType != INVTYPE_SHIELD &&
                vItemTemplate->InventoryType != INVTYPE_RANGED &&
                vItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                vItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                vItemTemplate->InventoryType != INVTYPE_HOLDABLE &&
                vItemTemplate->InventoryType != INVTYPE_THROWN &&
                vItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT &&
                vItemTemplate->InventoryType != INVTYPE_RELIC)
                return false;

            break;
        }
        case INVENTORY_LIST_264_TRINKETS:
        {
            if (vItemTemplate->InventoryType != INVTYPE_TRINKET || vItemTemplate->ItemLevel != 264)
                return false;

            break;
        }
        case INVENTORY_LIST_APPAREL:
        {
            if (vItemTemplate->InventoryType != INVTYPE_SHIRT && vItemTemplate->InventoryType != INVTYPE_TABARD)
                return false;

            break;
        }
    }

    return true;
}

bool MallInOne::CheckSkill(Player* player, uint16 skill)
{
    uint8 skillCount = 0;

    if (player->HasSkill(SKILL_ALCHEMY))
        skillCount++;

    if (player->HasSkill(SKILL_BLACKSMITHING))
        skillCount++;

    if (player->HasSkill(SKILL_ENCHANTING))
        skillCount++;

    if (player->HasSkill(SKILL_ENGINEERING))
        skillCount++;

    if (player->HasSkill(SKILL_HERBALISM))
        skillCount++;

    if (player->HasSkill(SKILL_INSCRIPTION))
        skillCount++;

    if (player->HasSkill(SKILL_JEWELCRAFTING))
        skillCount++;

    if (player->HasSkill(SKILL_LEATHERWORKING))
        skillCount++;

    if (player->HasSkill(SKILL_TAILORING))
        skillCount++;

    if (skillCount > 1)
        return false;

    if (skill != SKILL_ALCHEMY &&
        skill != SKILL_BLACKSMITHING &&
        skill != SKILL_ENCHANTING &&
        skill != SKILL_ENGINEERING &&
        skill != SKILL_HERBALISM &&
        skill != SKILL_INSCRIPTION &&
        skill != SKILL_JEWELCRAFTING &&
        skill != SKILL_LEATHERWORKING &&
        skill != SKILL_TAILORING)
        return false;

    return true;
}

bool MallInOne::CheckEnchantItem(Player* player, const ItemTemplate* pItemTemplate, uint16 enchantId, uint8 slot)
{
    switch (enchantId)
    {
        case ENCHANT_HEAD_ARCANUM_OF_TRIUMPH:
        case ENCHANT_HEAD_ARCANUM_OF_DOMINANCE:
        case ENCHANT_HEAD_ARCANUM_OF_TORMENT:
        case ENCHANT_HEAD_ARCANUM_OF_BURNING_MYSTERIES:
        case ENCHANT_HEAD_ARCANUM_OF_BLISSFUL_MENDING:
        case ENCHANT_HEAD_ARCANUM_OF_THE_STALWART_PROTECTOR:
        case ENCHANT_HEAD_MIND_AMPLIFICATION_DISH:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_HEAD)
                return false;

            break;
        }
        case ENCHANT_SHOULDER_INSCRIPTION_OF_TRIUMPH:
        case ENCHANT_SHOULDER_INSCRIPTION_OF_DOMINANCE:
        case ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_AXE:
        case ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_STORM:
        case ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_CRAG:
        case ENCHANT_SHOULDER_GREATER_INSCRIPTION_OF_THE_PINNACLE:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_AXE:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_STORM:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_CRAG:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_PINNACLE:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_SHOULDERS)
                return false;

            break;
        }
        case ENCHANT_CHEST_EXCEPTIONAL_RESILIENCE:
        case ENCHANT_CHEST_POWERFUL_STATS:
        case ENCHANT_CHEST_SUPER_HEALTH:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_CHEST)
                return false;

            break;
        }
        case ENCHANT_GLOVES_CRUSHER:
        case ENCHANT_GLOVES_EXCEPTIONAL_SPELLPOWER:
        case ENCHANT_GLOVES_MAJOR_AGILITY:
        case ENCHANT_GLOVES_PERCISION:
        case ENCHANT_GLOVES_SOCKET:
        case ENCHANT_GLOVES_HAND_MOUNTED_PRYO_ROCKET:
        case ENCHANT_GLOVES_HYPERSPEED_ACCELERATORS:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_HANDS)
                return false;

            break;
        }
        case ENCHANT_LEGS_ICESCALE_LEG_ARMOR:
        case ENCHANT_LEGS_SAPPHIRE_SPELLTHREAD:
        case ENCHANT_LEGS_BRILLIANT_SPELLTHREAD:
        case ENCHANT_LEGS_FROSTHIDE_LEG_ARMOR:
        case ENCHANT_LEGS_EARTHEN_LEG_ARMOR:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_LEGS)
                return false;

            break;
        }
        case ENCHANT_BRACERS_GREATER_ASSAULT:
        case ENCHANT_BRACERS_SUPERIOR_SPELLPOWER:
        case ENCHANT_BRACERS_EXCEPTIONAL_INTELLECT:
        case ENCHANT_BRACERS_MAJOR_SPIRIT:
        case ENCHANT_BRACERS_MAJOR_STAMINA:
        case ENCHANT_BRACER_SOCKET:
        case ENCHANT_BRACER_FUR_LINING_ATTACKPOWER:
        case ENCHANT_BRACER_FUR_LINING_SPELLPOWER:
        case ENCHANT_BRACER_FUR_LINING_AGILITY:
        case ENCHANT_BRACER_FUR_LINING_STAMINA:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_WRISTS)
                return false;

            break;
        }
        case ENCHANT_BELT_ETERNAL_BELT_BUCKLE:
        case ENCHANT_BELT_FRAG_BELT:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_WAIST)
                return false;

            break;
        }
        case ENCHANT_BOOTS_GREATER_ASSULT:
        case ENCHANT_BOOTS_SUPERIOR_AGILITY:
        case ENCHANT_BOOTS_ICEWALKER:
        case ENCHANT_BOOTS_TUSKARR_VITALITY:
        case ENCHANT_BOOTS_NITRO_BOOSTS:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case ENCHANT_CLOAK_SPELL_PIERCING:
        case ENCHANT_CLOAK_MAJOR_AGILITY:
        case ENCHANT_CLOAK_GREATER_SPEED:
        case ENCHANT_CLOAK_SHADOW_ARMOR:
        case ENCHANT_CLOAK_FLEXWEAVE_UNDERLAY:
        case ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE:
        case ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY:
        case ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY:
        case ENCHANT_CLOAK_DARKGLOW_EMBROIDERY:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_CLOAK)
                return false;

            break;
        }
        case ENCHANT_2H_WEAPON_MASSACRE:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_2HWEAPON)
                return false;

            break;
        }
        case ENCHANT_STAFF_GREATER_SPELLPOWER:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_2HWEAPON)
                return false;

            break;
        }
        case ENCHANT_WEAPON_BERSERKING:
        case ENCHANT_WEAPON_BLACK_MAGIC:
        case ENCHANT_WEAPON_BLOOD_DRAINING:
        case ENCHANT_WEAPON_BLADE_WARDING:
        case ENCHANT_WEAPON_TITANIUM_WEAPON_CHAIN:
        case ENCHANT_WEAPON_SUPERIOR_POTENCY:
        case ENCHANT_WEAPON_MIGHTY_SPELLPOWER:
        case ENCHANT_WEAPON_EXCEPTIONAL_AGILITY:
        case ENCHANT_WEAPON_EXCEPTIONAL_SPIRIT:
        case ENCHANT_WEAPON_TITANGUARD:
        case ENCHANT_WEAPON_ACCURACY:
        case ENCHANT_WEAPON_MONGOOSE:
        case ENCHANT_WEAPON_EXECUTIONER:
        case ENCHANT_WEAPON_DEATHFROST:
        case ENCHANT_WEAPON_FALLEN_CRUSADER:
        case ENCHANT_WEAPON_CINDERGLACIER:
        case ENCHANT_WEAPON_RAZORICE:
        case ENCHANT_WEAPON_LICHBANE:
        case ENCHANT_WEAPON_SPELLBREAKING:
        case ENCHANT_WEAPON_SPELLSHATTERING:
        case ENCHANT_WEAPON_SWORDBREAKING:
        case ENCHANT_WEAPON_SWORDSHATTERING:
        case ENCHANT_WEAPON_NERUBIAN_CARAPACE:
        case ENCHANT_WEAPON_STONESKIN_GARGOYLE:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_FIST &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_DAGGER)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                pItemTemplate->InventoryType != INVTYPE_WEAPON &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND)
                return false;

            break;
        }
        case ENCHANT_RANGED_HEARTSEEKER_SCOPE:
        case ENCHANT_RANGED_SUN_SCOPE:
        case ENCHANT_RANGED_DIAMOND_CUT_REFRACTOR_SCOPE:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_BOW &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_GUN &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_RANGED &&
                pItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT)
                return false;

            break;
        }
        case ENCHANT_SHIELD_TITANIUM_PLATING:
        case ENCHANT_SHIELD_GREATER_INTELLECT:
        case ENCHANT_SHIELD_RESILIENCE:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_SHIELD)
                return false;

            break;
        }
        case ENCHANT_RING_ASSAULT:
        case ENCHANT_RING_GREATER_SPELLPOWER:
        case ENCHANT_RING_STAMINA:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MISC)
                return false;

            if (pItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
    }

    switch (slot)
    {
        case EQUIPMENT_SLOT_HEAD:
        case EQUIPMENT_SLOT_SHOULDERS:
        case EQUIPMENT_SLOT_CHEST:
        case EQUIPMENT_SLOT_HANDS:
        case EQUIPMENT_SLOT_LEGS:
        case EQUIPMENT_SLOT_WRISTS:
        case EQUIPMENT_SLOT_WAIST:
        case EQUIPMENT_SLOT_FEET:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_ARMOR)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MAIL &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_HEAD &&
                pItemTemplate->InventoryType != INVTYPE_SHOULDERS &&
                pItemTemplate->InventoryType != INVTYPE_CHEST &&
                pItemTemplate->InventoryType != INVTYPE_HANDS &&
                pItemTemplate->InventoryType != INVTYPE_LEGS &&
                pItemTemplate->InventoryType != INVTYPE_WRISTS &&
                pItemTemplate->InventoryType != INVTYPE_WAIST &&
                pItemTemplate->InventoryType != INVTYPE_FEET)
                return false;

            break;
        }
        case EQUIPMENT_SLOT_BACK:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_ARMOR)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_CLOTH)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_CLOAK)
                return false;

            break;
        }
        case EQUIPMENT_SLOT_FINGER1:
        case EQUIPMENT_SLOT_FINGER2:
        {
            if (pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_ARMOR)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_MISC)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_FINGER)
                return false;

            break;
        }
        case EQUIPMENT_SLOT_MAINHAND:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_WEAPON)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_FIST &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_DAGGER)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_WEAPON &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                pItemTemplate->InventoryType != INVTYPE_2HWEAPON)
                return false;

            break;
        }
        case EQUIPMENT_SLOT_OFFHAND:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON && pItemTemplate->Class != ITEM_CLASS_ARMOR)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_WEAPON)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_FIST &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_DAGGER)
                    return false;

            if (pItemTemplate->Class == ITEM_CLASS_ARMOR)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_WEAPON &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONMAINHAND &&
                pItemTemplate->InventoryType != INVTYPE_WEAPONOFFHAND &&
                pItemTemplate->InventoryType != INVTYPE_2HWEAPON &&
                pItemTemplate->InventoryType != INVTYPE_SHIELD)
                return false;

            break;
        }
        case EQUIPMENT_SLOT_RANGED:
        {
            if (pItemTemplate->Class != ITEM_CLASS_WEAPON)
                return false;

            if (pItemTemplate->Class == ITEM_CLASS_WEAPON)
                if (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_BOW &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_GUN &&
                    pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW)
                    return false;

            if (pItemTemplate->InventoryType != INVTYPE_RANGED &&
                pItemTemplate->InventoryType != INVTYPE_RANGEDRIGHT)
                return false;

            break;
        }
    }

    switch (enchantId)
    {
        case ENCHANT_2H_WEAPON_MASSACRE:
        {
            if (pItemTemplate->Class == ITEM_CLASS_WEAPON && pItemTemplate->InventoryType != INVTYPE_2HWEAPON)
                return false;

            break;
        }
        case ENCHANT_STAFF_GREATER_SPELLPOWER:
        {
            if (pItemTemplate->Class == ITEM_CLASS_WEAPON && pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
                return false;

            break;
        }
        case ENCHANT_RANGED_HEARTSEEKER_SCOPE:
        case ENCHANT_RANGED_SUN_SCOPE:
        case ENCHANT_RANGED_DIAMOND_CUT_REFRACTOR_SCOPE:
        {
            if (pItemTemplate->Class == ITEM_CLASS_WEAPON && (pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_BOW &&
                pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_GUN && pItemTemplate->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW))
                return false;

            break;
        }
        case ENCHANT_SHIELD_TITANIUM_PLATING:
        case ENCHANT_SHIELD_GREATER_INTELLECT:
        case ENCHANT_SHIELD_RESILIENCE:
        {
            if (pItemTemplate->InventoryType != INVTYPE_SHIELD)
                return false;

            break;
        }
        case ENCHANT_WEAPON_FALLEN_CRUSADER:
        case ENCHANT_WEAPON_CINDERGLACIER:
        case ENCHANT_WEAPON_RAZORICE:
        case ENCHANT_WEAPON_LICHBANE:
        case ENCHANT_WEAPON_SPELLBREAKING:
        case ENCHANT_WEAPON_SPELLSHATTERING:
        case ENCHANT_WEAPON_SWORDBREAKING:
        case ENCHANT_WEAPON_SWORDSHATTERING:
        case ENCHANT_WEAPON_NERUBIAN_CARAPACE:
        case ENCHANT_WEAPON_STONESKIN_GARGOYLE:
        {
            if (pItemTemplate->Class == ITEM_CLASS_WEAPON && player->getClass() != CLASS_DEATH_KNIGHT)
                return false;

            break;
        }
        case ENCHANT_GLOVES_SOCKET:
        case ENCHANT_BRACER_SOCKET:
        {
            if (player->GetSkillValue(SKILL_BLACKSMITHING) < 450)
                return false;

            break;
        }
        case ENCHANT_RING_ASSAULT:
        case ENCHANT_RING_GREATER_SPELLPOWER:
        case ENCHANT_RING_STAMINA:
        {
            if (player->GetSkillValue(SKILL_ENCHANTING) < 450)
                return false;

            break;
        }
        case ENCHANT_HEAD_MIND_AMPLIFICATION_DISH:
        case ENCHANT_GLOVES_HAND_MOUNTED_PRYO_ROCKET:
        case ENCHANT_GLOVES_HYPERSPEED_ACCELERATORS:
        case ENCHANT_BELT_FRAG_BELT:
        case ENCHANT_BOOTS_NITRO_BOOSTS:
        case ENCHANT_CLOAK_FLEXWEAVE_UNDERLAY:
        case ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE:
        {
            if (player->GetSkillValue(SKILL_ENGINEERING) < 450)
                return false;

            for (uint8 i = 0; i < MAX_ITEM_SPELLS; ++i)
                if (pItemTemplate->Spells[i].SpellId && !pItemTemplate->Spells[i].SpellTrigger)
                    return false;

            break;
        }
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_AXE:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_STORM:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_CRAG:
        case ENCHANT_SHOULDERS_MASTERS_INSCRIPTION_OF_THE_PINNACLE:
        {
            if (player->GetSkillValue(SKILL_INSCRIPTION) < 450)
                return false;

            break;
        }
        case ENCHANT_BRACER_FUR_LINING_ATTACKPOWER:
        case ENCHANT_BRACER_FUR_LINING_SPELLPOWER:
        case ENCHANT_BRACER_FUR_LINING_AGILITY:
        case ENCHANT_BRACER_FUR_LINING_STAMINA:
        {
            if (player->GetSkillValue(SKILL_LEATHERWORKING) < 450)
                return false;

            break;
        }
        case ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY:
        case ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY: 
        case ENCHANT_CLOAK_DARKGLOW_EMBROIDERY:
        {
            if (player->GetSkillValue(SKILL_TAILORING) < 450)
                return false;

            break;
        }
    }

    return true;
}

void AddSC_MallInOne()
{
    new MallInOne();
}
