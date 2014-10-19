#ifndef AXIUM_CONFUSEDGENERATOR_H
#define AXIUM_CONFUSEDGENERATOR_H

#include "MovementGenerator.h"
#include "Timer.h"

#define MAX_CONF_WAYPOINTS 24

template<class T>
class ConfusedMovementGenerator : public MovementGeneratorMedium< T, ConfusedMovementGenerator<T> >
{
    public:
        explicit ConfusedMovementGenerator(uint32 duration = 0) :
        Duration(duration), TotalDuration(0), HasDuration(false), init(false), i_nextMoveTime(0) { }
        void Initialize(T &);
        void Finalize(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);
        MovementGeneratorType GetMovementGeneratorType() { return CONFUSED_MOTION_TYPE; }
    private:
        bool init;
        float i_waypoints[MAX_CONF_WAYPOINTS+1][3];
        uint32 i_nextMove;
        TimeTracker i_nextMoveTime;
        float x, y, z;
        uint32 Duration;
        TimeTracker TotalDuration;
        bool HasDuration;
};
#endif
