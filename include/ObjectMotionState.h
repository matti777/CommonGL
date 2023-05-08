#ifndef OBJECTMOTIONSTATE_H
#define OBJECTMOTIONSTATE_H

#include <btBulletDynamicsCommon.h>
#include <stdint.h>

#include "PickingColor.h"

/**
 * This class binds together a Bullet Physics motion state and an arbitrary
 * renderable object and its transformation.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class ObjectMotionState : public btDefaultMotionState
{
public:
    /**
     * Constructs the motion state. Does NOT take ownership of the
     * optional parameter object.
     *
     * @param initialTransform
     * @param object
     */
    ObjectMotionState(const btTransform& initialTransform, void* object);
    virtual ~ObjectMotionState();

public:
    void UpdateObjectTransform();
    float* GetObjectTransform() { return m_objectTransform; }
    void* GetObject() { return m_object; }

private:
    // Cached object transform
    float m_objectTransform[16];

    // Arbitrary object, or any kind of user data
    void* m_object;
};

#endif // OBJECTMOTIONSTATE_H
