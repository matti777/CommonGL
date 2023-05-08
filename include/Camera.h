#ifndef CAMERA_H
#define CAMERA_H

/**
 * Camera abstraction.
 */
class Camera
{
public:
    Camera();

public:
    /**
     * Creates a camera matrix based on camera position and "look-at" target.
     * Both vectors are world space coordinates.
     * @param position float[3]
     * @param target float[3]
     */
//    void LookAt(float* position, float* target);

    /** Returns a pointer to the camera matrix. */
    float* GetCameraMatrix() { return m_cameraMatrix; }

    /** Returns a pointer to the projection matrix. */
    float* GetProjectionMatrix() { return m_projectionMatrix; }

    /** Calculates the inverse camera matrix and stores it internally. */
    void CalculateInverseCameraMatrix();

    /** Returns a pointer to the inverse camera matrix. */
    float* GetInverseCameraMatrix() { return m_inverseCameraMatrix; }

    /**
     * Extracts the camera 'forward' vector.
     * @param forward float[3]
     */
    void ExtractForwardVector(float* forward);

    /**
     * Extracts the camera 'right' vector.
     * @param right float[3]
     */
    void ExtractRightVector(float* right);

    /**
     * Extracts the camera 'up' vector.
     * @param up float[3]
     */
    void ExtractUpVector(float* up);

private:
    float m_cameraMatrix[16];
    float m_inverseCameraMatrix[16];
    float m_projectionMatrix[16];
};

#endif // CAMERA_H
