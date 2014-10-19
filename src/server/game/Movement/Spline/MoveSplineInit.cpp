#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "MovementPacketBuilder.h"
#include "Unit.h"
#include "Transport.h"
#include "Vehicle.h"

namespace Movement
{
    UnitMoveType SelectSpeedType(uint32 moveFlags)
    {
        if (moveFlags & MOVEMENTFLAG_FLYING)
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.flight >= speed_obj.flight_back*/ )
                return MOVE_FLIGHT_BACK;
            else
                return MOVE_FLIGHT;
        }
        else if (moveFlags & MOVEMENTFLAG_SWIMMING)
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.swim >= speed_obj.swim_back*/)
                return MOVE_SWIM_BACK;
            else
                return MOVE_SWIM;
        }
        else if (moveFlags & MOVEMENTFLAG_WALKING)
        {
            //if ( speed_obj.run > speed_obj.walk )
            return MOVE_WALK;
        }
        else if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.run >= speed_obj.run_back*/)
            return MOVE_RUN_BACK;

        return MOVE_RUN;
    }

    int32 MoveSplineInit::Launch()
    {
        MoveSpline& move_spline = *unit.movespline;

        Location real_position(unit.GetPositionX(), unit.GetPositionY(), unit.GetPositionZMinusOffset(), unit.GetOrientation());
        args.splineId = splineIdGen.NewId();
        // Elevators also use MOVEMENTFLAG_ONTRANSPORT but we do not keep track of their position changes
        if (unit.HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) && unit.GetTransGUID())
        {
            real_position.x = unit.GetTransOffsetX();
            real_position.y = unit.GetTransOffsetY();
            real_position.z = unit.GetTransOffsetZ();
            real_position.orientation = unit.GetTransOffsetO();
        }

        // there is a big chance that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        if (!move_spline.Finalized())
            real_position = move_spline.ComputePosition();

        if (args.path.empty())
        {
            // form final position only for rotate
            if (!args.flags.isFacing())
                return 0;

            MoveTo(real_position);
        }
        else
        {
            // check path equivalence
            if (!args.flags.isFacing() && args.path[0] == args.path[args.path.size() - 1])
                return 0;
        }

        // corrent first vertex
        args.path[0] = real_position;
        args.initialOrientation = real_position.orientation;

        uint32 moveFlags = unit.m_movementInfo.GetMovementFlags();
        if (args.flags.walkmode)
            moveFlags |= MOVEMENTFLAG_WALKING;
        else
            moveFlags &= ~MOVEMENTFLAG_WALKING;

        moveFlags |= (MOVEMENTFLAG_SPLINE_ENABLED|MOVEMENTFLAG_FORWARD);

        if (!args.HasVelocity)
            args.velocity = unit.GetSpeed(SelectSpeedType(moveFlags));

        if (!args.Validate())
            return 0;

        if (moveFlags & MOVEMENTFLAG_ROOT)
            moveFlags &= ~MOVEMENTFLAG_MASK_MOVING;

        unit.m_movementInfo.SetMovementFlags(moveFlags);
        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data.append(unit.GetPackGUID());
        if (unit.GetTransGUID())
        {
            data.SetOpcode(SMSG_MONSTER_MOVE_TRANSPORT);
            data.appendPackGUID(unit.GetTransGUID());
            data << int8(unit.GetTransSeat());
        }
        PacketBuilder::WriteMonsterMove(move_spline, data);
        unit.SendMessageToSet(&data,true);

        return move_spline.Duration();
    }

    void MoveSplineInit::Stop()
    {
        MoveSpline& move_spline = *unit.movespline;

        // No need to stop if we are not moving
        if (move_spline.Finalized())
            return;

        Location loc = move_spline.ComputePosition();
        args.flags = MoveSplineFlag::Done;
        unit.m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_FORWARD);
        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data.append(unit.GetPackGUID());
        if (unit.GetTransGUID())
        {
            data.SetOpcode(SMSG_MONSTER_MOVE_TRANSPORT);
            data.appendPackGUID(unit.GetTransGUID());
            data << int8(unit.GetTransSeat());
        }

        PacketBuilder::WriteStopMovement(loc, args.splineId, data);
        unit.SendMessageToSet(&data, true);
    }

    MoveSplineInit::MoveSplineInit(Unit& m) : unit(m)
    {
        // Elevators also use MOVEMENTFLAG_ONTRANSPORT but we do not keep track of their position changes
        args.TransformForTransport = unit.HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) && unit.GetTransGUID();
        // mix existing state into new
        args.flags.walkmode = unit.m_movementInfo.HasMovementFlag(MOVEMENTFLAG_WALKING);
        args.flags.flying = unit.m_movementInfo.HasMovementFlag((MovementFlags)(MOVEMENTFLAG_FLYING|MOVEMENTFLAG_LEVITATING));
    }

    void MoveSplineInit::SetFacing(const Unit* target)
    {
        args.flags.EnableFacingTarget();
        args.facing.target = target->GetGUID();
    }

    void MoveSplineInit::SetFacing(float angle)
    {
        if (args.TransformForTransport)
        {
            if (Unit* vehicle = unit.GetVehicleBase())
                angle -= vehicle->GetOrientation();
            else if (Transport* transport = unit.GetTransport())
                angle -= transport->GetOrientation();
        }

        args.facing.angle = G3D::wrap(angle, 0.f, (float)G3D::twoPi());
        args.flags.EnableFacingAngle();
    }

    void MoveSplineInit::MoveTo(Vector3 const& dest, bool generatePath, bool forceDestination)
    {
        if (generatePath)
        {
            PathFinderMovementGenerator path(&unit);
            bool result = path.Calculate(dest.x, dest.y, dest.z, forceDestination);
            if (result && !(path.GetPathType() & PATHFIND_NOPATH))
            {
                MovebyPath(path.GetPath());
                return;
            }
        }

        args.path_Idx_offset = 0;
        args.path.resize(2);
        TransportPathTransform transform(unit, args.TransformForTransport);
        args.path[1] = transform(dest);
    }

    Vector3 TransportPathTransform::operator()(Vector3 input)
    {
        if (_transformForTransport)
        {
            if (Unit* vehicle = _owner.GetVehicleBase())
            {
                input.x -= vehicle->GetPositionX();
                input.y -= vehicle->GetPositionY();
                input.z -= vehicle->GetPositionZMinusOffset();
            }
            else if (Transport* transport = _owner.GetTransport())
            {
                float unused = 0.0f;
                transport->CalculatePassengerOffset(input.x, input.y, input.z, unused);
            }
        }

        return input;
    }
}
