#include "ByteBuffer.h"
#include "TargetedMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "World.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"

template<class T, typename D>
void TargetedMovementGeneratorMedium<T,D>::_setTargetLocation(T &owner, bool updateDestination)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return;

    if (owner.HasUnitState(UNIT_STATE_NOT_MOVE))
        return;

    if (owner.GetTypeId() == TYPEID_UNIT && !i_target->isInAccessiblePlaceFor(owner.ToCreature()))
        return;

    float x = 0;
    float y = 0;
    float z = 0;

    if (updateDestination || !i_path)
    {
        float dist = 0;
        float size = 0;

        if (!i_offset)
        {
            dist = i_target->GetCombatReach();
            size = i_target->GetCombatReach() - i_target->GetObjectSize();
        }
        else
        {
            // Pets need special handling.
            // We need to subtract GetObjectSize() because it gets added back further down the chain
            //  and that makes pets too far away. Subtracting it allows pets to properly
            //  be (GetCombatReach() + i_offset) away.
            // Only applies when i_target is pet's owner otherwise pets and mobs end up
            //   doing a "dance" while fighting
            if (owner.isPet() && i_target->GetTypeId() == TYPEID_PLAYER)
            {
                dist = i_target->GetCombatReach();
                size = i_target->GetCombatReach() - i_target->GetObjectSize();
            }
            else
            {
                dist = i_offset + 1.0f;
                size = owner.GetObjectSize();
            }
        }

        if (i_target->IsWithinDistInMap(&owner, dist) && owner.IsWithinLOS(i_target->GetPositionX(), i_target->GetPositionY(), i_target->GetPositionZ()))
        {
            i_recalculateTravel = false;
            return;
        }

        if (!owner.IsWithinLOS(i_target->GetPositionX(), i_target->GetPositionY(), i_target->GetPositionZ()))
            i_target->GetPosition(x, y, z);
        else
        {
            i_target->GetClosePoint(x, y, z, size, i_offset, i_angle); // to at i_offset distance from target and i_angle from target facing
            float ground = i_target->GetMap()->GetWaterOrGroundLevel(x, y, z);
            if (fabs(z - ground) >= 3.0f)
                i_target->GetPosition(x, y, z);
        }
    }
    else
    {
        // the destination has not changed, we just need to refresh the path (usually speed change)
        G3D::Vector3 end = i_path->GetEndPosition();
        x = end.x;
        y = end.y;
        z = end.z;
    }

    // Custom temp fix for blades edge
    if (i_target->GetMapId() == 562)
    {
        float ground = i_target->GetMap()->GetWaterOrGroundLevel(x, y, z);
        if (fabs(z - ground) >= 8.0f)
            return;
    }

    owner.UpdateAllowedPositionZ(x, y, z, true);

    if (owner.GetMap()->IsInWater(x, y, z))
    {
        Movement::MoveSplineInit init(owner);
        init.MoveTo(x, y, z);
        init.Launch();
    }
    else
    {
        if (!i_path)
            i_path = new PathFinderMovementGenerator(&owner);

        bool lastPathOffMesh = i_path->UsingOffMesh();
        bool result = i_path->Calculate(x, y, z);
        if (!result || (i_path->GetPathType() & PATHFIND_NOPATH))
        {
            if (owner.HasUnitState(UNIT_STATE_FOLLOW))
            {
                Movement::MoveSplineInit init(owner);
                init.MoveTo(x, y, z, false, true);
                init.Launch();
            }
            else
                i_recalculateTravel = true;

            return;
        }

        bool newPathOffMesh = i_path->UsingOffMesh();
        if (lastPathOffMesh && newPathOffMesh)
        {
            G3D::Vector3 dest = owner.movespline->FinalDestination();
            if (i_target->IsWithinDist3d(dest.x, dest.y, dest.z, 4.0f))
            {
                i_recalculateTravel = true;
                return;
            }
        }

        Movement::MoveSplineInit init(owner);
        init.MovebyPath(i_path->GetPath());
        init.Launch();
    }

    D::_addUnitStateMove(owner);
    i_targetReached = false;
    i_recalculateTravel = false;
    owner.AddUnitState(UNIT_STATE_CHASE);
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T,D>::Update(T &owner, uint32 time_diff)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (!&owner || !owner.isAlive())
        return false;

    if (owner.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED | UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING | UNIT_STATE_NOT_MOVE))
    {
        D::_clearUnitStateMove(owner);
        return true;
    }

    if (owner.GetTypeId() == TYPEID_UNIT && !i_target->isInAccessiblePlaceFor((const Creature*)&owner))
        return false;

    // prevent movement while casting spells with cast time or channel time
    if (owner.HasUnitState(UNIT_STATE_CASTING))
    {
        if (!owner.IsStopped())
            owner.StopMoving();
        return true;
    }

    if (owner.HasUnitState(UNIT_STATE_POSSESSED))
        return false;

    // prevent crash after creature killed pet
    if (static_cast<D*>(this)->_lostTarget(owner))
    {
        D::_clearUnitStateMove(owner);
        return true;
    }

    bool targetMoved = false;
    i_recheckDistance.Update(time_diff);
    if (i_recheckDistance.Passed())
    {
        i_recheckDistance.Reset(100);
        //More distance let have better performance, less distance let have more sensitive reaction at target move.
        float allowed_dist = 0.0f;

        if (owner.HasUnitState(UNIT_STATE_FOLLOW))
            allowed_dist = i_target->GetCombatReach();

        if (!owner.GetCharmerOrOwner())
            if (owner.HasUnitState(UNIT_STATE_CHASE))
                allowed_dist = owner.GetCombatReach();

        G3D::Vector3 dest = owner.movespline->FinalDestination();

        if (owner.GetTypeId() == TYPEID_UNIT)
            targetMoved = !i_target->IsWithinDist3d(dest.x, dest.y, dest.z, allowed_dist);
    }

    if (i_recalculateTravel || targetMoved)
        _setTargetLocation(owner, true);

    if (owner.movespline->Finalized())
    {
        static_cast<D*>(this)->MovementInform(owner);
        if (i_angle == 0.0f && !owner.HasInArc(0.01f, i_target.getTarget()))
            owner.SetInFront(i_target.getTarget());

        if (!i_targetReached)
        {
            i_targetReached = true;
            static_cast<D*>(this)->_reachTarget(owner);
            owner.SetFacingTo(i_target->GetOrientation());
        }

        if (i_path)
            i_path->Clear();
    }

    return true;
}

//-----------------------------------------------//
template<class T>
void ChaseMovementGenerator<T>::_reachTarget(T &owner)
{
    if (owner.IsWithinObjectSizeDistance(this->i_target.getTarget(), MELEE_RANGE))
        owner.Attack(this->i_target.getTarget(), true);
}

template<>
void ChaseMovementGenerator<Player>::Initialize(Player &owner)
{
    owner.AddUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
    _setTargetLocation(owner, true);
}

template<>
void ChaseMovementGenerator<Creature>::Initialize(Creature &owner)
{
    owner.SetWalk(false);
    owner.AddUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
    _setTargetLocation(owner, true);
}

template<class T>
void ChaseMovementGenerator<T>::Finalize(T &owner)
{
    owner.ClearUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
}

template<class T>
void ChaseMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
void ChaseMovementGenerator<T>::MovementInform(T &/*unit*/)
{
}

template<>
void ChaseMovementGenerator<Creature>::MovementInform(Creature &unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    if (unit.AI())
        unit.AI()->MovementInform(CHASE_MOTION_TYPE, i_target.getTarget()->GetGUIDLow());
}

//-----------------------------------------------//
template<>
bool FollowMovementGenerator<Creature>::EnableWalking() const
{
    return i_target.isValid() && i_target->IsWalking();
}

template<>
bool FollowMovementGenerator<Player>::EnableWalking() const
{
    return false;
}

template<>
void FollowMovementGenerator<Player>::_updateSpeed(Player &/*owner*/)
{
}

template<>
void FollowMovementGenerator<Creature>::_updateSpeed(Creature &owner)
{
}

template<>
void FollowMovementGenerator<Player>::Initialize(Player &owner)
{
    owner.AddUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    _updateSpeed(owner);
    _setTargetLocation(owner, true);
}

template<>
void FollowMovementGenerator<Creature>::Initialize(Creature &owner)
{
    owner.AddUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    _updateSpeed(owner);
    _setTargetLocation(owner, true);
}

template<class T>
void FollowMovementGenerator<T>::Finalize(T &owner)
{
    owner.ClearUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    _updateSpeed(owner);
}

template<class T>
void FollowMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
void FollowMovementGenerator<T>::MovementInform(T &/*unit*/)
{
}

template<>
void FollowMovementGenerator<Creature>::MovementInform(Creature &unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    if (unit.AI())
        unit.AI()->MovementInform(FOLLOW_MOTION_TYPE, i_target.getTarget()->GetGUIDLow());
}

//-----------------------------------------------//
template void TargetedMovementGeneratorMedium<Player,ChaseMovementGenerator<Player> >::_setTargetLocation(Player &, bool);
template void TargetedMovementGeneratorMedium<Player,FollowMovementGenerator<Player> >::_setTargetLocation(Player &, bool);
template void TargetedMovementGeneratorMedium<Creature,ChaseMovementGenerator<Creature> >::_setTargetLocation(Creature &, bool);
template void TargetedMovementGeneratorMedium<Creature,FollowMovementGenerator<Creature> >::_setTargetLocation(Creature &, bool);
template bool TargetedMovementGeneratorMedium<Player,ChaseMovementGenerator<Player> >::Update(Player &, uint32);
template bool TargetedMovementGeneratorMedium<Player,FollowMovementGenerator<Player> >::Update(Player &, uint32);
template bool TargetedMovementGeneratorMedium<Creature,ChaseMovementGenerator<Creature> >::Update(Creature &, uint32);
template bool TargetedMovementGeneratorMedium<Creature,FollowMovementGenerator<Creature> >::Update(Creature &, uint32);

template void ChaseMovementGenerator<Player>::_reachTarget(Player &);
template void ChaseMovementGenerator<Creature>::_reachTarget(Creature &);
template void ChaseMovementGenerator<Player>::Finalize(Player &);
template void ChaseMovementGenerator<Creature>::Finalize(Creature &);
template void ChaseMovementGenerator<Player>::Reset(Player &);
template void ChaseMovementGenerator<Creature>::Reset(Creature &);
template void ChaseMovementGenerator<Player>::MovementInform(Player &);

template void FollowMovementGenerator<Player>::Finalize(Player &);
template void FollowMovementGenerator<Creature>::Finalize(Creature &);
template void FollowMovementGenerator<Player>::Reset(Player &);
template void FollowMovementGenerator<Creature>::Reset(Creature &);
template void FollowMovementGenerator<Player>::MovementInform(Player &);
