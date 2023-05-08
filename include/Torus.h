#ifndef TORUS_H
#define TORUS_H

#include "OpenGLAPI.h"

/**
 * Classic donut / torus model with configurable geometry. The torus is
 * created by rotating a circle in XY plane around the Y axis. The torus will
 * be centered around the origin.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class Torus
{
public: // Construction and destruction
    /**
     * Creates a new Torus.
     *
     * @param numRotateDivides number of sections around the Y axis
     * @param numCircleDivides number of sections around the XY circle
     * @param rotateRadius radius to move the circle away from the origin
     * @param circleRadius radius of the circle
     */
    static Torus* Create(int numRotateDivides, int numCircleDivides,
                         float rotateRadius, float circleRadius);
    virtual ~Torus();
  
public: // Public API
    void Render(bool filled);
    
private:
    Torus();
    bool Setup(int numRotateDivides, int numCircleDivides,
               float rotateRadius, float circleRadius);
    
private: // Data
    // vertex/index buffers
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    
    int m_numIndices;
};

#endif