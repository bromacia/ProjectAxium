#include "MotionMaster.h"
#include "CreatureAISelector.h"
#include "Creature.h"

#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include <cassert>

inline bool isStatic(MovementGenerator *mv)
{
    return (mv == &si_idleMovement);
}

void MotionMaster::Initialize()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DirectDelete(curr);
    }

    InitDefault();
}

// set new default movement generator
void MotionMaster::InitDefault()
{
    if (i_owner->GetTypeId() == TYPEID_UNIT)
    {
        MovementGenerator* movement = FactorySelector::selectMovementGenerator(i_owner->ToCreature());
        Mutate(movement == NULL ? &si_idleMovement : movement, MOTION_SLOT_IDLE);
    }
    else
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
}

MotionMaster::~MotionMaster()
{
    // clear ALL movement generators (including default)
    while (!empty())
    {
        MovementGenerator* curr = top();
        pop();
        if (curr)
            DirectDelete(curr);
    }
}

void MotionMaster::UpdateMotion(uint32 diff)
{
    if (!i_owner)
        return;

    ASSERT(!empty());

    m_cleanFlag |= MMCF_UPDATE;
    if (!top()->Update(*i_owner, diff))
    {
        m_cleanFlag &= ~MMCF_UPDATE;
        MovementExpired();
    }
    else
        m_cleanFlag &= ~MMCF_UPDATE;

    if (m_expList)
    {
        for (size_t i = 0; i < m_expList->size(); ++i)
        {
            MovementGenerator* mg = (*m_expList)[i];
            DirectDelete(mg);
        }

        delete m_expList;
        m_expList = NULL;

        if (empty())
            Initialize();
        else if (needInitTop())
            InitTop();
        else if (m_cleanFlag & MMCF_RESET)
            top()->Reset(*i_owner);

        m_cleanFlag &= ~MMCF_RESET;
    }
}

void MotionMaster::DirectClean(bool reset)
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DirectDelete(curr);
    }

    if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(*i_owner);
}

void MotionMaster::DelayedClean()
{
    while (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        if (curr)
            DelayedDelete(curr);
    }
}

void MotionMaster::DirectExpire(bool reset)
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        DirectDelete(curr);
    }

    while (!top())
        --i_top;

    if (empty())
        Initialize();
    else if (needInitTop())
        InitTop();
    else if (reset)
        top()->Reset(*i_owner);
}

void MotionMaster::DelayedExpire()
{
    if (size() > 1)
    {
        MovementGenerator *curr = top();
        pop();
        DelayedDelete(curr);
    }

    while (!top())
        --i_top;
}

void MotionMaster::MoveIdle()
{
    //! Should be preceded by MovementExpired or Clear if there's an overlying movementgenerator active
    if (empty() || !isStatic(top()))
        Mutate(&si_idleMovement, MOTION_SLOT_IDLE);
}

void MotionMaster::MoveRandom(float spawndist)
{
    if (i_owner->GetTypeId() == TYPEID_UNIT)
        Mutate(new RandomMovementGenerator<Creature>(spawndist), MOTION_SLOT_IDLE);
}

void MotionMaster::MoveTargetedHome()
{
    Clear(false);

    if (i_owner->GetTypeId()==TYPEID_UNIT && !((Creature*)i_owner)->GetCharmerOrOwnerGUID())
        Mutate(new HomeMovementGenerator<Creature>(), MOTION_SLOT_ACTIVE);
    else if (i_owner->GetTypeId()==TYPEID_UNIT && ((Creature*)i_owner)->GetCharmerOrOwnerGUID())
        if (Unit* target = ((Creature*)i_owner)->GetCharmerOrOwner())
            if (!i_owner->m_movedPlayer)
                Mutate(new FollowMovementGenerator<Creature>(*target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveConfused(uint32 duration)
{
    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new ConfusedMovementGenerator<Player>(duration), MOTION_SLOT_CONFUSED);
    else
        Mutate(new ConfusedMovementGenerator<Creature>(duration), MOTION_SLOT_CONFUSED);
}

void MotionMaster::MoveChase(Unit* target, float dist, float angle)
{
    if (!target || target == i_owner || i_owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        return;

    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new ChaseMovementGenerator<Player>(*target,dist,angle), MOTION_SLOT_ACTIVE);
    else
        Mutate(new ChaseMovementGenerator<Creature>(*target,dist,angle), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveFollow(Unit* target, float dist, float angle, MovementSlot slot)
{
    if (!target || target == i_owner || i_owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        return;

    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new FollowMovementGenerator<Player>(*target,dist,angle), slot);
    else
        Mutate(new FollowMovementGenerator<Creature>(*target,dist,angle), slot);
}

void MotionMaster::MovePoint(uint32 id, float x, float y, float z, bool generatePath)
{
    i_owner->UpdateAllowedPositionZ(x, y, z);

    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath), MOTION_SLOT_ACTIVE);
    else
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveLand(uint32 id, Position const& pos)
{
    float x, y, z;
    pos.GetPosition(x, y, z);
    i_owner->UpdateAllowedPositionZ(x, y, z);

    Movement::MoveSplineInit init(*i_owner);
    init.MoveTo(x, y, z);
    init.SetAnimation(Movement::ToGround);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveTakeoff(uint32 id, Position const& pos)
{
    float x, y, z;
    pos.GetPosition(x, y, z);
    i_owner->UpdateAllowedPositionZ(x, y, z);

    Movement::MoveSplineInit init(*i_owner);
    init.MoveTo(x, y, z);
    init.SetAnimation(Movement::ToFly);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveKnockbackFrom(float srcX, float srcY, float speedXY, float speedZ)
{
    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    float x, y, z;
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    float max_height = -Movement::computeFallElevation(moveTimeHalf,false,-speedZ);

    for (uint8 i = 0; i < 5; ++i)
    {
        switch (i)
        {
            case 0: i_owner->GetNearPoint(i_owner, x, y, z, i_owner->GetObjectSize(), dist, i_owner->GetAngle(srcX, srcY) + M_PI);        break;
            case 1: i_owner->GetNearPoint(i_owner, x, y, z, i_owner->GetObjectSize(), dist * 0.8, i_owner->GetAngle(srcX, srcY) + M_PI);  break;
            case 2: i_owner->GetNearPoint(i_owner, x, y, z, i_owner->GetObjectSize(), dist * 0.5, i_owner->GetAngle(srcX, srcY) + M_PI);  break;
            case 3: i_owner->GetNearPoint(i_owner, x, y, z, i_owner->GetObjectSize(), dist * 0.25, i_owner->GetAngle(srcX, srcY) + M_PI); break;
            case 4: i_owner->GetNearPoint(i_owner, x, y, z, i_owner->GetObjectSize(), dist * 0.1, i_owner->GetAngle(srcX, srcY) + M_PI);  break;
        }

        i_owner->UpdateAllowedPositionZ(x, y, z);

        if (i_owner->IsWithinLOS(x, y, z))
        {
            Movement::MoveSplineInit init(*i_owner);
            init.MoveTo(x, y, z);
            init.SetParabolic(max_height, 0);
            init.SetOrientationFixed(true);
            init.SetVelocity(speedXY);
            init.Launch();
            Mutate(new EffectMovementGenerator(0), MOTION_SLOT_CONTROLLED);
            break;
        }
    }
}

void MotionMaster::MoveJumpTo(float angle, float speedXY, float speedZ)
{
    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    float x, y, z;
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;

    i_owner->GetClosePoint(x, y, z, i_owner->GetObjectSize(), dist, angle);
    i_owner->UpdateAllowedPositionZ(x, y, z);

    MoveJump(x, y, z, speedXY, speedZ);
}

void MotionMaster::MoveJump(float x, float y, float z, float speedXY, float speedZ, uint32 id)
{
    i_owner->UpdateAllowedPositionZ(x, y, z);

    float moveTimeHalf = speedZ / Movement::gravity;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    Movement::MoveSplineInit init(*i_owner);
    init.MoveTo(x, y, z);
    init.SetParabolic(max_height, 0);
    init.SetVelocity(speedXY);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveFall(uint32 id)
{
    // Use larger distance for vmap height search than in most other cases
    float groundz = i_owner->GetPositionZ();
    float tz = i_owner->GetMap()->GetWaterOrGroundLevel(i_owner->GetPositionX(), i_owner->GetPositionY(), i_owner->GetPositionZ(), &groundz, true);
    i_owner->UpdateAllowedPositionZ(i_owner->GetPositionX(), i_owner->GetPositionY(), tz);

    // Abort if the ground is very near
    if (fabs(i_owner->GetPositionZ() - tz) < 0.1f)
        return;

    Movement::MoveSplineInit init(*i_owner);
    init.MoveTo(i_owner->GetPositionX(), i_owner->GetPositionY(), tz);
    init.SetFall();
    init.Launch();
    Mutate(new EffectMovementGenerator(id), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveCharge(float x, float y, float z, float speed, uint32 id, bool generatePath, PathFinderMovementGenerator* path)
{
    if (Impl[MOTION_SLOT_CONTROLLED] && Impl[MOTION_SLOT_CONTROLLED]->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE)
        return;

    i_owner->UpdateAllowedPositionZ(x, y, z);

    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new PointMovementGenerator<Player>(id, x, y, z, generatePath, speed, path), MOTION_SLOT_CONTROLLED);
    else
        Mutate(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, speed, path), MOTION_SLOT_CONTROLLED);
}

void MotionMaster::MoveSeekAssistance(float x, float y, float z)
{
    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    i_owner->AttackStop();
    i_owner->ToCreature()->SetReactState(REACT_PASSIVE);
    Mutate(new AssistanceMovementGenerator(x, y, z), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveSeekAssistanceDistract(uint32 time)
{
    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        return

    Mutate(new AssistanceDistractMovementGenerator(time), MOTION_SLOT_ACTIVE);
}

void MotionMaster::MoveFleeing(Unit* enemy, uint32 time, uint32 duration)
{
    if (!enemy)
        return;

    if (i_owner->HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    if (i_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new FleeingMovementGenerator<Player>(enemy->GetGUID(), duration), MOTION_SLOT_FLEEING);
    else
        if (time)
            Mutate(new TimedFleeingMovementGenerator(enemy->GetGUID(), time), MOTION_SLOT_FLEEING);
        else
            Mutate(new FleeingMovementGenerator<Creature>(enemy->GetGUID(), duration), MOTION_SLOT_FLEEING);
}

void MotionMaster::MoveTaxiFlight(uint32 path, uint32 pathnode)
{
    if (i_owner->GetTypeId() != TYPEID_PLAYER)
        return;

    if (path < sTaxiPathNodesByPath.size())
    {
        FlightPathMovementGenerator* mgen = new FlightPathMovementGenerator(sTaxiPathNodesByPath[path], pathnode);
        Mutate(mgen, MOTION_SLOT_CONTROLLED);
    }
}

void MotionMaster::MoveDistract(uint32 timer)
{
    if (Impl[MOTION_SLOT_ACTIVE])
        return;

    Mutate(new DistractMovementGenerator(timer), MOTION_SLOT_ACTIVE);
}

void MotionMaster::Mutate(MovementGenerator *m, MovementSlot slot)
{
    if ((i_top == MOTION_SLOT_CONFUSED || i_top == MOTION_SLOT_FLEEING) && (m->GetMovementGeneratorType() == CONFUSED_MOTION_TYPE || m->GetMovementGeneratorType() == FLEEING_MOTION_TYPE))
    {
        m->Initialize(*i_owner);
        return;
    }

    if (MovementGenerator *curr = Impl[slot])
    {
        Impl[slot] = NULL; // in case a new one is generated in this slot during directdelete
        if (i_top == slot && (m_cleanFlag & MMCF_UPDATE))
            DelayedDelete(curr);
        else
            DirectDelete(curr);
    }
    else if (i_top < slot)
        i_top = slot;

    Impl[slot] = m;
    if (i_top > slot)
        needInit[slot] = true;
    else
    {
        needInit[slot] = false;
        m->Initialize(*i_owner);
    }
}

void MotionMaster::MovePath(uint32 path_id, bool repeatable)
{
    if (!path_id)
        return;

    Mutate(new WaypointMovementGenerator<Creature>(path_id, repeatable), MOTION_SLOT_IDLE);
}

void MotionMaster::MoveRotate(uint32 time, RotateDirection direction)
{
    if (!time)
        return;

    Mutate(new RotateMovementGenerator(time, direction), MOTION_SLOT_ACTIVE);
}

void MotionMaster::propagateSpeedChange()
{
    for (int i = 0; i <= i_top; ++i)
    {
        if (Impl[i])
            Impl[i]->unitSpeedChanged();
    }
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
   if (empty())
       return IDLE_MOTION_TYPE;

   return top()->GetMovementGeneratorType();
}

MovementGeneratorType MotionMaster::GetMotionSlotType(int slot) const
{
    if (!Impl[slot])
        return NULL_MOTION_TYPE;
    else
        return Impl[slot]->GetMovementGeneratorType();
}

void MotionMaster::InitTop()
{
    top()->Initialize(*i_owner);
    needInit[i_top] = false;
}

void MotionMaster::DirectDelete(_Ty curr)
{
    if (isStatic(curr))
        return;
    curr->Finalize(*i_owner);
    delete curr;
}

void MotionMaster::DelayedDelete(_Ty curr)
{
    sLog->outCrash("Unit (Entry %u) is trying to delete its updating MG (Type %u)!", i_owner->GetEntry(), curr->GetMovementGeneratorType());
    if (isStatic(curr))
        return;
    if (!m_expList)
        m_expList = new ExpireList();
    m_expList->push_back(curr);
}

bool MotionMaster::GetDestination(float &x, float &y, float &z)
{
    if (i_owner->movespline->Finalized())
       return false;

    const G3D::Vector3& dest = i_owner->movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;
    return true;
}
