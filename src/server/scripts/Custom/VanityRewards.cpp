#include "ScriptPCH.h"

enum VanityRewardOption
{
    VANITY_REWARD_OPTION_TABARD = 2,
    VANITY_REWARD_OPTION_MOUNTS,
    VANITY_REWARD_OPTION_ITEMS,
};

class VanityRewards : public CreatureScript
{
    public:
        VanityRewards() : CreatureScript("VanityRewards") {}

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->PlayerTalkClass->ClearMenus();

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_tabardsummer02:30|t Tabards", GOSSIP_SENDER_MAIN, VANITY_REWARD_OPTION_TABARD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_mount_bigblizzardbear:30|t Mounts", GOSSIP_SENDER_MAIN, VANITY_REWARD_OPTION_MOUNTS);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_discoball_01:30|t Items", GOSSIP_SENDER_MAIN, VANITY_REWARD_OPTION_ITEMS);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
        {
            ShowInventory(player, creature, action);
            return true;
        }

        bool ShowInventory(Player* player, Creature* creature, uint32 invListOption)
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
                        data << uint32(vItemTemplate->DisplayInfoID);
                        data << int32(0xFFFFFFFF);                      // Left in stock
                        data << uint32(vItemTemplate->BuyPrice);
                        data << uint32(vItemTemplate->MaxDurability);
                        data << uint32(vItemTemplate->BuyCount);
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

        bool CheckVendorItem(Player* player, const ItemTemplate* vItemTemplate, uint32 invListOption)
        {
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
                case VANITY_REWARD_OPTION_TABARD:
                {
                    if (vItemTemplate->InventoryType != INVTYPE_TABARD)
                        return false;

                    break;
                }
                case VANITY_REWARD_OPTION_MOUNTS:
                {
                    if (vItemTemplate->Class != ITEM_CLASS_MISC)
                        return false;

                    if (vItemTemplate->SubClass != ITEM_SUBCLASS_JUNK_MOUNT)
                        return false;

                    break;
                }
                case VANITY_REWARD_OPTION_ITEMS:
                {
                    if (vItemTemplate->Class == ITEM_CLASS_MISC && vItemTemplate->SubClass == ITEM_SUBCLASS_JUNK_MOUNT)
                        return false;

                    if (vItemTemplate->InventoryType == INVTYPE_TABARD)
                        return false;

                    break;
                }
            }

            return true;
        }
};

void AddSC_VanityRewards()
{
    new VanityRewards();
}
