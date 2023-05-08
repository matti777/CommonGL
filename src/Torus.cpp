#include <math.h>

#include "Torus.h"
#include "CommonFunctions.h"

Torus::~Torus()
{
    // Release all OpenGL resources
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_indexBuffer);
}

Torus::Torus()
    : m_vertexBuffer(0),
      m_indexBuffer(0),
      m_numIndices(0)
{
}

Torus* Torus::Create(int numRotateDivides, int numCircleDivides,
                     float rotateRadius, float circleRadius)
{
    Torus* torus = new Torus();
    if ( !torus->Setup(numRotateDivides, numCircleDivides,
                       rotateRadius, circleRadius) )
    {
        LOG_DEBUG("Torus::Create(): Setup() failed");
        delete torus;
        return NULL;
    }
    else
    {
        return torus;
    }
}

bool Torus::Setup(int numRotateDivides, int numCircleDivides,
                  float rotateRadius, float circleRadius)
{
    int numCoords = numRotateDivides * numCircleDivides;
    m_numIndices = numCoords * 2 * 3;
    VertexAttribsCoordsOnly* coords = new VertexAttribsCoordsOnly[numCoords];
    GLushort* indices = new GLushort[m_numIndices];
    
    // Generate the geometry
    VertexAttribsCoordsOnly* coord = coords;
    for ( int i = 0; i < numRotateDivides; i++ )
    {
        float rotateAngle = i * (2 * M_PI) / numRotateDivides;
        
        for ( int j = 0; j < numCircleDivides; j++ )
        {
            // Calculate the x, y on the circle
            float circleAngle = j * (2 * M_PI) / numCircleDivides;
            float circleX = cosf(circleAngle) * circleRadius;
            float circleY = sinf(circleAngle) * circleRadius;
            
            // Calculate object space x, y by rotating the circle around Y axis
            coord->x = cosf(rotateAngle) * (circleX + rotateRadius);
            coord->y = circleY;
            coord->z = sinf(rotateAngle) * (circleX + rotateRadius);
            coord++;
        }
    }

    // Create the indices
    GLushort* index = indices;
    for ( int i = 0; i < numRotateDivides; i++ )
    {
        int current = i * numCircleDivides;
        int next = ((i + 1) % numRotateDivides) * numCircleDivides;
        
        for ( int j = 0; j < numCircleDivides; j++ )
        {
            int next_j = (j + 1) % numCircleDivides;
            
            // Create 2 triangles between the vertices in "current circle" and
            // the "next circle". Define the triangles in counterclockwise order
            *index++ = next + next_j;
            *index++ = next + j;
            *index++ = current + j;
            
            *index++ = current + j;
            *index++ = current + next_j;
            *index++ = next + next_j;
        }
    }
    
    // Create vertex/index buffers
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    // Upload geometry
    glBufferData(GL_ARRAY_BUFFER, numCoords * sizeof(VertexAttribsCoordsOnly),
                 coords, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(GLushort),
                 indices, GL_STATIC_DRAW);
    
    delete coords;
    delete indices;
    
    return (glGetError() == GL_NO_ERROR);
}

void Torus::Render(bool filled)
{
    // Only using coords; disable others
    glDisableVertexAttribArray(TEXCOORD_INDEX);
    glDisableVertexAttribArray(NORMAL_INDEX);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsCoordsOnly),
                          (const GLvoid*)offsetof(VertexAttribsCoordsOnly, x));
    
    GLenum mode = ( filled ) ? GL_TRIANGLES : GL_LINES;
    glDrawElements(mode, m_numIndices, GL_UNSIGNED_SHORT, NULL);
    
    // Re-enable arrays
    glEnableVertexAttribArray(TEXCOORD_INDEX);
    glEnableVertexAttribArray(NORMAL_INDEX);
}


