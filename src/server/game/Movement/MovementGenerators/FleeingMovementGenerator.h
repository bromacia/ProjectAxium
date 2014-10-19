#ifndef AXIUM_FLEEINGMOVEMENTGENERATOR_H
#define AXIUM_FLEEINGMOVEMENTGENERATOR_H

#include "MovementGenerator.h"

template<class T>
class FleeingMovementGenerator : public MovementGeneratorMedium< T, FleeingMovementGenerator<T> >
{
    public:
        FleeingMovementGenerator(uint64 fright, uint32 duration = 0) :
        i_frightGUID(fright), Duration(duration), TotalDuration(0), HasDuration(false), init(false), i_nextCheckTime(0) { }
        void Initialize(T &);
        void Finalize(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);
        MovementGeneratorType GetMovementGeneratorType() { return FLEEING_MOTION_TYPE; }
    private:
        bool init;
        void _setTargetLocation(T &owner);
        bool _getPoint(T &owner, float &x, float &y, float &z);
        bool _setMoveData(T &owner);
        void _Init(T &);

        bool is_water_ok   :1;
        bool is_land_ok    :1;
        bool i_only_forward:1;

        float i_caster_x;
        float i_caster_y;
        float i_caster_z;
        float i_last_distance_from_caster;
        float i_to_distance_from_caster;
        float i_cur_angle;
        uint64 i_frightGUID;
        TimeTracker i_nextCheckTime;
        uint32 Duration;
        TimeTracker TotalDuration;
        bool HasDuration;
};

class TimedFleeingMovementGenerator : public FleeingMovementGenerator<Creature>
{
    public:
        TimedFleeingMovementGenerator(uint64 fright, uint32 time) : FleeingMovementGenerator<Creature>(fright, 0), i_totalFleeTime(time) { }
        MovementGeneratorType GetMovementGeneratorType() { return TIMED_FLEEING_MOTION_TYPE; }
        bool Update(Unit &, const uint32);
        void Finalize(Unit &);

    private:
        TimeTracker i_totalFleeTime;
};

#endif
