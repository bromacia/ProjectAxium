#include "Chat.h"
#include "MapManager.h"
#include "PvPMgr.h"
#include "MoveSpline.h"

class utility_commandscript : public CommandScript
{
    public:
        utility_commandscript() : CommandScript("utility_commandscript") {}

        ChatCommand* GetCommands() const
        {
            static ChatCommand utilityCommandTable[] =
            {
                { "customize",              SEC_PLAYER,         false, &HandleCustomizeCommand,                 "", NULL },
                { "race",                   SEC_PLAYER,         false, &HandleRaceCommand,                      "", NULL },
                { "faction",                SEC_PLAYER,         false, &HandleFactionCommand,                   "", NULL },
                { "prepare",                SEC_GAMEMASTER,     false, &HandlePrepareCommand,                   "", NULL },
                { "barbershop",             SEC_PLAYER,         false, &HandleBarbershopCommand,                "", NULL },
                { "sendcooldown",           SEC_GAMEMASTER,     false, &HandleSendCooldownCommand,              "", NULL },
                { "transmogrifyitem",       SEC_GAMEMASTER,     false, &HandleTransmogrifyItemCommand,          "", NULL },
                { "transmogrifyenchant",    SEC_GAMEMASTER,     false, &HandleTransmogrifyEnchantCommand,       "", NULL },
                { "untransmogrifyitem",     SEC_GAMEMASTER,     false, &HandleUntransmogrifyItemCommand,        "", NULL },
                { "untransmogrifyenchant",  SEC_GAMEMASTER,     false, &HandleUntransmogrifyEnchantCommand,     "", NULL },
                { "transmogcopygear",       SEC_GAMEMASTER,     false, &HandleTransmogCopyGearCommand,          "", NULL },
                { "transmogsendgear",       SEC_GAMEMASTER,     false, &HandleTransmogSendGearCommand,          "", NULL },
                { "warp",                   SEC_GAMEMASTER,     false, &HandleWarpCommand,                      "", NULL },
                { "setviewpoint",           SEC_GAMEMASTER,     false, &HandleSetViewpointCommand,              "", NULL },
                { "restoreviewpoint",       SEC_GAMEMASTER,     false, &HandleRestoreViewpointCommand,          "", NULL },
                { "speed",                  SEC_GAMEMASTER,     false, &HandleSpeedCommand,                     "", NULL },
                { "movementflags",          SEC_GAMEMASTER,     false, &HandleMovementFlagsCommand,             "", NULL },
                { "unitstate",              SEC_GAMEMASTER,     false, &HandleUnitStateCommand,                 "", NULL },
                { "splineinfo",             SEC_GAMEMASTER,     false, &HandleSplineInfoCommand,                "", NULL },
                { "itemid",                 SEC_GAMEMASTER,     false, &HandleItemIdCommand,                    "", NULL },
                { "spellid",                SEC_GAMEMASTER,     false, &HandleSpellIdCommand,                   "", NULL },
                { "coeff",                  SEC_GAMEMASTER,     false, &HandleCoeffCommand,                     "", NULL },
                { "bank",                   SEC_PLAYER,         false, &HandleBankCommand,                      "", NULL },
                { "mailbox",                SEC_PLAYER,         false, &HandleMailboxCommand,                   "", NULL },
                { "commentator",            SEC_GAMEMASTER,     false, &HandleCommentatorCommand,               "", NULL },
                { NULL,                     0,                  false, NULL,                                    "", NULL }
            };
            static ChatCommand commandTable[] =
            {
                { "utility",          SEC_PLAYER,         false, NULL,                     "", utilityCommandTable },
                { NULL,               0,                  false, NULL,                                    "", NULL }
            };
            return commandTable;
        }

        static bool HandleCustomizeCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
        
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
            stmt->setUInt16(0, uint16(AT_LOGIN_CUSTOMIZE));

            handler->PSendSysMessage("Relog to customize your character");
            player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);

            stmt->setUInt32(1, player->GetGUIDLow());

            CharacterDatabase.Execute(stmt);
            return true;
        }

        static bool HandleRaceCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
        
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
            stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_RACE));

            handler->PSendSysMessage("Relog to change the race of your character");
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);

            stmt->setUInt32(1, player->GetGUIDLow());

            CharacterDatabase.Execute(stmt);
            return true;
        }

        static bool HandleFactionCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
        
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
            stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_FACTION));

            handler->PSendSysMessage("Relog to change the faction of your character");
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);

            stmt->setUInt32(1, player->GetGUIDLow());

            CharacterDatabase.Execute(stmt);
            return true;
        }

        static bool HandlePrepareCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            uint64 target_guid = player->GetGUID();

            if (!handler->extractPlayerTarget((char*)args, &player, &target_guid))
                return false;
        
            if (player)
            {
                player->ResurrectPlayer(1.0f);
                player->SpawnCorpseBones();
                player->SaveToDB();
                player->RemoveAllNegativeAuras();
                player->ClearDiminishings();
                player->ClearComboPoints();
                player->ClearInCombat();
                player->getHostileRefManager().deleteReferences();
                player->RemoveAllPlayerSpellCooldowns();

                if (Pet* pet = player->GetPet())
                    pet->RemoveAllPetSpellCooldowns(player);

                if (player->IsGMInvisible())
                    if (!player->HasAura(44816))
                        player->AddAura(44816, player);
            }
            else
                sObjectAccessor->ConvertCorpseForPlayer(target_guid);

            return true;
        }

        static bool HandleBarbershopCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();

            if (player->GetMorphId())
            {
                handler->PSendSysMessage("You can't do that while morphed");
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player->IsSitState())
            {
                handler->PSendSysMessage("You cant do that while sitting");
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player->HasAuraType(SPELL_AURA_TRANSFORM))
            {
                handler->PSendSysMessage("You cant do that while transformed");
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player->getClass() == CLASS_DRUID && player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
            {
                handler->PSendSysMessage("You cant do that while shapeshifted");
                handler->SetSentErrorMessage(true);
                return false;
            }

            WorldPacket data(SMSG_ENABLE_BARBER_SHOP, 0);
            player->GetSession()->SendPacket(&data);
            return true;
        }

        static bool HandleSendCooldownCommand(ChatHandler* handler, const char* args)
        {
            Player* target = handler->getSelectedPlayer();
            uint64 target_guid = handler->getSelectedPlayer()->GetGUID();

            if (!*args)
                return false;

            uint32 spell = handler->extractSpellIdFromLink((char*)args);

            char* timeArg = strtok(NULL, " ");

            if (!timeArg)
                return false;

            uint32 cooldown = (uint32)atoi(timeArg);

            if (!spell || !sSpellMgr->GetSpellInfo(spell) || !cooldown)
                return false;

            if (!target->HasSpell(spell))
            {
                handler->PSendSysMessage("%s does not know that spell", target->GetName());
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->RemoveSpellCooldown(spell, true);
            target->AddSpellCooldown(spell, 0, time(NULL) + cooldown, true);
            return true;
        }

        static bool HandleTransmogrifyItemCommand(ChatHandler* handler, const char* args)
        {
            Player* target = handler->getSelectedPlayer();
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 targetGuid = target->GetGUID();

            if (!*args)
                return false;

            char* slotArg = strtok((char*)args, " ");
            if (!slotArg)
                return false;

            uint8 slot = (uint32)atoi(slotArg);
            if (!slot)
                return false;

            Item* item = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!item)
                return false;

            char* itemArg = strtok(NULL, " ");
            if (!itemArg)
                return false;

            uint32 itemId = handler->extractItemIdFromLink(itemArg);
            if (!itemId)
                return false;

            target->TransmogrifyItem(item, slot, itemId);
            return true;
        }

        static bool HandleTransmogrifyEnchantCommand(ChatHandler* handler, const char* args)
        {
            Player* target = handler->getSelectedPlayer();
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 targetGuid = target->GetGUID();

            if (!*args)
                return false;

            char* slotArg = strtok((char*)args, " ");
            if (!slotArg)
                return false;

            uint8 slot = (uint32)atoi(slotArg);
            if (!slot)
                return false;

            Item* item = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!item)
                return false;

            char* enchantIdArg = strtok(NULL, " ");
            if (!enchantIdArg)
                return false;

            uint16 enchantId = (uint16)atoi(enchantIdArg);
            if (!enchantId)
                return false;

            target->TransmogrifyEnchant(item, slot, enchantId);
            return true;
        }

        static bool HandleUntransmogrifyItemCommand(ChatHandler* handler, const char* args)
        {
            Player* target = handler->getSelectedPlayer();
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 targetGuid = target->GetGUID();

            if (!*args)
                return false;

            char* slotArg = strtok((char*)args, " ");
            if (!slotArg)
                return false;

            uint8 slot = (uint32)atoi(slotArg);
            if (!slot)
                return false;

            Item* item = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!item)
                return false;

            target->UntransmogrifyItem(item, slot);
            return true;
        }

        static bool HandleUntransmogrifyEnchantCommand(ChatHandler* handler, const char* args)
        {
            Player* target = handler->getSelectedPlayer();
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 targetGuid = target->GetGUID();

            if (!*args)
                return false;

            char* slotArg = strtok((char*)args, " ");
            if (!slotArg)
                return false;

            uint8 slot = (uint32)atoi(slotArg);
            if (!slot)
                return false;

            Item* item = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!item)
                return false;

            target->UntransmogrifyEnchant(item, slot);
            return true;
        }

        static bool HandleTransmogCopyGearCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* unitTarget = handler->getSelectedUnit();
            if (!unitTarget)
                return false;

            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            {
                handler->PSendSysMessage("Target is not a player.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            Player* target = unitTarget->ToPlayer();
            if (!target)
                return false;

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
            {
                if (slot == EQUIPMENT_SLOT_HEAD     || slot == EQUIPMENT_SLOT_SHOULDERS ||
                    slot == EQUIPMENT_SLOT_CHEST    || slot == EQUIPMENT_SLOT_HANDS ||
                    slot == EQUIPMENT_SLOT_LEGS     || slot == EQUIPMENT_SLOT_WRISTS ||
                    slot == EQUIPMENT_SLOT_WAIST    || slot == EQUIPMENT_SLOT_FEET ||
                    slot == EQUIPMENT_SLOT_MAINHAND || slot == EQUIPMENT_SLOT_OFFHAND ||
                    slot == EQUIPMENT_SLOT_RANGED)
                {
                    Item* playerItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (!playerItem)
                        continue;

                    Item* targetItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (!targetItem)
                        continue;

                    uint32 itemId = targetItem->TransmogEntry ? targetItem->TransmogEntry : targetItem->GetEntry();
                    if (itemId)
                        player->TransmogrifyItem(playerItem, slot, itemId);

                    uint16 enchantId = targetItem->TransmogEnchant ? targetItem->TransmogEnchant : 0;
                    if (enchantId)
                        player->TransmogrifyEnchant(playerItem, slot, enchantId);
                }
            }
            return true;
        }

        static bool HandleTransmogSendGearCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* unitTarget = handler->getSelectedUnit();
            if (!unitTarget)
                return false;

            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            {
                handler->PSendSysMessage("Target is not a player.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            Player* target = unitTarget->ToPlayer();
            if (!target)
                return false;

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
            {
                if (slot == EQUIPMENT_SLOT_HEAD || slot == EQUIPMENT_SLOT_SHOULDERS ||
                    slot == EQUIPMENT_SLOT_CHEST || slot == EQUIPMENT_SLOT_HANDS ||
                    slot == EQUIPMENT_SLOT_LEGS || slot == EQUIPMENT_SLOT_WRISTS ||
                    slot == EQUIPMENT_SLOT_WAIST || slot == EQUIPMENT_SLOT_FEET ||
                    slot == EQUIPMENT_SLOT_MAINHAND || slot == EQUIPMENT_SLOT_OFFHAND ||
                    slot == EQUIPMENT_SLOT_RANGED)
                {
                    Item* playerItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (!playerItem)
                        continue;

                    Item* targetItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (!targetItem)
                        continue;

                    uint32 itemId = playerItem->TransmogEntry ? playerItem->TransmogEntry : playerItem->GetEntry();
                    if (itemId)
                        target->TransmogrifyItem(targetItem, slot, itemId);

                    uint16 enchantId = playerItem->TransmogEnchant ? playerItem->TransmogEnchant : 0;
                    if (enchantId)
                        target->TransmogrifyEnchant(targetItem, slot, enchantId);
                }
            }
            return true;
        }

        static bool HandleWarpCommand(ChatHandler* handler, const char* args)
        {
            if (!*args)
                return false;

            Player* player = handler->GetSession()->GetPlayer();

            char* arg1 = strtok((char*)args, " ");
            char* arg2 = strtok(NULL, " ");

            if (!arg1)
                return false;

            if (!arg2)
                return false;

            char dir = arg1[0];
            uint32 value = (int)atoi(arg2);
            float x = player->GetPositionX();
            float y = player->GetPositionY();
            float z = player->GetPositionZ();
            float o = player->GetOrientation();
            uint32 mapid = player->GetMapId();

            if (!MapManager::IsValidMapCoord(mapid, x, y, z))
            {
                handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player->isInFlight())
            {
                player->GetMotionMaster()->MovementExpired();
                player->CleanupAfterTaxiFlight();
            }
            else
                player->SaveRecallPosition();

            switch (dir)
            {
                case 'u':
                    player->TeleportTo(mapid, x, y, z + value, o);
                    break;
                case 'd':
                    player->TeleportTo(mapid, x, y, z - value, o);
                    break;
                case 'f':
                {
                    float fx = x + cosf(o) * value;
                    float fy = y + sinf(o) * value; 
                    player->TeleportTo(mapid, fx, fy, z, o);
                    break;
                }
                case 'b':
                {
                    float bx = x - cosf(o) * value;
                    float by = y - sinf(o) * value;
                    player->TeleportTo(mapid, bx, by, z, o);
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        static bool HandleSpeedCommand(ChatHandler* handler, const char* args)
        {
            if (!*args)
                return false;

            char* typeArg = strtok((char*)args, " ");

            if (!typeArg)
                return false;

            uint8 moveType = (uint8)atoi(typeArg);

            if (moveType > MAX_MOVE_TYPE)
                return false;

            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();
            float speed = 0.0f;
            float speedRate = 0.0f;
            std::string moveTypeString = "Unknown";

            switch (moveType)
            {
                case MOVE_WALK:
                    speed = target->GetSpeed(MOVE_WALK);
                    speedRate = target->GetSpeedRate(MOVE_WALK);
                    moveTypeString = "MOVE_WALK";
                    break;
                case MOVE_RUN:
                    speed = target->GetSpeed(MOVE_RUN);
                    speedRate = target->GetSpeedRate(MOVE_RUN);
                    moveTypeString = "MOVE_RUN";
                    break;
                case MOVE_RUN_BACK:
                    speed = target->GetSpeed(MOVE_RUN_BACK);
                    speedRate = target->GetSpeedRate(MOVE_RUN_BACK);
                    moveTypeString = "MOVE_RUN_BACK";
                    break;
                case MOVE_SWIM:
                    speed = target->GetSpeed(MOVE_SWIM);
                    speedRate = target->GetSpeedRate(MOVE_SWIM);
                    moveTypeString = "MOVE_SWIM";
                    break;
                case MOVE_SWIM_BACK:
                    speed = target->GetSpeed(MOVE_SWIM_BACK);
                    speedRate = target->GetSpeedRate(MOVE_SWIM_BACK);
                    moveTypeString = "MOVE_SWIM_BACK";
                    break;
                case MOVE_TURN_RATE:
                    speed = target->GetSpeed(MOVE_TURN_RATE);
                    speedRate = target->GetSpeedRate(MOVE_TURN_RATE);
                    moveTypeString = "MOVE_TURN_RATE";
                    break;
                case MOVE_FLIGHT:
                    speed = target->GetSpeed(MOVE_FLIGHT);
                    speedRate = target->GetSpeedRate(MOVE_FLIGHT);
                    moveTypeString = "MOVE_FLIGHT";
                    break;
                case MOVE_FLIGHT_BACK:
                    speed = target->GetSpeed(MOVE_FLIGHT_BACK);
                    speedRate = target->GetSpeedRate(MOVE_FLIGHT_BACK);
                    moveTypeString = "MOVE_FLIGHT_BACK";
                    break;
                case MOVE_PITCH_RATE:
                    speed = target->GetSpeed(MOVE_PITCH_RATE);
                    speedRate = target->GetSpeedRate(MOVE_PITCH_RATE);
                    moveTypeString = "MOVE_PITCH_RATE";
                    break;
                default:
                    break;
            }

            handler->PSendSysMessage("Target: %s, Move Type: %s", target->GetName(), moveTypeString.c_str());
            handler->PSendSysMessage("Speed: %f", speed);
            handler->PSendSysMessage("Speed Rate: %f", speedRate);
            return true;
        }

        static bool HandleMovementFlagsCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();

            uint32 moveFlags = target->m_movementInfo.GetMovementFlags();
            handler->PSendSysMessage("Target: %s has movementflag(s):", target->GetName());

            if (!moveFlags)
            {
                handler->PSendSysMessage("MOVEMENTFLAG_NONE");
                return true;
            }

            if (moveFlags & MOVEMENTFLAG_FORWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_FORWARD");
            else if (moveFlags & MOVEMENTFLAG_BACKWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_BACKWARD");
            else if (moveFlags & MOVEMENTFLAG_STRAFE_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_STRAFE_LEFT");
            else if (moveFlags & MOVEMENTFLAG_STRAFE_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_STRAFE_RIGHT");
            else if (moveFlags & MOVEMENTFLAG_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_LEFT");
            else if (moveFlags & MOVEMENTFLAG_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_RIGHT");
            else if (moveFlags & MOVEMENTFLAG_PITCH_UP)
                handler->PSendSysMessage("MOVEMENTFLAG_PITCH_UP");
            else if (moveFlags & MOVEMENTFLAG_PITCH_DOWN)
                handler->PSendSysMessage("MOVEMENTFLAG_PITCH_DOWN");
            else if (moveFlags & MOVEMENTFLAG_WALKING)
                handler->PSendSysMessage("MOVEMENTFLAG_WALKING");
            else if (moveFlags & MOVEMENTFLAG_ONTRANSPORT)
                handler->PSendSysMessage("MOVEMENTFLAG_ONTRANSPORT");
            else if (moveFlags & MOVEMENTFLAG_LEVITATING)
                handler->PSendSysMessage("MOVEMENTFLAG_LEVITATING");
            else if (moveFlags & MOVEMENTFLAG_ROOT)
                handler->PSendSysMessage("MOVEMENTFLAG_ROOT");
            else if (moveFlags & MOVEMENTFLAG_JUMPING)
                handler->PSendSysMessage("MOVEMENTFLAG_JUMPING");
            else if (moveFlags & MOVEMENTFLAG_FALLING)
                handler->PSendSysMessage("MOVEMENTFLAG_FALLING");
            else if (moveFlags & MOVEMENTFLAG_PENDING_STOP)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STOP");
            else if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_STOP)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_STOP");
            else if (moveFlags & MOVEMENTFLAG_PENDING_FORWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_FORWARD");
            else if (moveFlags & MOVEMENTFLAG_PENDING_BACKWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_BACKWARD");
            else if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_LEFT");
            else if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_RIGHT");
            else if (moveFlags & MOVEMENTFLAG_PENDING_ROOT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_ROOT");
            else if (moveFlags & MOVEMENTFLAG_SWIMMING)
                handler->PSendSysMessage("MOVEMENTFLAG_SWIMMING");
            else if (moveFlags & MOVEMENTFLAG_ASCENDING)
                handler->PSendSysMessage("MOVEMENTFLAG_ASCENDING");
            else if (moveFlags & MOVEMENTFLAG_DESCENDING)
                handler->PSendSysMessage("MOVEMENTFLAG_DESCENDING");
            else if (moveFlags & MOVEMENTFLAG_CAN_FLY)
                handler->PSendSysMessage("MOVEMENTFLAG_CAN_FLY");
            else if (moveFlags & MOVEMENTFLAG_FLYING)
                handler->PSendSysMessage("MOVEMENTFLAG_FLYING");
            else if (moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION)
                handler->PSendSysMessage("MOVEMENTFLAG_SPLINE_ELEVATION");
            else if (moveFlags & MOVEMENTFLAG_SPLINE_ENABLED)
                handler->PSendSysMessage("MOVEMENTFLAG_SPLINE_ENABLED");
            else if (moveFlags & MOVEMENTFLAG_WATERWALKING)
                handler->PSendSysMessage("MOVEMENTFLAG_WATERWALKING");
            else if (moveFlags & MOVEMENTFLAG_FALLING_SLOW)
                handler->PSendSysMessage("MOVEMENTFLAG_FALLING_SLOW");
            else if (moveFlags & MOVEMENTFLAG_HOVER)
                handler->PSendSysMessage("MOVEMENTFLAG_HOVER");
            else if (moveFlags & MOVEMENTFLAG_MASK_MOVING)
                handler->PSendSysMessage("MOVEMENTFLAG_MASK_MOVING");
            else if (moveFlags & MOVEMENTFLAG_MASK_TURNING)
                handler->PSendSysMessage("MOVEMENTFLAG_MASK_TURNING");

            return true;
        }

        static bool HandleUnitStateCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();

            uint32 unitState = target->GetUnitState();
            if (!unitState)
            {
                handler->PSendSysMessage("Target: %s has no unit states", target->GetName());
                return true;
            }

            handler->PSendSysMessage("Target: %s has unit state(s):", target->GetName());

            if (unitState & UNIT_STATE_DIED)
                handler->PSendSysMessage("UNIT_STATE_DIED");
            else if (unitState & UNIT_STATE_DIED)
                handler->PSendSysMessage("UNIT_STATE_DIED");
            else if (unitState & UNIT_STATE_MELEE_ATTACKING)
                handler->PSendSysMessage("UNIT_STATE_MELEE_ATTACKING");
            else if (unitState & UNIT_STATE_STUNNED)
                handler->PSendSysMessage("UNIT_STATE_STUNNED");
            else if (unitState & UNIT_STATE_ROAMING)
                handler->PSendSysMessage("UNIT_STATE_ROAMING");
            else if (unitState & UNIT_STATE_CHASE)
                handler->PSendSysMessage("UNIT_STATE_CHASE");
            else if (unitState & UNIT_STATE_FLEEING)
                handler->PSendSysMessage("UNIT_STATE_FLEEING");
            else if (unitState & UNIT_STATE_IN_FLIGHT)
                handler->PSendSysMessage("UNIT_STATE_IN_FLIGHT");
            else if (unitState & UNIT_STATE_FOLLOW)
                handler->PSendSysMessage("UNIT_STATE_FOLLOW");
            else if (unitState & UNIT_STATE_ROOT)
                handler->PSendSysMessage("UNIT_STATE_ROOT");
            else if (unitState & UNIT_STATE_CONFUSED)
                handler->PSendSysMessage("UNIT_STATE_CONFUSED");
            else if (unitState & UNIT_STATE_DISTRACTED)
                handler->PSendSysMessage("UNIT_STATE_DISTRACTED");
            else if (unitState & UNIT_STATE_ISOLATED)
                handler->PSendSysMessage("UNIT_STATE_ISOLATED");
            else if (unitState & UNIT_STATE_ATTACK_PLAYER)
                handler->PSendSysMessage("UNIT_STATE_ATTACK_PLAYER");
            else if (unitState & UNIT_STATE_CASTING)
                handler->PSendSysMessage("UNIT_STATE_CASTING");
            else if (unitState & UNIT_STATE_POSSESSED)
                handler->PSendSysMessage("UNIT_STATE_POSSESSED");
            else if (unitState & UNIT_STATE_CHARGING)
                handler->PSendSysMessage("UNIT_STATE_CHARGING");
            else if (unitState & UNIT_STATE_JUMPING)
                handler->PSendSysMessage("UNIT_STATE_JUMPING");
            else if (unitState & UNIT_STATE_ONVEHICLE)
                handler->PSendSysMessage("UNIT_STATE_ONVEHICLE");
            else if (unitState & UNIT_STATE_MOVE)
                handler->PSendSysMessage("UNIT_STATE_MOVE");
            else if (unitState & UNIT_STATE_ROTATING)
                handler->PSendSysMessage("UNIT_STATE_ROTATING");
            else if (unitState & UNIT_STATE_EVADE)
                handler->PSendSysMessage("UNIT_STATE_EVADE");
            else if (unitState & UNIT_STATE_ROAMING_MOVE)
                handler->PSendSysMessage("UNIT_STATE_ROAMING_MOVE");
            else if (unitState & UNIT_STATE_CONFUSED_MOVE)
                handler->PSendSysMessage("UNIT_STATE_CONFUSED_MOVE");
            else if (unitState & UNIT_STATE_FLEEING_MOVE)
                handler->PSendSysMessage("UNIT_STATE_FLEEING_MOVE");
            else if (unitState & UNIT_STATE_CHASE_MOVE)
                handler->PSendSysMessage("UNIT_STATE_CHASE_MOVE");
            else if (unitState & UNIT_STATE_FOLLOW_MOVE)
                handler->PSendSysMessage("UNIT_STATE_FOLLOW_MOVE");
            else if (unitState & UNIT_STATE_IGNORE_PATHFINDING)
                handler->PSendSysMessage("UNIT_STATE_IGNORE_PATHFINDING");
            else if (unitState & UNIT_STATE_UNATTACKABLE)
                handler->PSendSysMessage("UNIT_STATE_UNATTACKABLE");
            else if (unitState & UNIT_STATE_MOVING)
                handler->PSendSysMessage("UNIT_STATE_MOVING");
            else if (unitState & UNIT_STATE_CONTROLLED)
                handler->PSendSysMessage("UNIT_STATE_CONTROLLED");
            else if (unitState & UNIT_STATE_LOST_CONTROL)
                handler->PSendSysMessage("UNIT_STATE_LOST_CONTROL");
            else if (unitState & UNIT_STATE_SIGHTLESS)
                handler->PSendSysMessage("UNIT_STATE_SIGHTLESS");
            else if (unitState & UNIT_STATE_CANNOT_AUTOATTACK)
                handler->PSendSysMessage("UNIT_STATE_CANNOT_AUTOATTACK");
            else if (unitState & UNIT_STATE_CANNOT_TURN)
                handler->PSendSysMessage("UNIT_STATE_CANNOT_TURN");
            else if (unitState & UNIT_STATE_NOT_MOVE)
                handler->PSendSysMessage("UNIT_STATE_NOT_MOVE");
            else if (unitState & UNIT_STATE_ALL_STATE)
                handler->PSendSysMessage("UNIT_STATE_ALL_STATE");

            return true;
        }

        static bool HandleSplineInfoCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();

            handler->PSendSysMessage("Target: %s spline dump:", target->GetName());
            if (target->movespline && target->movespline->Initialized())
                handler->PSendSysMessage(target->movespline->ToString().c_str());
            else
                handler->PSendSysMessage("Spline not initialized for this unit");

            return true;
        }

        static bool HandleSetViewpointCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();

            handler->GetSession()->GetPlayer()->SetViewpoint(target, true);
            return true;
        }

        static bool HandleRestoreViewpointCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (WorldObject* viewpoint = player->GetCurrentViewpoint())
                if (viewpoint != player)
                    player->SetViewpoint(viewpoint, false);
            return true;
        }

        static bool HandleItemIdCommand(ChatHandler* handler, const char* args)
        {
            if (!*args)
                return false;

            uint32 itemId = handler->extractItemIdFromLink((char*)args);
            if (!itemId)
                return false;

            handler->PSendSysMessage("Item Id: %u", itemId);
            return true;
        }

        static bool HandleSpellIdCommand(ChatHandler* handler, const char* args)
        {
            if (!*args)
                return false;

            uint32 spellId = handler->extractSpellIdFromLink((char*)args);

            if (!spellId)
                return false;

            handler->PSendSysMessage("Spell Id: %u", spellId);
            return true;
        }

        static bool HandleCoeffCommand(ChatHandler* handler, const char* args)
        {
            if (!*args)
                return false;

            uint32 spellId = handler->extractSpellIdFromLink((char*)args);

            if (!spellId)
                return false;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                return false;

            float spell_dotDamage = sSpellMgr->GetSpellDotBonus(spellInfo);
            float spell_directDamage = sSpellMgr->GetSpellDirectBonus(spellInfo);
            float spell_apDotBonus = sSpellMgr->GetAPDotBonus(spellInfo);
            float spell_apBonus = sSpellMgr->GetAPBonus(spellInfo);

            handler->PSendSysMessage("Spell Id: %u", spellId);
            handler->PSendSysMessage("Spell Dot Bonus: %f", spell_dotDamage);
            handler->PSendSysMessage("Spell Direct Bonus: %f", spell_directDamage);
            handler->PSendSysMessage("AP Dot Bonus: %f", spell_apDotBonus);
            handler->PSendSysMessage("AP Bonus: %f", spell_apBonus);
            return true;
        }

        static bool HandleBankCommand(ChatHandler* handler, const char* /*args*/)
        {
            handler->GetSession()->SendShowBank(handler->GetSession()->GetPlayer()->GetGUID());
            return true;
        }

        static bool HandleMailboxCommand(ChatHandler* handler, const char* /*args*/)
        {
            WorldPacket data(SMSG_SHOW_MAILBOX, 8);
            data << uint64(handler->GetSession()->GetPlayer()->GetGUID());
            handler->GetSession()->SendPacket(&data);
            return true;
        }

        static bool HandleCommentatorCommand(ChatHandler* handler, const char* /*args*/)
        {
            handler->GetSession()->GetPlayer()->ToggleFlag(PLAYER_FLAGS, 4718592);
            return true;
        }
};

void AddSC_utility_commandscript()
{
    new utility_commandscript();
}
