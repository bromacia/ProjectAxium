#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "DBCStores.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "World.h"
#include "Util.h"
#include "Group.h"
#include "SpellInfo.h"

int PetAI::Permissible(const Creature* creature)
{
    if (creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature* c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK)
{
    m_AllySet.clear();
    UpdateAllies();
}

void PetAI::EnterEvadeMode()
{
}

bool PetAI::_needToStop()
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if (me->isCharmed() && me->getVictim() == me->GetCharmer())
        return true;

    return !me->IsValidAttackTarget(me->getVictim());
}

void PetAI::_stopAttack()
{
    if (!me->isAlive())
    {
        sLog->outStaticDebug("Creature stopped attacking because he is dead [guid=%u]", me->GetGUIDLow());
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop();
        me->getHostileRefManager().deleteReferences();
        return;
    }

    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop(); // Should stop pet's attack button from flashing
    me->GetCharmInfo()->SetIsCommandAttack(false);
    HandleReturnMovement();
}

void PetAI::UpdateAI(const uint32 diff)
{
    if (!me->isAlive())
        return;

    Unit* owner = me->GetCharmerOrOwner();
    if (!owner)
        return;

    CharmInfo* ci = me->GetCharmInfo();
    if (!ci)
        return;

    if (m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    if (Spell* spell = me->getQueuedSpell())
    {
        if (uint64 targetGuid = me->getQueuedSpellTargetGuid())
        {
            if (Unit* queuedTarget = ObjectAccessor::FindUnit(targetGuid))
            {
                if (!queuedTarget)
                {
                    spell->finish(false);
                    me->setQueuedSpell(NULL);
                    me->setQueuedSpellTargetGuid(0);
                    me->setIsRunningToTarget(false);
                    HandleReturnMovement();
                }
                else
                {
                    if (me->isRunningToTarget()) // Is running to a target to cast a spell
                    {
                        SpellCastResult result = spell->CheckPetCast(queuedTarget);

                        if (result == SPELL_CAST_OK)
                        {
                            me->GetMotionMaster()->Clear();
                            if (spell->GetSpellInfo()->CastTimeEntry->CastTime > 0)
                                me->StopMoving();

                            me->ToCreature()->AddCreatureSpellCooldown(spell->GetSpellInfo()->Id);
                            spell->prepare(&(spell->m_targets));

                            if (me->GetReactState() != REACT_PASSIVE)
                                if (me->ToCreature()->IsAIEnabled)
                                    me->ToCreature()->AI()->AttackStart(queuedTarget);

                            me->setQueuedSpell(NULL);
                            me->setQueuedSpellTargetGuid(0);
                            me->setIsRunningToTarget(false);
                        }
                        else if (result != SPELL_FAILED_OUT_OF_RANGE && result != SPELL_FAILED_LINE_OF_SIGHT)
                        {
                            spell->finish(false);
                            me->setQueuedSpell(NULL);
                            me->setQueuedSpellTargetGuid(0);
                            me->setIsRunningToTarget(false);
                            me->AttackStop();
                            HandleReturnMovement();
                        }
                    }
                    else // Has a spell in queue, but not running to target
                    {
                        SpellCastResult result = spell->CheckPetCast(queuedTarget);

                        if (result == SPELL_CAST_OK)
                        {
                            me->ToCreature()->AddCreatureSpellCooldown(spell->GetSpellInfo()->Id);
                            spell->prepare(&(spell->m_targets));
                            me->setQueuedSpell(NULL);
                            me->setQueuedSpellTargetGuid(0);
                        }
                    }
                }
            }
        }
    }

    if (me->getVictim() && me->getVictim()->isAlive())
    {
        if (!owner->canSeeOrDetect(me->getVictim()))
            _stopAttack();

        DoMeleeAttackIfReady();
    }
    else
    {
        if (me->HasReactState(REACT_AGGRESSIVE))
        {
            Unit* nextTarget = SelectNextTarget();

            if (nextTarget)
                AttackStart(nextTarget);
            else
            {
                me->GetCharmInfo()->SetIsCommandAttack(false);
                HandleReturnMovement();
            }
        }
    }

    // Autocast (casted only in combat or persistent spells in any state)
    if (!me->HasUnitState(UNIT_STATE_CASTING))
    {
        if (me->getQueuedSpell() != NULL)
            if (!me->isRunningToTarget())
                return;

        typedef std::vector<std::pair<Unit*, Spell*> > TargetSpellList;
        TargetSpellList targetSpellStore;

        for (uint8 i = 0; i < me->GetPetAutoSpellSize(); ++i)
        {
            uint32 spellID = me->GetPetAutoSpellOnPos(i);
            if (!spellID)
                continue;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
            if (!spellInfo)
                continue;

            if (me->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
                continue;

            if (me->HasSpellCooldown(spellInfo->Id))
                continue;

            if (spellInfo->IsPositive())
            {
                if (spellInfo->CanBeUsedInCombat())
                {
                    if (!me->GetCharmInfo()->IsCommandAttack())
                        if (!(spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && spellInfo->SpellFamilyFlags[1] & 0x2000000) // Fel Intelligence
                            && !(spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && spellInfo->SpellFamilyFlags[0] & 0x800000)) // Blood Pact
                            continue;
                }

                Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, 0);
                bool spellUsed = false;

                // Some spells can target enemy or friendly (DK Ghoul's Leap)
                // Check for enemy first (pet then owner)
                Unit* target = me->getAttackerForHelper();
                if (!target && owner)
                    target = owner->getAttackerForHelper();

                if (target)
                {
                    if (CanAttack(target) && spell->CanAutoCast(target))
                    {
                        targetSpellStore.push_back(std::make_pair(target, spell));
                        spellUsed = true;
                    }
                }

                if (spellInfo->HasEffect(SPELL_EFFECT_JUMP_DEST))
                {
                    if (!spellUsed)
                    {
                        spell->finish(false);
                        delete spell;
                    }

                    continue;
                }

                // No enemy, check friendly
                if (!spellUsed)
                {
                    for (std::set<uint64>::const_iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
                    {
                        Unit* ally = ObjectAccessor::GetUnit(*me, *tar);

                        //only buff targets that are in combat, unless the spell can only be cast while out of combat
                        if (!ally)
                            continue;

                        if (spell->CanAutoCast(ally))
                        {
                            targetSpellStore.push_back(std::make_pair(ally, spell));
                            spellUsed = true;
                            break;
                        }
                    }
                }
                // No valid targets at all
                if (!spellUsed)
                {
                    spell->finish(false);
                    delete spell;
                }
            }
            else if (me->getVictim() && CanAttack(me->getVictim()) && spellInfo->CanBeUsedInCombat())
            {
                Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, 0);
                if (spell->CanAutoCast(me->getVictim()))
                    targetSpellStore.push_back(std::make_pair(me->getVictim(), spell));
                else
                {
                    spell->finish(false);
                    delete spell;
                }
            }
        }

        //found units to cast on to
        if (!targetSpellStore.empty())
        {
            uint32 index = urand(0, targetSpellStore.size() - 1);

            Spell* spell  = targetSpellStore[index].second;
            Unit*  target = targetSpellStore[index].first;

            targetSpellStore.erase(targetSpellStore.begin() + index);

            SpellCastTargets targets;
            targets.SetUnitTarget(target);

            if (!me->HasInArc(M_PI, target))
            {
                me->SetInFront(target);
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(target->ToPlayer());

                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(owner->ToPlayer());
            }

            me->AddCreatureSpellCooldown(spell->m_spellInfo->Id);

            spell->prepare(&targets);
        }

        // deleted cached Spell objects
        for (TargetSpellList::const_iterator itr = targetSpellStore.begin(); itr != targetSpellStore.end(); ++itr)
            delete itr->second;
    }
}

void PetAI::UpdateAllies()
{
    Unit* owner = me->GetCharmerOrOwner();
    Group* group = NULL;

    m_updateAlliesTimer = 10*IN_MILLISECONDS;                //update friendly targets every 10 seconds, lesser checks increase performance

    if (!owner)
        return;
    else if (owner->GetTypeId() == TYPEID_PLAYER)
        group = owner->ToPlayer()->GetGroup();

    //only pet and owner/not in group->ok
    if (m_AllySet.size() == 2 && !group)
        return;
    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if (group && !group->isRaidGroup() && m_AllySet.size() == (group->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(me->GetGUID());
    if (group)                                              //add group
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if (!Target || !group->SameSubGroup((Player*)owner, Target))
                continue;

            if (Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

void PetAI::KilledUnit(Unit* victim)
{
    // Called from Unit::Kill() in case where pet or owner kills something
    // if owner killed this victim, pet may still be attacking something else
    if (me->getVictim() && me->getVictim() != victim)
        return;

    // Clear target just in case. May help problem where health / focus / mana
    // regen gets stuck. Also resets attack command.
    // Can't use _stopAttack() because that activates movement handlers and ignores
    // next target selection
    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop();  // Stops the pet's 'Attack' button from flashing

    Unit* nextTarget = SelectNextTarget();

    if (nextTarget)
        AttackStart(nextTarget);
    else
    {
        me->GetCharmInfo()->SetIsCommandAttack(false);
        HandleReturnMovement(); // Return
    }
}

void PetAI::AttackStart(Unit* target)
{
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!CanAttack(target))
        return;

    DoAttack(target, true);
}

void PetAI::OwnerDamagedBy(Unit* attacker)
{
    // Called when owner takes damage. Allows defensive pets to know
    //  that their owner might need help

    if (!attacker)
        return;

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->getVictim() && me->getVictim()->isAlive())
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(attacker);
}

void PetAI::OwnerAttacked(Unit* target)
{
    // Called when owner attacks something. Allows defensive pets to know
    //  that they need to assist

    // Target might be NULL if called from spell with invalid cast targets
    if (!target)
        return;

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->getVictim() && me->getVictim()->isAlive())
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(target);
}

Unit* PetAI::SelectNextTarget()
{
    // Provides next target selection after current target death

    // Passive pets don't do next target selection
    if (me->HasReactState(REACT_PASSIVE))
        return NULL;

    Unit* target = me->getAttackerForHelper();

    // Check pet's attackers first to prevent dragging mobs back to owner
    if (target && !target->HasBreakableCrowdControlAura(me))
        return target;

    if (me->GetCharmerOrOwner())
    {
        // Check owner's attackers if pet didn't have any
        target = me->GetCharmerOrOwner()->getAttackerForHelper();
        if (target && !target->HasBreakableCrowdControlAura(me))
            return target;

        // 3.0.2 - Pets now start attacking their owners target in defensive mode as soon as the hunter does
        target = me->GetCharmerOrOwner()->getVictim();
        if (target && !target->HasBreakableCrowdControlAura(me))
            return target;
    }

    // Default
    return NULL;
}

void PetAI::DoAttack(Unit* target, bool chase)
{
    // Handles attack with or without chase and also resets all
    // PetAI flags for next update / creature kill

    // me->GetCharmInfo()->SetIsCommandAttack(false);

    // The following conditions are true if chase == true
    // (Follow && (Aggressive || Defensive))
    // ((Stay || Follow) && (Passive && player clicked attack))

    if (chase)
    {
        if (me->Attack(target, true))
        {
            me->GetCharmInfo()->SetIsAtStay(false);
            me->GetCharmInfo()->SetIsFollowing(false);
            me->GetCharmInfo()->SetIsReturning(false);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target, MELEE_RANGE, me->GetAngle(target) + frand(-(float)M_PI / 4.0f, (float)M_PI / 4.0f));
        }
    }
    else // (Stay && ((Aggressive || Defensive) && In Melee Range)))
    {
        me->GetCharmInfo()->SetIsAtStay(true);
        me->GetCharmInfo()->SetIsFollowing(false);
        me->GetCharmInfo()->SetIsReturning(false);
        me->Attack(target, true);
    }
}

void PetAI::MovementInform(uint32 moveType, uint32 data)
{
    // Receives notification when pet reaches stay or follow owner
    switch (moveType)
    {
        case POINT_MOTION_TYPE:
        {
            // Pet is returning to where stay was clicked. data should be
            // pet's GUIDLow since we set that as the waypoint ID
            if (data == me->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                me->GetCharmInfo()->SetIsAtStay(true);
                me->GetCharmInfo()->SetIsReturning(false);
                me->GetCharmInfo()->SetIsFollowing(false);
                me->GetCharmInfo()->SetIsCommandAttack(false);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
            }
            break;
        }
        case FOLLOW_MOTION_TYPE:
        {
            // If data is owner's GUIDLow then we've reached follow point,
            // otherwise we're probably chasing a creature
            if (me->GetCharmerOrOwner() && me->GetCharmInfo() && data == me->GetCharmerOrOwner()->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                me->GetCharmInfo()->SetIsAtStay(false);
                me->GetCharmInfo()->SetIsReturning(false);
                me->GetCharmInfo()->SetIsFollowing(true);
                me->GetCharmInfo()->SetIsCommandAttack(false);
            }
            break;
        }
        default:
            break;
    }
}

bool PetAI::CanAttack(Unit* target)
{
    // Evaluates wether a pet can attack a specific
    // target based on CommandState, ReactState and other flags

    if (!target)
        return false;

    if (!me->GetCharmInfo())
        return false;

    // Returning - check first since pets returning ignore attacks
    if (me->GetCharmInfo()->IsReturning())
        return false;

    // Pet cannot be CC'd
    if (me->IsCrowdControlled())
        return false;

    // Passive - check now so we don't have to worry about passive in later checks
    if (me->HasReactState(REACT_PASSIVE))
        return me->GetCharmInfo()->IsCommandAttack();

    //  Pets commanded to attack should not stop their approach if attacked by another creature
    if (me->getVictim() && (me->getVictim() != target))
        return !me->GetCharmInfo()->IsCommandAttack();

    // From this point on, pet will always be either aggressive or defensive

    // Stay - can attack if target is within range or commanded to
    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
        return (me->IsWithinObjectSizeDistance(target, MELEE_RANGE) || me->GetCharmInfo()->IsCommandAttack());

    // Follow
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        return true;

    // default, though we shouldn't ever get here
    return false;
}

void PetAI::SpellCastFailed(Unit* target, SpellCastResult result, Spell* spell)
{
    if (!target)
        return;

    if (result != SPELL_FAILED_OUT_OF_RANGE && result != SPELL_FAILED_LINE_OF_SIGHT)
        return;

    // Create a new spell since the old spell failed and will be deleted
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell->GetSpellInfo()->Id);
    Spell* newSpell = new Spell(me, spellInfo, TRIGGERED_NONE);

    me->setIsRunningToTarget(true);
    me->setQueuedSpell(newSpell);
    me->setQueuedSpellTargetGuid(target->GetGUID());
    me->GetMotionMaster()->MoveChase(target);
}