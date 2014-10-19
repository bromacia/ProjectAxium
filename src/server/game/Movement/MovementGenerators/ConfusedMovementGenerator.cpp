#include "Creature.h"
#include "MapManager.h"
#include "ConfusedMovementGenerator.h"
#include "VMapFactory.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"
#include "PathFinderMovementGenerator.h"

#ifdef MAP_BASED_RAND_GEN
#define rand_norm() unit.rand_norm()
#define urand(a, b) unit.urand(a, b)
#endif

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T &unit)
{
    if (Duration)
        HasDuration = true;

    init = true;
    unit.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.AddUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    if (Player* player = unit.ToPlayer())
    {
        player->SetClientControl(player, false);
        if (unit.isPossessed())
            if (Unit* charmer = unit.GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    charmer->ToPlayer()->SetClientControl(charmer, false);
    }
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T &unit)
{
    i_nextMove = 1;
    i_nextMoveTime.Reset(0);
    unit.StopMoving();
    unit.AddUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
}

template<class T>
bool ConfusedMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    if (HasDuration)
    {
        if (!TotalDuration.GetExpiry() || TotalDuration.GetExpiry() != Duration)
            TotalDuration = Duration;

        TotalDuration.Update(diff);
        if (TotalDuration.Passed())
            return false;
    }

    if (!&unit || !unit.isAlive())
        return false;

    if (unit.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED | UNIT_STATE_FLEEING | UNIT_STATE_ROAMING) || unit.IsFlying())
    {
        unit.ClearUnitState(UNIT_STATE_CONFUSED_MOVE);
        return true;
    }

    if (Player* player = unit.ToPlayer())
        if (player->IsBeingTeleported())
            return true;

    float speed = unit.GetSpeed(MOVE_RUN);

    if (init)
    {
        float x = unit.GetPositionX();
        float y = unit.GetPositionY();
        float z = unit.GetPositionZ();
        float const wanderDistance = 3.0f;
        i_nextMove = urand(1, MAX_CONF_WAYPOINTS);

        for (uint8 idx = 0; idx < MAX_CONF_WAYPOINTS + 1; ++idx)
        {
            const float wanderX = wanderDistance * (float)rand_norm() - wanderDistance / 2;
            const float wanderY = wanderDistance * (float)rand_norm() - wanderDistance / 2;

            i_waypoints[idx][0] = x + wanderX;
            i_waypoints[idx][1] = y + wanderY;

            // prevent invalid coordinates generation
            Axium::NormalizeMapCoord(i_waypoints[idx][0]);
            Axium::NormalizeMapCoord(i_waypoints[idx][1]);
            i_waypoints[idx][2] =  z;
        }

        unit.UpdateAllowedPositionZ(x, y, z);

        if (unit.GetMap()->IsInWater(x, y, z))
        {
            Movement::MoveSplineInit init(unit);
            init.SetVelocity(speed);
            init.MoveTo(x, y, z);
            init.Launch();
        }
        else
        {
            PathFinderMovementGenerator path(&unit);

            if (!unit.IsWithinLOS(x, y, z) || !path.Calculate(x, y, z) || path.GetPathType() & PATHFIND_NOPATH)
            {
                i_nextMoveTime.Reset(sWorld->getIntConfig(CONFIG_CONFUSED_MOVEMENT_GENERATOR_RESET_TIME));
                init = false;
                return true;
            }

            Movement::MoveSplineInit init(unit);
            init.SetVelocity(speed);
            init.MovebyPath(path.GetPath());
            init.Launch();
        }

        init = false;
        return true;
    }

    if (i_nextMoveTime.Passed())
    {
        // currently moving, update location
        unit.AddUnitState(UNIT_STATE_CONFUSED_MOVE);

        if (unit.movespline->Finalized())
        {
            i_nextMove = urand(1, MAX_CONF_WAYPOINTS);
            i_nextMoveTime.Reset(sWorld->getIntConfig(CONFIG_CONFUSED_MOVEMENT_GENERATOR_RESET_TIME));
        }
    }
    else
    {
        // waiting for next move
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
        {
            // start moving
            unit.AddUnitState(UNIT_STATE_CONFUSED_MOVE);

            ASSERT(i_nextMove <= MAX_CONF_WAYPOINTS);
            float x = i_waypoints[i_nextMove][0];
            float y = i_waypoints[i_nextMove][1];
            float z = i_waypoints[i_nextMove][2];

            unit.UpdateAllowedPositionZ(x, y, z);

            if (unit.GetMap()->IsInWater(x, y, z))
            {
                Movement::MoveSplineInit init(unit);
                init.SetVelocity(speed);
                init.MoveTo(x, y, z);
                init.Launch();
            }
            else
            {
                PathFinderMovementGenerator path(&unit);

                if (!unit.IsWithinLOS(x, y, z) || !path.Calculate(x, y, z) || path.GetPathType() & PATHFIND_NOPATH)
                {
                    i_nextMoveTime.Reset(sWorld->getIntConfig(CONFIG_CONFUSED_MOVEMENT_GENERATOR_RESET_TIME));
                    return true;
                }

                Movement::MoveSplineInit init(unit);
                init.SetVelocity(speed);
                init.MovebyPath(path.GetPath());
                init.Launch();
            }
        }
    }

    return true;
}

template<>
void ConfusedMovementGenerator<Player>::Finalize(Player &unit)
{
    PathFinderMovementGenerator path(&unit);
    path.Clear();
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    unit.StopMoving();
    unit.InterruptSpline();

    if (unit.isPossessed())
    {
        if (Unit* charmer = unit.GetCharmer())
            if (charmer->GetTypeId() == TYPEID_PLAYER)
                charmer->ToPlayer()->SetClientControl(&unit, true);
    }
    else
        unit.SetClientControl(&unit, true);
}

template<>
void ConfusedMovementGenerator<Creature>::Finalize(Creature &unit)
{
    PathFinderMovementGenerator path(&unit);
    path.Clear();
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    unit.StopMoving();
    unit.InterruptSpline();

    if (unit.getVictim())
        unit.SetTarget(unit.getVictim()->GetGUID());
}

template void ConfusedMovementGenerator<Player>::Initialize(Player &player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Reset(Player &player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature &creature);
template bool ConfusedMovementGenerator<Player>::Update(Player &player, const uint32 &diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff);
