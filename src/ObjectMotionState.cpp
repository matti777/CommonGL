#include "ObjectMotionState.h"
#include "MatrixOperations.h"

ObjectMotionState::ObjectMotionState(const btTransform& initialTransform,
                                     void* object)
    : btDefaultMotionState(initialTransform),
      m_object(object)
{
    MatrixSetIdentity(m_objectTransform);
}

ObjectMotionState::~ObjectMotionState()
{
    // not owned
    m_object = NULL;
}

void ObjectMotionState::UpdateObjectTransform()
{
    // Copies the object transform from the superclass
    btTransform bodyTransform;
    getWorldTransform(bodyTransform);
    bodyTransform.getOpenGLMatrix(m_objectTransform);
}

