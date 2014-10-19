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
                { "customize",        SEC_PLAYER,         false, &HandleCustomizeCommand,                 "", NULL },
                { "race",             SEC_PLAYER,         false, &HandleRaceCommand,                      "", NULL },
                { "faction",          SEC_PLAYER,         false, &HandleFactionCommand,                   "", NULL },
                { "prepare",          SEC_GAMEMASTER,     false, &HandlePrepareCommand,                   "", NULL },
                { "barbershop",       SEC_PLAYER,         false, &HandleBarbershopCommand,                "", NULL },
                { "sendcooldown",     SEC_GAMEMASTER,     false, &HandleSendCooldownCommand,              "", NULL },
                { "transmogcopygear", SEC_GAMEMASTER,     false, &HandleTransmogCopyGearCommand,          "", NULL },
                { "transmogsendgear", SEC_GAMEMASTER,     false, &HandleTransmogSendGearCommand,          "", NULL },
                { "warp",             SEC_GAMEMASTER,     false, &HandleWarpCommand,                      "", NULL },
                { "setviewpoint",     SEC_GAMEMASTER,     false, &HandleSetViewpointCommand,              "", NULL },
                { "restoreviewpoint", SEC_GAMEMASTER,     false, &HandleRestoreViewpointCommand,          "", NULL },
                { "speed",            SEC_GAMEMASTER,     false, &HandleSpeedCommand,                     "", NULL },
                { "movementflags",    SEC_GAMEMASTER,     false, &HandleMovementFlagsCommand,             "", NULL },
                { "unitstate",        SEC_GAMEMASTER,     false, &HandleUnitStateCommand,                 "", NULL },
                { "splineinfo",       SEC_GAMEMASTER,     false, &HandleSplineInfoCommand,                "", NULL },
                { "itemid",           SEC_GAMEMASTER,     false, &HandleItemIdCommand,                    "", NULL },
                { "spellid",          SEC_GAMEMASTER,     false, &HandleSpellIdCommand,                   "", NULL },
                { "coeff",            SEC_GAMEMASTER,     false, &HandleCoeffCommand,                     "", NULL },
                { "bank",             SEC_PLAYER,         false, &HandleBankCommand,                      "", NULL },
                { "mailbox",          SEC_PLAYER,         false, &HandleMailboxCommand,                   "", NULL },
                { "commentator",      SEC_GAMEMASTER,     false, &HandleCommentatorCommand,               "", NULL },
                { NULL,               0,                  false, NULL,                                    "", NULL }
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

        static bool HandleTransmogCopyGearCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit *unitTarget = handler->getSelectedUnit();

            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            {
                handler->PSendSysMessage("Target is not a player.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            Player* target = unitTarget->ToPlayer();

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
            {
                if (Item* playerItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    if (Item* targetItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
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
                            if (targetItem->TransmogEntry)
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEntry = %u WHERE guid = %u", targetItem->TransmogEntry, playerItem->GetGUIDLow());
                                playerItem->TransmogEntry = targetItem->TransmogEntry;
                                player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), targetItem->TransmogEntry);
                            }
                            else
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEntry = %u WHERE guid = %u", targetItem->GetEntry(), playerItem->GetGUIDLow());
                                playerItem->TransmogEntry = targetItem->GetEntry();
                                player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), targetItem->GetEntry());
                            }

                            if (targetItem->TransmogEnchant)
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEnchant = %u WHERE guid = %u", targetItem->TransmogEnchant, playerItem->GetGUIDLow());
                                playerItem->TransmogEnchant = targetItem->TransmogEnchant;
                                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, targetItem->TransmogEnchant);
                                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 1, targetItem->TransmogEnchant);
                            }
                        }
                    }
            }
            return true;
        }

        static bool HandleTransmogSendGearCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit *unitTarget = handler->getSelectedUnit();

            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            {
                handler->PSendSysMessage("Target is not a player.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            Player* target = unitTarget->ToPlayer();

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
            {
                if (Item* playerItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    if (Item* targetItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
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
                            if (playerItem->TransmogEntry)
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEntry = %u WHERE guid = %u", playerItem->TransmogEntry, targetItem->GetGUIDLow());
                                targetItem->TransmogEntry = playerItem->TransmogEntry;
                                target->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), playerItem->TransmogEntry);
                            }
                            else
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEntry = %u WHERE guid = %u", playerItem->GetEntry(), targetItem->GetGUIDLow());
                                targetItem->TransmogEntry = playerItem->GetEntry();
                                target->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), playerItem->GetEntry());
                            }

                            if (playerItem->TransmogEnchant)
                            {
                                CharacterDatabase.PExecute("UPDATE item_instance SET TransmogEnchant = %u WHERE guid = %u", playerItem->TransmogEnchant, playerItem->GetGUIDLow());
                                targetItem->TransmogEnchant = playerItem->TransmogEnchant;
                                target->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, playerItem->TransmogEnchant);
                                target->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 1, playerItem->TransmogEnchant);
                            }
                        }
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
                case MOVE_WALK:        speed = target->GetSpeed(MOVE_WALK);        speedRate = target->GetSpeedRate(MOVE_WALK);        moveTypeString = "MOVE_WALK";        break;
                case MOVE_RUN:         speed = target->GetSpeed(MOVE_RUN);         speedRate = target->GetSpeedRate(MOVE_RUN);         moveTypeString = "MOVE_RUN";         break;
                case MOVE_RUN_BACK:    speed = target->GetSpeed(MOVE_RUN_BACK);    speedRate = target->GetSpeedRate(MOVE_RUN_BACK);    moveTypeString = "MOVE_RUN_BACK";    break;
                case MOVE_SWIM:        speed = target->GetSpeed(MOVE_SWIM);        speedRate = target->GetSpeedRate(MOVE_SWIM);        moveTypeString = "MOVE_SWIM";        break;
                case MOVE_SWIM_BACK:   speed = target->GetSpeed(MOVE_SWIM_BACK);   speedRate = target->GetSpeedRate(MOVE_SWIM_BACK);   moveTypeString = "MOVE_SWIM_BACK";   break;
                case MOVE_TURN_RATE:   speed = target->GetSpeed(MOVE_TURN_RATE);   speedRate = target->GetSpeedRate(MOVE_TURN_RATE);   moveTypeString = "MOVE_TURN_RATE";   break;
                case MOVE_FLIGHT:      speed = target->GetSpeed(MOVE_FLIGHT);      speedRate = target->GetSpeedRate(MOVE_FLIGHT);      moveTypeString = "MOVE_FLIGHT";      break;
                case MOVE_FLIGHT_BACK: speed = target->GetSpeed(MOVE_FLIGHT_BACK); speedRate = target->GetSpeedRate(MOVE_FLIGHT_BACK); moveTypeString = "MOVE_FLIGHT_BACK"; break;
                case MOVE_PITCH_RATE:  speed = target->GetSpeed(MOVE_PITCH_RATE);  speedRate = target->GetSpeedRate(MOVE_PITCH_RATE);  moveTypeString = "MOVE_PITCH_RATE";  break;
                default: break;
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

            if (moveFlags & MOVEMENTFLAG_BACKWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_BACKWARD");

            if (moveFlags & MOVEMENTFLAG_STRAFE_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_STRAFE_LEFT");

            if (moveFlags & MOVEMENTFLAG_STRAFE_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_STRAFE_RIGHT");

            if (moveFlags & MOVEMENTFLAG_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_LEFT");

            if (moveFlags & MOVEMENTFLAG_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_RIGHT");

            if (moveFlags & MOVEMENTFLAG_PITCH_UP)
                handler->PSendSysMessage("MOVEMENTFLAG_PITCH_UP");

            if (moveFlags & MOVEMENTFLAG_PITCH_DOWN)
                handler->PSendSysMessage("MOVEMENTFLAG_PITCH_DOWN");

            if (moveFlags & MOVEMENTFLAG_WALKING)
                handler->PSendSysMessage("MOVEMENTFLAG_WALKING");

            if (moveFlags & MOVEMENTFLAG_ONTRANSPORT)
                handler->PSendSysMessage("MOVEMENTFLAG_ONTRANSPORT");

            if (moveFlags & MOVEMENTFLAG_LEVITATING)
                handler->PSendSysMessage("MOVEMENTFLAG_LEVITATING");

            if (moveFlags & MOVEMENTFLAG_ROOT)
                handler->PSendSysMessage("MOVEMENTFLAG_ROOT");

            if (moveFlags & MOVEMENTFLAG_JUMPING)
                handler->PSendSysMessage("MOVEMENTFLAG_JUMPING");

            if (moveFlags & MOVEMENTFLAG_FALLING)
                handler->PSendSysMessage("MOVEMENTFLAG_FALLING");

            if (moveFlags & MOVEMENTFLAG_PENDING_STOP)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STOP");

            if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_STOP)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_STOP");

            if (moveFlags & MOVEMENTFLAG_PENDING_FORWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_FORWARD");

            if (moveFlags & MOVEMENTFLAG_PENDING_BACKWARD)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_BACKWARD");

            if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_LEFT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_LEFT");

            if (moveFlags & MOVEMENTFLAG_PENDING_STRAFE_RIGHT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_STRAFE_RIGHT");

            if (moveFlags & MOVEMENTFLAG_PENDING_ROOT)
                handler->PSendSysMessage("MOVEMENTFLAG_PENDING_ROOT");

            if (moveFlags & MOVEMENTFLAG_SWIMMING)
                handler->PSendSysMessage("MOVEMENTFLAG_SWIMMING");

            if (moveFlags & MOVEMENTFLAG_ASCENDING)
                handler->PSendSysMessage("MOVEMENTFLAG_ASCENDING");

            if (moveFlags & MOVEMENTFLAG_DESCENDING)
                handler->PSendSysMessage("MOVEMENTFLAG_DESCENDING");

            if (moveFlags & MOVEMENTFLAG_CAN_FLY)
                handler->PSendSysMessage("MOVEMENTFLAG_CAN_FLY");

            if (moveFlags & MOVEMENTFLAG_FLYING)
                handler->PSendSysMessage("MOVEMENTFLAG_FLYING");

            if (moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION)
                handler->PSendSysMessage("MOVEMENTFLAG_SPLINE_ELEVATION");

            if (moveFlags & MOVEMENTFLAG_SPLINE_ENABLED)
                handler->PSendSysMessage("MOVEMENTFLAG_SPLINE_ENABLED");

            if (moveFlags & MOVEMENTFLAG_WATERWALKING)
                handler->PSendSysMessage("MOVEMENTFLAG_WATERWALKING");

            if (moveFlags & MOVEMENTFLAG_FALLING_SLOW)
                handler->PSendSysMessage("MOVEMENTFLAG_FALLING_SLOW");

            if (moveFlags & MOVEMENTFLAG_HOVER)
                handler->PSendSysMessage("MOVEMENTFLAG_HOVER");

            return true;
        }

        static bool HandleUnitStateCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();

            handler->PSendSysMessage("Target: %s has unit state(s):", target->GetName());

            if (target->HasUnitState(UNIT_STATE_DIED))
                handler->PSendSysMessage("UNIT_STATE_DIED");

            if (target->HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                handler->PSendSysMessage("UNIT_STATE_MELEE_ATTACKING");

            if (target->HasUnitState(UNIT_STATE_STUNNED))
                handler->PSendSysMessage("UNIT_STATE_STUNNED");

            if (target->HasUnitState(UNIT_STATE_ROAMING))
                handler->PSendSysMessage("UNIT_STATE_ROAMING");

            if (target->HasUnitState(UNIT_STATE_CHASE))
                handler->PSendSysMessage("UNIT_STATE_CHASE");

            if (target->HasUnitState(UNIT_STATE_FLEEING))
                handler->PSendSysMessage("UNIT_STATE_FLEEING");

            if (target->HasUnitState(UNIT_STATE_IN_FLIGHT))
                handler->PSendSysMessage("UNIT_STATE_IN_FLIGHT");

            if (target->HasUnitState(UNIT_STATE_FOLLOW))
                handler->PSendSysMessage("UNIT_STATE_FOLLOW");

            if (target->HasUnitState(UNIT_STATE_ROOT))
                handler->PSendSysMessage("UNIT_STATE_ROOT");

            if (target->HasUnitState(UNIT_STATE_CONFUSED))
                handler->PSendSysMessage("UNIT_STATE_CONFUSED");

            if (target->HasUnitState(UNIT_STATE_DISTRACTED))
                handler->PSendSysMessage("UNIT_STATE_DISTRACTED");

            if (target->HasUnitState(UNIT_STATE_ISOLATED))
                handler->PSendSysMessage("UNIT_STATE_ISOLATED");

            if (target->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
                handler->PSendSysMessage("UNIT_STATE_ATTACK_PLAYER");

            if (target->HasUnitState(UNIT_STATE_CASTING))
                handler->PSendSysMessage("UNIT_STATE_CASTING");

            if (target->HasUnitState(UNIT_STATE_POSSESSED))
                handler->PSendSysMessage("UNIT_STATE_POSSESSED");

            if (target->HasUnitState(UNIT_STATE_CHARGING))
                handler->PSendSysMessage("UNIT_STATE_CHARGING");

            if (target->HasUnitState(UNIT_STATE_JUMPING))
                handler->PSendSysMessage("UNIT_STATE_JUMPING");

            if (target->HasUnitState(UNIT_STATE_ONVEHICLE))
                handler->PSendSysMessage("UNIT_STATE_ONVEHICLE");

            if (target->HasUnitState(UNIT_STATE_MOVE))
                handler->PSendSysMessage("UNIT_STATE_MOVE");

            if (target->HasUnitState(UNIT_STATE_ROTATING))
                handler->PSendSysMessage("UNIT_STATE_ROTATING");

            if (target->HasUnitState(UNIT_STATE_EVADE))
                handler->PSendSysMessage("UNIT_STATE_EVADE");

            if (target->HasUnitState(UNIT_STATE_ROAMING_MOVE))
                handler->PSendSysMessage("UNIT_STATE_ROAMING_MOVE");

            if (target->HasUnitState(UNIT_STATE_CONFUSED_MOVE))
                handler->PSendSysMessage("UNIT_STATE_CONFUSED_MOVE");

            if (target->HasUnitState(UNIT_STATE_FLEEING_MOVE))
                handler->PSendSysMessage("UNIT_STATE_FLEEING_MOVE");

            if (target->HasUnitState(UNIT_STATE_CHASE_MOVE))
                handler->PSendSysMessage("UNIT_STATE_CHASE_MOVE");

            if (target->HasUnitState(UNIT_STATE_FOLLOW_MOVE))
                handler->PSendSysMessage("UNIT_STATE_FOLLOW_MOVE");

            if (target->HasUnitState(UNIT_STATE_IGNORE_PATHFINDING))
                handler->PSendSysMessage("UNIT_STATE_IGNORE_PATHFINDING");

            if (target->HasUnitState(UNIT_STATE_UNATTACKABLE))
                handler->PSendSysMessage("UNIT_STATE_UNATTACKABLE");

            if (target->HasUnitState(UNIT_STATE_MOVING))
                handler->PSendSysMessage("UNIT_STATE_MOVING");

            if (target->HasUnitState(UNIT_STATE_CONTROLLED))
                handler->PSendSysMessage("UNIT_STATE_CONTROLLED");

            if (target->HasUnitState(UNIT_STATE_LOST_CONTROL))
                handler->PSendSysMessage("UNIT_STATE_LOST_CONTROL");

            if (target->HasUnitState(UNIT_STATE_SIGHTLESS))
                handler->PSendSysMessage("UNIT_STATE_SIGHTLESS");

            if (target->HasUnitState(UNIT_STATE_CANNOT_AUTOATTACK))
                handler->PSendSysMessage("UNIT_STATE_CANNOT_AUTOATTACK");

            if (target->HasUnitState(UNIT_STATE_CANNOT_TURN))
                handler->PSendSysMessage("UNIT_STATE_CANNOT_TURN");

            if (target->HasUnitState(UNIT_STATE_NOT_MOVE))
                handler->PSendSysMessage("UNIT_STATE_NOT_MOVE");

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

            uint32 itemId = 0;

            if (args[0] == '[')
            {
                char* citemName = strtok((char*)args, "]");

                if (citemName && citemName[0])
                {
                    std::string itemName = citemName+1;
                    WorldDatabase.EscapeString(itemName);
                    QueryResult result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
                    if (!result)
                        return false;

                    itemId = result->Fetch()->GetUInt16();
                }
                else
                    return false;
            }
            else
            {
                char* cId = handler->extractKeyFromLink((char*)args, "Hitem");
                if (!cId)
                    return false;
                itemId = atol(cId);
            }

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
