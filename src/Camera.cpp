#include <math.h>

#include "Camera.h"
#include "MatrixOperations.h"

Camera::Camera()
{
    // set the camera matrix to identity
    MatrixSetIdentity(m_cameraMatrix);
}

void Camera::CalculateInverseCameraMatrix()
{
    CalculateInverseTransform(m_cameraMatrix, m_inverseCameraMatrix);
}

void Camera::ExtractRightVector(float* right)
{
    right[0] = -m_inverseCameraMatrix[0];
    right[1] = -m_inverseCameraMatrix[1];
    right[2] = -m_inverseCameraMatrix[2];
}

void Camera::ExtractUpVector(float* up)
{
    up[0] = -m_inverseCameraMatrix[4];
    up[1] = -m_inverseCameraMatrix[5];
    up[2] = -m_inverseCameraMatrix[6];
}

void Camera::ExtractForwardVector(float* forward)
{
    forward[0] = -m_inverseCameraMatrix[8];
    forward[1] = -m_inverseCameraMatrix[9];
    forward[2] = -m_inverseCameraMatrix[10];
}

