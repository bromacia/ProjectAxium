#ifndef AXIUM_HOMEMOVEMENTGENERATOR_H
#define AXIUM_HOMEMOVEMENTGENERATOR_H

#include "MovementGenerator.h"

class Creature;

template <class T>
class HomeMovementGenerator;

template <>
class HomeMovementGenerator<Creature> : public MovementGeneratorMedium< Creature, HomeMovementGenerator<Creature> >
{
    public:
        HomeMovementGenerator() : arrived(false) {}
        ~HomeMovementGenerator() {}
        void Initialize(Creature &);
        void Finalize(Creature &);
        void Reset(Creature &);
        bool Update(Creature &, const uint32);
        MovementGeneratorType GetMovementGeneratorType() { return HOME_MOTION_TYPE; }
    private:
        void _setTargetLocation(Creature &);
        bool arrived;
};
#endif

