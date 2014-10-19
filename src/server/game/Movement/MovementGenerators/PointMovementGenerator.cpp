#include "PointMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "World.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"

//----- Point Movement Generator
template<class T>
void PointMovementGenerator<T>::Initialize(T &unit)
{
    init = true;
}

template<class T>
bool PointMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    if (!&unit || !unit.isAlive())
        return false;

    if (!init)
    {
        if (unit.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED | UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING))
        {
            unit.ClearUnitState(UNIT_STATE_ROAMING_MOVE);
            if (path)
                unit.ClearUnitState(UNIT_STATE_CHARGING);
            return true;
        }
    }

    if (Player* player = unit.ToPlayer())
        if (player->IsBeingTeleported())
            return true;

    if (init)
    {
        if (path)
        {
            Movement::MoveSplineInit pInit(unit);
            pInit.MovebyPath(path->GetPath());
            if (speed > 0.0f)
                pInit.SetVelocity(speed);
            pInit.Launch();
            unit.AddUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);
            init = false;
            return true;
        }

        if (unit.GetMap()->IsInWater(i_x, i_y, i_z))
        {
            Movement::MoveSplineInit init(unit);
            init.MoveTo(i_x, i_y, i_z);
            if (speed > 0.0f)
                init.SetVelocity(speed);
            init.Launch();
        }
        else
        {
            if (m_generatePath)
            {
                PathFinderMovementGenerator i_path(&unit);

                if (!unit.IsWithinLOS(i_x, i_y, i_z) || !i_path.Calculate(i_x, i_y, i_z) || i_path.GetPathType() & PATHFIND_NOPATH)
                {
                    init = false;
                    return true;
                }

                Movement::MoveSplineInit init(unit);
                init.MovebyPath(i_path.GetPath());
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }
            else
            {
                Movement::MoveSplineInit init(unit);
                init.MoveTo(i_x, i_y, i_z);
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }

        }

        unit.AddUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE);
        init = false;
        return true;
    }

    if (!path && i_recalculateSpeed && !unit.movespline->Finalized())
    {
        if (unit.GetMap()->IsInWater(i_x, i_y, i_z))
        {
            Movement::MoveSplineInit init(unit);
            init.MoveTo(i_x, i_y, i_z);
            if (speed > 0.0f)
                init.SetVelocity(speed);
            init.Launch();
        }
        else
        {
            if (m_generatePath)
            {
                PathFinderMovementGenerator i_path(&unit);

                if (!unit.IsWithinLOS(i_x, i_y, i_z) || !i_path.Calculate(i_x, i_y, i_z) || i_path.GetPathType() & PATHFIND_NOPATH)
                    return true;

                Movement::MoveSplineInit init(unit);
                init.MovebyPath(i_path.GetPath());
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }
            else
            {
                Movement::MoveSplineInit init(unit);
                init.MoveTo(i_x, i_y, i_z);
                if (speed > 0.0f)
                    init.SetVelocity(speed);
                init.Launch();
            }
        }
    }

    unit.AddUnitState(UNIT_STATE_ROAMING_MOVE);
    if (path)
        unit.AddUnitState(UNIT_STATE_CHARGING);

    return !unit.movespline->Finalized();
}

template<class T>
void PointMovementGenerator<T>::Finalize(T &unit)
{
    unit.ClearUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);
    unit.InterruptSpline();

    if (unit.movespline->Finalized())
        MovementInform(unit);
}

template<class T>
void PointMovementGenerator<T>::Reset(T &unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();

    unit.AddUnitState(UNIT_STATE_ROAMING|UNIT_STATE_ROAMING_MOVE);

    if (path)
        unit.AddUnitState(UNIT_STATE_CHARGING);
}

template<class T>
void PointMovementGenerator<T>::MovementInform(T & /*unit*/)
{
}

template <> void PointMovementGenerator<Creature>::MovementInform(Creature &unit)
{
    if (unit.AI())
        unit.AI()->MovementInform(POINT_MOTION_TYPE, id);
}

template void PointMovementGenerator<Player>::Initialize(Player&);
template void PointMovementGenerator<Creature>::Initialize(Creature&);
template void PointMovementGenerator<Player>::Finalize(Player&);
template void PointMovementGenerator<Creature>::Finalize(Creature&);
template void PointMovementGenerator<Player>::Reset(Player&);
template void PointMovementGenerator<Creature>::Reset(Creature&);
template bool PointMovementGenerator<Player>::Update(Player &, const uint32 &);
template bool PointMovementGenerator<Creature>::Update(Creature&, const uint32 &);

void AssistanceMovementGenerator::Finalize(Unit &unit)
{
    unit.ToCreature()->SetNoCallAssistance(false);
    unit.ToCreature()->CallAssistance();
    if (unit.isAlive())
        unit.GetMotionMaster()->MoveSeekAssistanceDistract(sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY));
}

bool EffectMovementGenerator::Update(Unit &unit, const uint32)
{
    return !unit.movespline->Finalized();
}

void EffectMovementGenerator::Finalize(Unit &unit)
{
    if (unit.GetTypeId() != TYPEID_UNIT)
        return;

    unit.InterruptSpline();

    if (((Creature&)unit).AI() && unit.movespline->Finalized())
        ((Creature&)unit).AI()->MovementInform(EFFECT_MOTION_TYPE, m_Id);
}
