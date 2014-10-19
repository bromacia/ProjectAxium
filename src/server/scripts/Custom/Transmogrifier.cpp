#include "Transmogrifier.h"

Transmogrifier::Transmogrifier() : CreatureScript("Transmogrifier")
{
}

bool Transmogrifier::OnGossipHello(Player* player, Creature* creature)
{
    player->SetSelectedTransmogItemSlot(0);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_helmet_94:30|t Individual", GOSSIP_SENDER_MAIN, TRANSMOG_ACTION_SHOW_INDIVIDUAL);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Inv_weapon_shortblade_54:30|t Enchants", GOSSIP_SENDER_MAIN, TRANSMOG_ACTION_SHOW_ENCHANTS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_Enchant_Disenchant:30|t Remove Item Transmogrifiers", GOSSIP_SENDER_MAIN, TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_DualWieldSpecialization:30|t Remove Enchant Transmogrifiers", GOSSIP_SENDER_MAIN, TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\spell_Shadow_SacrificialShield:30|t Remove All Transmogrifiers", GOSSIP_SENDER_MAIN, TRANSMOG_ACTION_SELECT_REMOVE_EVERYTHING);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool Transmogrifier::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    switch (sender)
    {
        case GOSSIP_SENDER_MAIN:
        {
            switch (action)
            {
                case TRANSMOG_ACTION_SHOW_INDIVIDUAL:                      ShowIndividualTransmogOptions(player, creature);        break;
                case TRANSMOG_ACTION_SHOW_ENCHANTS:                        ShowTransmogEnchants(player, creature);                 break;
                case TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS:    ShowRemoveTransmogItemOptions(player, creature);        break;
                case TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS: ShowRemoveTransmogEnchantOptions(player, creature);     break;
                case TRANSMOG_ACTION_SELECT_REMOVE_EVERYTHING:             RemoveAllTransmog(player);                              break;
                case GOSSIP_SENDER_MAIN:                                   OnGossipHello(player, creature);                        break;
            }
            break;
        }
        case TRANSMOG_ACTION_SHOW_INDIVIDUAL:                   SelectIndividualTransmog(player, creature, action);   break;
        case TRANSMOG_ACTION_SHOW_ENCHANTS:                     TransmogrifyEnchant(player, action);                  break;
        case TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS:
        {
            switch (action)
            {
                case TRANSMOG_ACTION_SELECT_REMOVE_ALL_ARMOR:   RemoveAllArmorTransmog(player);                     break;
                case TRANSMOG_ACTION_SELECT_REMOVE_ALL_WEAPON:  RemoveAllWeaponTransmog(player);                    break;
                default:                                        RemoveItemTransmog(player, action);                 break;
            }
            break;
        }
        case TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS:
        {
            switch (action)
            {
                case TRANSMOG_ACTION_SELECT_REMOVE_ALL_ENCHANT: RemoveAllEnchantTransmog(player);      break;
                default:                                        RemoveEnchantTransmog(player, action); break;
            }
            break;
        }
    }

    return true;
}

void Transmogrifier::ShowIndividualTransmogOptions(Player* player, Creature* creature)
{
    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platehelm:30|t Head", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_HEAD);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plateshoulder:30|t Shoulders", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_SHOULDERS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platechest:30|t Chest", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_CHEST);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plategloves:30|t Hands", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_HANDS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platepants:30|t Legs", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_LEGS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platebracer:30|t Wrists", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_WRISTS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_platebelt:30|t Waist", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_WAIST);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_desecrated_plateboots:30|t Boots", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_FEET);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_sword_04:30|t Main Hand", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_MAINHAND);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_shield_04:30|t Off Hand", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_OFFHAND);

    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
        if (item->GetTemplate()->InventoryType != INVTYPE_RELIC)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_weapon_crossbow_04:30|t Ranged", TRANSMOG_ACTION_SHOW_INDIVIDUAL, TRANSMOG_ACTION_SHOW_INDIVIDUAL_RANGED);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());

    player->SetSelectedTransmogItemSlot(0);
}

void Transmogrifier::ShowTransmogEnchants(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mongoose", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_MONGOOSE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Executioner", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_EXECUTIONER);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sunfire", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_SUNFIRE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Soulfrost", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_SOULFROST);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Draining", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_BLOOD_DRAINING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razorice", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_RAZORICE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Berserking", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_BERSERKING);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Spellsurge", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_SPELLSURGE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Potency", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_POTENCY);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Deathfrost", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_DEATHFROST);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sparkles", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_SPARKLES);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Red Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_RED);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yellow Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_YELLOW);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blue Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_BLUE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Purple Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_PURPLE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orange Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_ORANGE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Green Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_GREEN);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Grey Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_GREY);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "White Glow", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_GLOW_WHITE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "No Visual", TRANSMOG_ACTION_SHOW_ENCHANTS, TRANSMOG_ENCHANT_NO_VISUAL);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
}

void Transmogrifier::ShowRemoveTransmogItemOptions(Player* player, Creature* creature)
{
    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Head", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_HEAD);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shoulders", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_SHOULDERS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Chest", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_CHEST);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hands", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_HANDS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Legs", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_LEGS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Wrists", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_WRISTS);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Waist", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_WAIST);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Boots", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_FEET);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Main Hand", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_MAINHAND);

    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Off Hand", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_OFFHAND);

    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
        if (item->GetTemplate()->InventoryType != INVTYPE_RELIC)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ranged", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_RANGED);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remove All Armor Transmogrifiers", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_ALL_ARMOR);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remove All Weapon Transmogrifiers", TRANSMOG_ACTION_SHOW_REMOVE_ITEM_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_ALL_WEAPON);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
}

void Transmogrifier::ShowRemoveTransmogEnchantOptions(Player* player, Creature* creature)
{
    if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Main Hand", TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_MAINHAND_ENCHANT);

    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
        if (item->GetTemplate()->Class == ITEM_CLASS_WEAPON)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Off Hand", TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_OFFHAND_ENCHANT);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remove All Enchant Transmogrifiers", TRANSMOG_ACTION_SHOW_REMOVE_ENCHANT_TRANSMOG_OPTIONS, TRANSMOG_ACTION_SELECT_REMOVE_ALL_ENCHANT);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
}

void Transmogrifier::SelectIndividualTransmog(Player* player, Creature* creature, uint16 action)
{
    uint8 itemSlot = GetItemSlotByAction(action);
    player->SetSelectedTransmogItemSlot(itemSlot);

    player->CLOSE_GOSSIP_MENU();

    const ItemTemplate* pItemTemplate = player->GetItemByPos(INVENTORY_SLOT_BAG_0, itemSlot)->GetTemplate();
    if (!pItemTemplate)
    {
        player->SendSysMessage("Unable to find item data for slot %u.", itemSlot);
        player->SetSelectedTransmogItemSlot(0);
        return;
    }

    const VendorItemData* items = creature->GetVendorItems();
    if (!items)
    {
        player->SendSysMessage("This transmogrifier doesn't have any items.");
        player->SetSelectedTransmogItemSlot(0);
        return;
    }

    uint16 itemCount = items->GetItemCount();
    uint8 count = 0;
    uint8 slot = 0;

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
                if (!player->CheckItem(vItemTemplate, pItemTemplate))
                    continue;

                int32 leftInStock = 0xFFFFFFFF; // The item will appear normally

                if (!player->CheckExtendedCost2(vItemTemplate))
                    leftInStock = 0x0; // The item will appear greyed out

                ++count;
                data << uint32(slot + 1);                       // Client expects counting to start at 1
                data << uint32(vItem->item);                    // Entry
                data << uint32(vItemTemplate->DisplayInfoID);   // DisplayId
                data << int32(leftInStock);                     // Left in stock
                data << uint32(0);                              // Price
                data << uint32(vItemTemplate->MaxDurability);   // Durability
                data << uint32(vItemTemplate->BuyCount);        // Buy Count
                data << uint32(0);                              // Extended Cost

            }
        }
    }

    if (!count)
    {
        player->SendSysMessage("No items found for that option.");
        player->SetSelectedTransmogItemSlot(0);
        return;
    }

    data.put<uint8>(countPos, count);
    player->GetSession()->SendPacket(&data);
}

void Transmogrifier::TransmogrifyEnchant(Player* player, uint16 action)
{
    if (action && player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND) != 0)
    {
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
            if (const ItemTemplate* pItemTemplate = pItem->GetTemplate())
                if (pItemTemplate->Class == ITEM_CLASS_WEAPON)
                {
                    player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND)->TransmogEnchant = action;
                    player->SetUInt16Value(TRANSMOG_SLOT_MAINHAND_ENCHANT, 0, action);
                    player->SetUInt16Value(TRANSMOG_SLOT_MAINHAND_ENCHANT, 1, action);
                    TransmogItemInformation tItemInfo;
                    tItemInfo.TransmogEntry = pItem->TransmogEntry;
                    tItemInfo.TransmogEnchant = pItem->TransmogEnchant;
                    player->transmogItemsSaveQueue[pItem->GetGUIDLow()] = tItemInfo;
                }
    }

    if (action && player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND) != 0)
    {
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
            if (const ItemTemplate* pItemTemplate = pItem->GetTemplate())
                if (pItemTemplate->Class == ITEM_CLASS_WEAPON)
                {
                    player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND)->TransmogEnchant = action;
                    player->SetUInt16Value(TRANSMOG_SLOT_OFFHAND_ENCHANT, 0, action);
                    player->SetUInt16Value(TRANSMOG_SLOT_OFFHAND_ENCHANT, 1, action);
                    TransmogItemInformation tItemInfo;
                    tItemInfo.TransmogEntry = pItem->TransmogEntry;
                    tItemInfo.TransmogEnchant = pItem->TransmogEnchant;
                    player->transmogItemsSaveQueue[pItem->GetGUIDLow()] = tItemInfo;
                }
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveItemTransmog(Player* player, uint16 action)
{
    uint8 itemSlot = GetItemSlotByAction(action);

    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, itemSlot))
    {
        item->TransmogEntry = 0;
        player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (itemSlot * 2), item->GetEntry());
        TransmogItemInformation tItemInfo;
        tItemInfo.TransmogEntry = item->TransmogEntry;
        tItemInfo.TransmogEnchant = item->TransmogEnchant;
        player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveEnchantTransmog(Player* player, uint16 action)
{
    uint8 itemSlot = GetItemSlotByAction(action);

    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, itemSlot))
    {
        item->TransmogEnchant = 0;
        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (itemSlot * 2), 0, item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (itemSlot * 2), 1, item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
        TransmogItemInformation tItemInfo;
        tItemInfo.TransmogEntry = item->TransmogEntry;
        tItemInfo.TransmogEnchant = item->TransmogEnchant;
        player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveAllArmorTransmog(Player* player)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            if (slot == EQUIPMENT_SLOT_HEAD ||
                slot == EQUIPMENT_SLOT_SHOULDERS ||
                slot == EQUIPMENT_SLOT_CHEST ||
                slot == EQUIPMENT_SLOT_HANDS ||
                slot == EQUIPMENT_SLOT_LEGS ||
                slot == EQUIPMENT_SLOT_WRISTS ||
                slot == EQUIPMENT_SLOT_WAIST ||
                slot == EQUIPMENT_SLOT_FEET)
            {
                item->TransmogEntry = 0;
                player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), item->GetEntry());
                TransmogItemInformation tItemInfo;
                tItemInfo.TransmogEntry = item->TransmogEntry;
                tItemInfo.TransmogEnchant = item->TransmogEnchant;
                player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveAllWeaponTransmog(Player* player)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            if (slot == EQUIPMENT_SLOT_MAINHAND ||
                slot == EQUIPMENT_SLOT_OFFHAND ||
                slot == EQUIPMENT_SLOT_RANGED)
            {
                item->TransmogEntry = 0;
                player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), item->GetEntry());
                TransmogItemInformation tItemInfo;
                tItemInfo.TransmogEntry = item->TransmogEntry;
                tItemInfo.TransmogEnchant = item->TransmogEnchant;
                player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveAllEnchantTransmog(Player* player)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            if (slot == EQUIPMENT_SLOT_MAINHAND ||
                slot == EQUIPMENT_SLOT_OFFHAND ||
                slot == EQUIPMENT_SLOT_RANGED)
            {
                item->TransmogEnchant = 0;
                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 1, item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
                TransmogItemInformation tItemInfo;
                tItemInfo.TransmogEntry = item->TransmogEntry;
                tItemInfo.TransmogEnchant = item->TransmogEnchant;
                player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
}

void Transmogrifier::RemoveAllTransmog(Player* player)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            if (slot == EQUIPMENT_SLOT_HEAD ||
                slot == EQUIPMENT_SLOT_SHOULDERS ||
                slot == EQUIPMENT_SLOT_CHEST ||
                slot == EQUIPMENT_SLOT_HANDS ||
                slot == EQUIPMENT_SLOT_LEGS ||
                slot == EQUIPMENT_SLOT_WRISTS ||
                slot == EQUIPMENT_SLOT_WAIST ||
                slot == EQUIPMENT_SLOT_FEET ||
                slot == EQUIPMENT_SLOT_MAINHAND ||
                slot == EQUIPMENT_SLOT_OFFHAND ||
                slot == EQUIPMENT_SLOT_RANGED)
            {
                item->TransmogEntry = 0;
                player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), item->GetEntry());
                TransmogItemInformation tItemInfo;
                tItemInfo.TransmogEntry = item->TransmogEntry;
                tItemInfo.TransmogEnchant = item->TransmogEnchant;
                player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
            }

            if (slot == EQUIPMENT_SLOT_MAINHAND ||
                slot == EQUIPMENT_SLOT_OFFHAND ||
                slot == EQUIPMENT_SLOT_RANGED)
            {
                item->TransmogEnchant = 0;
                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 1, item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
                TransmogItemInformation tItemInfo;
                tItemInfo.TransmogEntry = item->TransmogEntry;
                tItemInfo.TransmogEnchant = item->TransmogEnchant;
                player->transmogItemsSaveQueue[item->GetGUIDLow()] = tItemInfo;
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
}

void AddSC_Transmogrifier()
{
    new Transmogrifier();
}
