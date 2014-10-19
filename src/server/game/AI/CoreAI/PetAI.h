#ifndef AXIUM_PETAI_H
#define AXIUM_PETAI_H

#include "CreatureAI.h"
#include "Timer.h"

class Creature;
class Spell;

class PetAI : public CreatureAI
{
    public:

        explicit PetAI(Creature* c);

        void EnterEvadeMode();

        void UpdateAI(const uint32);
        static int Permissible(const Creature*);

        void KilledUnit(Unit* /*victim*/);
        void AttackStart(Unit* target);
        void MovementInform(uint32 moveType, uint32 data);
        void OwnerDamagedBy(Unit* attacker);
        void OwnerAttacked(Unit* target);
        void SpellCastFailed(Unit* target, SpellCastResult result, Spell* spell);

    private:
        bool _isVisible(Unit*) const;
        bool _needToStop(void);
        void _stopAttack(void);

        void UpdateAllies();

        TimeTracker i_tracker;
        bool inCombat;
        std::set<uint64> m_AllySet;
        uint32 m_updateAlliesTimer;

        Unit* SelectNextTarget();
        void DoAttack(Unit* target, bool chase);
        bool CanAttack(Unit* target);
};
#endif

