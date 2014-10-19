#include "ScriptLoader.h"

// spells
void AddSC_deathknight_spell_scripts();
void AddSC_druid_spell_scripts();
void AddSC_generic_spell_scripts();
void AddSC_hunter_spell_scripts();
void AddSC_mage_spell_scripts();
void AddSC_paladin_spell_scripts();
void AddSC_priest_spell_scripts();
void AddSC_rogue_spell_scripts();
void AddSC_shaman_spell_scripts();
void AddSC_warlock_spell_scripts();
void AddSC_warrior_spell_scripts();
void AddSC_quest_spell_scripts();
void AddSC_item_spell_scripts();
void AddSC_holiday_spell_scripts();

void AddSC_SmartSCripts();

//Commands
void AddSC_account_commandscript();
void AddSC_achievement_commandscript();
void AddSC_arena_commandscript();
void AddSC_challenge_commandscript();
void AddSC_debug_commandscript();
void AddSC_event_commandscript();
void AddSC_gm_commandscript();
void AddSC_go_commandscript();
void AddSC_gobject_commandscript();
void AddSC_honor_commandscript();
void AddSC_learn_commandscript();
void AddSC_misc_commandscript();
void AddSC_modify_commandscript();
void AddSC_npc_commandscript();
void AddSC_quest_commandscript();
void AddSC_reload_commandscript();
void AddSC_tele_commandscript();
void AddSC_ticket_commandscript();
void AddSC_titles_commandscript();
void AddSC_wp_commandscript();
void AddSC_gps_commandscript();
void AddSC_morph_commandscript();
void AddSC_utility_commandscript();
void AddSC_group_commandscript();
void AddSC_mmaps_commandscript();
void AddSC_spectate_commandscript();

#ifdef SCRIPTS
//world
void AddSC_areatrigger_scripts();
void AddSC_generic_creature();
void AddSC_go_scripts();
void AddSC_guards();
void AddSC_item_scripts();
void AddSC_npc_professions();
void AddSC_npc_innkeeper();
void AddSC_npcs_special();
void AddSC_npc_taxi();
void AddSC_achievement_scripts();

void AddSC_alterac_valley();
void AddSC_isle_of_conquest();


// battlegrounds

// outdoor pvp
void AddSC_outdoorpvp_ep();
void AddSC_outdoorpvp_hp();
void AddSC_outdoorpvp_na();
void AddSC_outdoorpvp_si();
void AddSC_outdoorpvp_tf();
void AddSC_outdoorpvp_zm();

// player
void AddSC_chat_log();

#endif

void AddScripts()
{
    AddSpellScripts();
    AddSC_SmartSCripts();
    AddCommandScripts();
#ifdef SCRIPTS
    AddWorldScripts();
    AddEasternKingdomsScripts();
    AddKalimdorScripts();
    AddOutlandScripts();
    AddNorthrendScripts();
    AddBattlegroundScripts();
    AddOutdoorPvPScripts();
    AddCustomScripts();
#endif
}

void AddSpellScripts()
{
    AddSC_deathknight_spell_scripts();
    AddSC_druid_spell_scripts();
    AddSC_generic_spell_scripts();
    AddSC_hunter_spell_scripts();
    AddSC_mage_spell_scripts();
    AddSC_paladin_spell_scripts();
    AddSC_priest_spell_scripts();
    AddSC_rogue_spell_scripts();
    AddSC_shaman_spell_scripts();
    AddSC_warlock_spell_scripts();
    AddSC_warrior_spell_scripts();
    AddSC_quest_spell_scripts();
    AddSC_item_spell_scripts();
    AddSC_holiday_spell_scripts();
}

void AddCommandScripts()
{
    AddSC_account_commandscript();
    AddSC_achievement_commandscript();
    AddSC_arena_commandscript();
    AddSC_challenge_commandscript();
    AddSC_debug_commandscript();
    AddSC_event_commandscript();
    AddSC_gm_commandscript();
    AddSC_go_commandscript();
    AddSC_gobject_commandscript();
    AddSC_honor_commandscript();
    AddSC_learn_commandscript();
    AddSC_misc_commandscript();
    AddSC_modify_commandscript();
    AddSC_npc_commandscript();
    AddSC_quest_commandscript();
    AddSC_reload_commandscript();
    AddSC_tele_commandscript();
    AddSC_ticket_commandscript();
    AddSC_titles_commandscript();
    AddSC_wp_commandscript();
    AddSC_gps_commandscript();
    AddSC_morph_commandscript();
    AddSC_utility_commandscript();
    AddSC_group_commandscript();
    AddSC_mmaps_commandscript();
    AddSC_spectate_commandscript();
}

void AddWorldScripts()
{
#ifdef SCRIPTS
    AddSC_areatrigger_scripts();
    AddSC_generic_creature();
    AddSC_go_scripts();
    AddSC_guards();
    AddSC_item_scripts();
    AddSC_npc_professions();
    AddSC_npc_innkeeper();
    AddSC_npcs_special();
    AddSC_npc_taxi();
    AddSC_achievement_scripts();
    AddSC_chat_log();
#endif
}

void AddEasternKingdomsScripts()
{
#ifdef SCRIPTS
    AddSC_alterac_valley();                 //Alterac Valley
#endif
}

void AddKalimdorScripts()
{
#ifdef SCRIPTS

#endif
}

void AddOutlandScripts()
{
#ifdef SCRIPTS

#endif
}

void AddNorthrendScripts()
{
#ifdef SCRIPTS
    AddSC_isle_of_conquest();
#endif
}

void AddOutdoorPvPScripts()
{
#ifdef SCRIPTS
    AddSC_outdoorpvp_ep();
    AddSC_outdoorpvp_hp();
    AddSC_outdoorpvp_na();
    AddSC_outdoorpvp_si();
    AddSC_outdoorpvp_tf();
    AddSC_outdoorpvp_zm();
#endif
}

void AddBattlegroundScripts()
{
#ifdef SCRIPTS
#endif
}

// Custom
#ifdef SCRIPTS
    void AddSC_Transmogrifier();
    void AddSC_MallInOne();
    void AddSC_ArenaMaster();
    void AddSC_ReadyMarker();
    void AddSC_Teleporter();
    void AddSC_PlayerHandbook();
    void AddSC_VanityRewards();
#endif

void AddCustomScripts()
{
#ifdef SCRIPTS
    AddSC_Transmogrifier();
    AddSC_MallInOne();
    AddSC_ArenaMaster();
    AddSC_ReadyMarker();
    AddSC_Teleporter();
    AddSC_PlayerHandbook();
    AddSC_VanityRewards();
#endif
}
