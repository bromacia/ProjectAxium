#include "DuelMgr.h"
#include "Player.h"
#include "Unit.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellAuras.h"

DuelMgr::DuelMgr()
{
}

DuelMgr::~DuelMgr()
{
}

void DuelMgr::HandleDuelStart(Player* initiator, Player* opponent)
{
    if (!initiator || !opponent)
        return;

    Player* players[2] = { initiator, opponent };

    for (uint8 i = 0; i < 2; ++i)
    {
        ResetPlayer(players[i]);
        players[i]->SetIsDueling(true);

        if (Pet* pet = players[i]->GetPet())
        {
            pet->SetIsDueling(true);
            ResetPet(pet, players[i]);
        }
    }
}

void DuelMgr::HandleDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
{
    if (!winner || !loser)
        return;

    Player* players[2] = { winner, loser };

    for (uint8 i = 0; i < 2; ++i)
    {
        players[i]->SetIsDueling(false);
        ResetPlayer(players[i]);

        if (Pet* pet = players[i]->GetPet())
        {
            pet->SetIsDueling(false);
            ResetPet(pet, players[i]);
        }
    }
}

void DuelMgr::ResetPlayer(Player* player)
{
    if (!player)
        return;

    player->CombatStop();
    player->SetFullHealth();
    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
    player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));
    player->SetPower(POWER_RAGE, 0);
    player->SetPower(POWER_RUNIC_POWER, 0);
    player->RemoveAllPlayerSpellCooldowns();
    player->RemoveAllNegativeAuras();
    player->ClearDiminishings();
    player->ClearComboPoints();
    player->RemoveAllTempSummons();
    player->getHostileRefManager().deleteReferences();
}

void DuelMgr::ResetPet(Pet* pet, Player* owner)
{
    if (!pet)
        return;

    pet->CombatStop();
    pet->SetFullHealth();
    pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
    pet->SetPower(POWER_FOCUS, pet->GetMaxPower(POWER_FOCUS));
    pet->RemoveAllPetSpellCooldowns(owner);
    pet->RemoveAllNegativeAuras();
    pet->ClearDiminishings();
    pet->ClearInCombat();
    pet->getHostileRefManager().deleteReferences();
}
