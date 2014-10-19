#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"

#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"

void WorldSession::HandleMessagechatOpcode(WorldPacket & recv_data)
{
    uint32 type;
    uint32 lang;

    recv_data >> type;
    recv_data >> lang;

    if (lang != LANG_ADDON)
    {
        switch (type)
        {
            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_BATTLEGROUND_LEADER:
                lang = LANG_UNIVERSAL;
                break;
        }
    }

    if (type >= MAX_CHAT_MSG_TYPE)
    {
        sLog->outError("CHAT: Wrong message type received: %u", type);
        recv_data.rfinish();
        return;
    }

    Player* sender = GetPlayer();
    if (!sender)
        return;

    //sLog->outDebug("CHAT: packet received. type %u, lang %u", type, lang);

    // prevent talking at unknown language (cheating)
    LanguageDesc const* langDesc = GetLanguageDescByID(lang);
    if (!langDesc)
    {
        SendNotification(LANG_UNKNOWN_LANGUAGE);
        recv_data.rfinish();
        return;
    }
    if (langDesc->skill_id != 0 && !sender->HasSkill(langDesc->skill_id))
    {
        // also check SPELL_AURA_COMPREHEND_LANGUAGE (client offers option to speak in that language)
        Unit::AuraEffectList const& langAuras = sender->GetAuraEffectsByType(SPELL_AURA_COMPREHEND_LANGUAGE);
        bool foundAura = false;
        for (Unit::AuraEffectList::const_iterator i = langAuras.begin(); i != langAuras.end(); ++i)
        {
            if ((*i)->GetMiscValue() == int32(lang))
            {
                foundAura = true;
                break;
            }
        }
        if (!foundAura)
        {
            SendNotification(LANG_NOT_LEARNED_LANGUAGE);
            recv_data.rfinish();
            return;
        }
    }

    if (lang == LANG_ADDON)
    {
        // LANG_ADDON is only valid for the following message types
        switch (type)
        {
            case CHAT_MSG_PARTY:
            case CHAT_MSG_RAID:
            case CHAT_MSG_GUILD:
            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_WHISPER:
                if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
                {
                    std::string msg = "";
                    recv_data >> msg;

                    if (msg.empty())
                        return;

                    sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), lang, msg);
                }

                // Disabled addon channel?
                if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
                    return;
                break;
            default:
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Player %s (GUID: %u) sent a chatmessage with an invalid language/message type combination",
                    GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());

                recv_data.rfinish();
                return;
        }
    }
    // LANG_ADDON should not be changed nor be affected by flood control
    else
    {
        // send in universal language if player in .gmon mode (ignore spell effects)
        if (sender->HasGameMasterTagOn())
            lang = LANG_UNIVERSAL;
        else
        {
            // send in universal language in two side iteration allowed mode
            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT))
                lang = LANG_UNIVERSAL;
            else
            {
                switch (type)
                {
                    case CHAT_MSG_PARTY:
                    case CHAT_MSG_PARTY_LEADER:
                    case CHAT_MSG_RAID:
                    case CHAT_MSG_RAID_LEADER:
                    case CHAT_MSG_RAID_WARNING:
                        // allow two side chat at group channel if two side group allowed
                        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP))
                            lang = LANG_UNIVERSAL;
                        break;
                    case CHAT_MSG_GUILD:
                    case CHAT_MSG_OFFICER:
                        // allow two side chat at guild channel if two side guild allowed
                        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                            lang = LANG_UNIVERSAL;
                        break;
                }
            }

            // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
            Unit::AuraEffectList const& ModLangAuras = sender->GetAuraEffectsByType(SPELL_AURA_MOD_LANGUAGE);
            if (!ModLangAuras.empty())
                lang = ModLangAuras.front()->GetMiscValue();
        }

        if (!sender->CanSpeak())
        {
            std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
            SendNotification(GetAxiumString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
            recv_data.rfinish(); // Prevent warnings
            return;
        }

        if (type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
            sender->UpdateSpeakTime();
    }

    if (sender->HasAura(1852) && type != CHAT_MSG_WHISPER)
    {
        std::string msg="";
        recv_data >> msg;

        SendNotification(GetAxiumString(LANG_GM_SILENCE), sender->GetName());
        return;
    }

    std::string to, channel, msg;
    bool ignoreChecks = false;
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
            recv_data >> msg;
            break;
        case CHAT_MSG_WHISPER:
            recv_data >> to;
            recv_data >> msg;
            break;
        case CHAT_MSG_CHANNEL:
            recv_data >> channel;
            recv_data >> msg;
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            recv_data >> msg;
            ignoreChecks = true;
            break;
    }

    if (!ignoreChecks)
    {
        if (msg.empty())
            return;

        if (ChatHandler(this).ParseCommands(msg.c_str()))
        {
            if (sender->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
            {
                Unit* selection = ChatHandler(this).getSelectedUnit();
                uint32 target_accid = 0;
                uint32 target_guid = 0;
                uint32 target_typeid = 0;
                if (selection)
                {
                    if (selection->GetTypeId() == TYPEID_PLAYER) // Only players have account id's
                        target_accid = selection->ToPlayer()->GetSession()->GetAccountId();

                    target_guid = selection->GetGUIDLow();
                    target_typeid = uint32(selection->GetTypeId());
                }

                PreparedStatement* stmt = LogDatabase.GetPreparedStatement(LOG_INS_COMMAND_LOG);
                stmt->setUInt32(0, sender->GetSession()->GetAccountId());
                stmt->setUInt32(1, sender->GetGUIDLow());
                stmt->setUInt32(2, target_accid);
                stmt->setUInt32(3, target_guid);
                stmt->setUInt32(4, target_typeid);
                stmt->setString(5, msg.c_str());
                LogDatabase.Execute(stmt);
            }
            return;
        }

        if (lang != LANG_ADDON)
        {
            // Strip invisible characters for non-addon messages
            if (sWorld->getBoolConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING))
                stripLineInvisibleChars(msg);

            if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Player %s (GUID: %u) sent a chatmessage with an invalid link: %s",
                    GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), msg.c_str());

                if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_KICK))
                    CloseSession();

                return;
            }
        }
    }

    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        {
            if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ))
            {
                SendNotification(GetAxiumString(LANG_SAY_REQ), sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ));
                return;
            }

            if (sender->isDead())
                return;

            if (sender->IsArenaSpectator())
                return;

            if (type == CHAT_MSG_SAY)
                sender->Say(msg, lang);
            else if (type == CHAT_MSG_EMOTE)
                sender->TextEmote(msg);
            else if (type == CHAT_MSG_YELL)
                sender->Yell(msg, lang);

            sender->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_WHISPER:
        {
            if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ))
            {
                SendNotification(GetAxiumString(LANG_WHISPER_REQ), sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ));
                return;
            }

            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }

            Player* receiver = sObjectAccessor->FindPlayerByName(to.c_str());
            bool senderIsPlayer = AccountMgr::IsPlayerAccount(GetSecurity());
            bool receiverIsPlayer = AccountMgr::IsPlayerAccount(receiver ? receiver->GetSession()->GetSecurity() : SEC_PLAYER);
            if (!receiver || (senderIsPlayer && !receiverIsPlayer && !receiver->IsAcceptWhispers() && !receiver->IsInWhisperWhiteList(sender->GetGUID())))
            {
                SendPlayerNotFoundNotice(to);
                return;
            }

            if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT) && senderIsPlayer && receiverIsPlayer)
                if (GetPlayer()->GetTeam() != receiver->GetTeam())
                {
                    SendWrongFactionNotice();
                    return;
                }

            if (GetPlayer()->HasAura(1852) && !receiver->HasGameMasterTagOn())
            {
                SendNotification(GetAxiumString(LANG_GM_SILENCE), GetPlayer()->GetName());
                return;
            }

            // If player is a Gamemaster and doesn't accept whisper, we auto-whitelist every player that the Gamemaster is talking to
            if (!senderIsPlayer && !sender->IsAcceptWhispers() && !sender->IsInWhisperWhiteList(receiver->GetGUID()))
                sender->AddWhisperWhiteList(receiver->GetGUID());

            GetPlayer()->Whisper(msg, lang, receiver->GetGUID());
            GetPlayer()->LogChatMessageToDB(msg, type, lang, receiver);
        }
        break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        {
            // if player is in battleground, he cannot say to battleground members by /p
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = _player->GetGroup();
                if (!group)
                    return;

                if (_player->GetBattleground())
                    if (group->isBGGroup() && !_player->GetBattleground()->isArena())
                        return;
            }

            if (type == CHAT_MSG_PARTY_LEADER && !group->IsLeader(_player->GetGUID()))
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, uint8(type), lang, NULL, 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false, group->GetMemberGroup(GetPlayer()->GetGUID()));
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_GUILD:
        {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);

                    guild->BroadcastToGuild(this, false, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                    GetPlayer()->LogChatMessageToDB(msg, type, lang);
                }
            }
        }
        break;
        case CHAT_MSG_OFFICER:
        {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);

                    guild->BroadcastToGuild(this, true, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                    GetPlayer()->LogChatMessageToDB(msg, type, lang);
                }
            }
        }
        break;
        case CHAT_MSG_RAID:
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup())
                    return;
            }

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_RAID_LEADER:
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup() || !group->IsLeader(_player->GetGUID()))
                    return;
            }

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID_LEADER, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_RAID_WARNING:
        {
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() || !(group->IsLeader(GetPlayer()->GetGUID()) || group->IsAssistant(GetPlayer()->GetGUID())) || group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID_WARNING, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_BATTLEGROUND:
        {
            //battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_BATTLEGROUND, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_BATTLEGROUND_LEADER:
        {
            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup() || !group->IsLeader(GetPlayer()->GetGUID()))
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_BATTLEGROUND_LEADER, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            GetPlayer()->LogChatMessageToDB(msg, type, lang);
        }
        break;
        case CHAT_MSG_CHANNEL:
        {
            if (AccountMgr::IsPlayerAccount(GetSecurity()))
            {
                if (_player->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ))
                {
                    SendNotification(GetAxiumString(LANG_CHANNEL_REQ), sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ));
                    return;
                }
            }

            if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
            {

                if (Channel* chn = cMgr->GetChannel(channel, _player))
                {
                    sScriptMgr->OnPlayerChat(_player, type, lang, msg, chn);

                    chn->Say(_player->GetGUID(), msg.c_str(), lang);
                    GetPlayer()->LogChatMessageToDB(msg, type, lang);
                }
            }
        }
        break;
        case CHAT_MSG_AFK:
        {
            if ((msg.empty() || !_player->isAFK()) && !_player->isInCombat())
            {
                if (!_player->isAFK())
                {
                    if (msg.empty())
                        msg  = GetAxiumString(LANG_PLAYER_AFK_DEFAULT);
                    _player->afkMsg = msg;
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);
                GetPlayer()->LogChatMessageToDB(msg, type, lang);

                _player->ToggleAFK();
                if (_player->isAFK() && _player->isDND())
                    _player->ToggleDND();
            }
        }
        break;
        case CHAT_MSG_DND:
        {
            if (msg.empty() || !_player->isDND())
            {
                if (!_player->isDND())
                {
                    if (msg.empty())
                        msg = GetAxiumString(LANG_PLAYER_DND_DEFAULT);
                    _player->dndMsg = msg;
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);
                GetPlayer()->LogChatMessageToDB(msg, type, lang);

                _player->ToggleDND();
                if (_player->isDND() && _player->isAFK())
                    _player->ToggleAFK();
            }
        }
        break;
        default:
            sLog->outError("CHAT: unknown message type %u, lang: %u", type, lang);
            break;
    }
}

void WorldSession::HandleEmoteOpcode(WorldPacket & recv_data)
{
    if (!_player->isAlive() || _player->HasUnitState(UNIT_STATE_DIED))
        return;

    uint32 emote;
    recv_data >> emote;
    sScriptMgr->OnPlayerEmote(GetPlayer(), emote);
    _player->HandleEmoteCommand(emote);
}

namespace Axium
{
    class EmoteChatBuilder
    {
        public:
            EmoteChatBuilder(Player const& player, uint32 text_emote, uint32 emote_num, Unit const* target)
                : i_player(player), i_text_emote(text_emote), i_emote_num(emote_num), i_target(target) {}

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* nam = i_target ? i_target->GetNameForLocaleIdx(loc_idx) : NULL;
                uint32 namlen = (nam ? strlen(nam) : 0) + 1;

                data.Initialize(SMSG_TEXT_EMOTE, (20+namlen));
                data << i_player.GetGUID();
                data << (uint32)i_text_emote;
                data << i_emote_num;
                data << (uint32)namlen;
                if (namlen > 1)
                    data.append(nam, namlen);
                else
                    data << (uint8)0x00;
            }

        private:
            Player const& i_player;
            uint32        i_text_emote;
            uint32        i_emote_num;
            Unit const*   i_target;
    };
}                                                           // namespace Axium

void WorldSession::HandleTextEmoteOpcode(WorldPacket & recv_data)
{
    if (!_player->isAlive())
        return;

    if (!_player->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetAxiumString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 text_emote, emoteNum;
    uint64 guid;

    recv_data >> text_emote;
    recv_data >> emoteNum;
    recv_data >> guid;


    if (_player->lastEmoteTime > getMSTime())
        return;

    if (_player->IsArenaSpectator())
        return;

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(text_emote);
    if (!em)
        return;

    uint32 emote_anim = em->textid;

    switch (emote_anim)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
            // Only allow text-emotes for "dead" entities (feign death included)
            if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
                break;
             GetPlayer()->HandleEmoteCommand(emote_anim);
             break;
    }

    Unit* unit = ObjectAccessor::GetUnit(*_player, guid);
    CellCoord p = Axium::ComputeCellCoord(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Axium::EmoteChatBuilder emote_builder(*GetPlayer(), text_emote, emoteNum, unit);
    Axium::LocalizedPacketDo<Axium::EmoteChatBuilder > emote_do(emote_builder);
    Axium::PlayerDistWorker<Axium::LocalizedPacketDo<Axium::EmoteChatBuilder > > emote_worker(GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), emote_do);
    TypeContainerVisitor<Axium::PlayerDistWorker<Axium::LocalizedPacketDo<Axium::EmoteChatBuilder> >, WorldTypeMapContainer> message(emote_worker);
    cell.Visit(p, message, *GetPlayer()->GetMap(), *GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));
    _player->lastEmoteTime = getMSTime() + 1000;
    sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);

    GetPlayer()->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE, text_emote, 0, unit);

    //Send scripted event call
    if (unit && unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), text_emote);
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recv_data)
{
    uint64 iguid;
    uint8 unk;
    //sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: Received CMSG_CHAT_IGNORED");

    recv_data >> iguid;
    recv_data >> unk;                                       // probably related to spam reporting

    Player* player = ObjectAccessor::FindPlayer(iguid);
    if (!player || !player->GetSession())
        return;

    WorldPacket data;
    ChatHandler::FillMessageData(&data, this, CHAT_MSG_IGNORED, LANG_UNIVERSAL, NULL, GetPlayer()->GetGUID(), GetPlayer()->GetName(), NULL);
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleChannelDeclineInvite(WorldPacket &recvPacket)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Opcode %u", recvPacket.GetOpcode());
}

void WorldSession::SendPlayerNotFoundNotice(std::string name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendPlayerAmbiguousNotice(std::string name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_AMBIGUOUS, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendWrongFactionNotice()
{
    WorldPacket data(SMSG_CHAT_WRONG_FACTION, 0);
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice(ChatRestrictionType restriction)
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 1);
    data << uint8(restriction);
    SendPacket(&data);
}
