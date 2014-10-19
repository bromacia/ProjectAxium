#ifndef AXIUM_POINTMOVEMENTGENERATOR_H
#define AXIUM_POINTMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "FollowerReference.h"
#include "PathFinderMovementGenerator.h"

template<class T>
class PointMovementGenerator : public MovementGeneratorMedium< T, PointMovementGenerator<T> >
{
    public:
        PointMovementGenerator(uint32 _id, float _x, float _y, float _z, bool _generatePath, float _speed = 0.0f, PathFinderMovementGenerator* _path = NULL) :
        init(false), id(_id), i_x(_x), i_y(_y), i_z(_z), speed(_speed), m_generatePath(_generatePath), i_recalculateSpeed(false), path(_path) { }
        void Initialize(T &);
        void Finalize(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);
        void MovementInform(T &);
        void unitSpeedChanged() { i_recalculateSpeed = true; }
        MovementGeneratorType GetMovementGeneratorType() { return POINT_MOTION_TYPE; }
        bool GetDestination(float& x, float& y, float& z) const { x = i_x; y = i_y; z = i_z; return true; }
    private:
        bool init;
        uint32 id;
        float i_x, i_y, i_z;
        float speed;
        bool m_generatePath;
        bool i_recalculateSpeed;
        PathFinderMovementGenerator* path;
};

class AssistanceMovementGenerator : public PointMovementGenerator<Creature>
{
    public:
        AssistanceMovementGenerator(float _x, float _y, float _z) : PointMovementGenerator<Creature>(0, _x, _y, _z, true) { }
        MovementGeneratorType GetMovementGeneratorType() { return ASSISTANCE_MOTION_TYPE; }
        void Finalize(Unit &);
};

// Does almost nothing - just doesn't allows previous movegen interrupt current effect.
class EffectMovementGenerator : public MovementGenerator
{
    public:
        explicit EffectMovementGenerator(uint32 Id) : m_Id(Id) { }
        void Initialize(Unit &) { }
        void Finalize(Unit &unit);
        void Reset(Unit &) { }
        bool Update(Unit &u, const uint32);
        MovementGeneratorType GetMovementGeneratorType() { return EFFECT_MOTION_TYPE; }
    private:
        uint32 m_Id;
};

#endif

