#ifndef AXIUM_RANDOMMOTIONGENERATOR_H
#define AXIUM_RANDOMMOTIONGENERATOR_H

#include "MovementGenerator.h"

template<class T>
class RandomMovementGenerator : public MovementGeneratorMedium< T, RandomMovementGenerator<T> >
{
    public:
        RandomMovementGenerator(float spawn_dist = 0.0f) : i_nextMoveTime(0), wander_distance(spawn_dist) { }
        void _setRandomLocation(T &);
        void Initialize(T &);
        void Finalize(T &);
        void Reset(T &);
        bool Update(T &, const uint32);
        bool GetResetPosition(T&, float& x, float& y, float& z);
        MovementGeneratorType GetMovementGeneratorType() { return RANDOM_MOTION_TYPE; }
    private:
        TimeTrackerSmall i_nextMoveTime;
        uint32 i_nextMove;
        float wander_distance;
};
#endif

