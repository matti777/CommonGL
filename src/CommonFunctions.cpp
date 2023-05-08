#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CommonFunctions.h"
#include "MatrixOperations.h"
#include "Rect.h"

// Vertex buffer for DrawImage2D()/DrawQuad2D()
GLuint g_vertexBuffer;

// Holds indices for two triangles representing a rectangle
GLuint g_rectangleIndexBuffer;

// Holds coords-only vertex data for full screen rectangle, matching
// g_rectangleIndexBuffer. To be used directly without view/projection,
// ie. in vertex shader just do gl_Position = in_coord
GLuint g_rectangleCoordsVertexBuffer;

bool InitCommonData()
{
    const GLushort RectIndices[] = { 0,1,3,  3,1,2 };

    glGenBuffers(1, &g_rectangleIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_rectangleIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RectIndices),
                 RectIndices, GL_STATIC_DRAW);

    VertexAttribsCoordsOnly vertices[] = {
        { -1,  1, 0 }, // top left
        { -1, -1, 0 }, // bottom left
        { 1,  -1, 0 }, // bottom right
        { 1,   1, 0 }  // top right
    };

    glGenBuffers(1, &g_rectangleCoordsVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_rectangleCoordsVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_vertexBuffer);

    int error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        LOG_DEBUG("InitCommonData(): GL error: 0x%x", error);
    }
    return (error == GL_NO_ERROR);
}

void DeinitCommonData()
{
    glDeleteBuffers(1, &g_rectangleIndexBuffer);
    glDeleteBuffers(1, &g_rectangleCoordsVertexBuffer);
    glDeleteBuffers(1, &g_vertexBuffer);
}

void SetVertexAttribsTexCoordsPointers()
{
    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, x));
    glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, u));
}

void SetVertexAttribsPointers()
{
    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribs),
                          (const GLvoid*)offsetof(VertexAttribs, x));
    glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribs),
                          (const GLvoid*)offsetof(VertexAttribs, u));
    glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribs),
                          (const GLvoid*)offsetof(VertexAttribs, nx));
}

void SetVertexAttribsTangentPointers()
{
    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTangent),
                          (const GLvoid*)offsetof(VertexAttribsTangent, x));
    glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTangent),
                          (const GLvoid*)offsetof(VertexAttribsTangent, u));
    glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTangent),
                          (const GLvoid*)offsetof(VertexAttribsTangent, nx));
    glVertexAttribPointer(TANGENT_INDEX, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTangent),
                          (const GLvoid*)offsetof(VertexAttribsTangent, tx));
}

void AdjustTangentVector(VertexAttribsTangent* vertex,
                         const float* tangent, const float* bitangent)
{
    // Orthogonalize the tangent to normal using the Gram-Schmidt method;
    // T' = normalize(T - N * dot(N, T))
    float t[3];
    const float* normal = &(vertex->nx);
    float tdotn = DotProduct(normal, tangent);
    t[0] = tangent[0] - (normal[0] * tdotn);
    t[1] = tangent[1] - (normal[1] * tdotn);
    t[2] = tangent[2] - (normal[2] * tdotn);
    NormalizeVector(t);
    vertex->tx = t[0];
    vertex->ty = t[1];
    vertex->tz = t[2];

    // Find the bitangent via T x N and see if its facing the same way
    // as the one we calculated above; use this info for handedness
    float b[3];
    CrossProduct(normal, t, b);
    float bdotb = DotProduct(b, bitangent);
    float handedness = (bdotb < 0.0) ? -1.0 : 1.0;
    vertex->tw = handedness;

//    vertex->tx = tangent[0];
//    vertex->ty = tangent[1];
//    vertex->tz = tangent[2];
}

void CalculateTangentVectors(const VertexAttribs* input,
                             VertexAttribsTangent* output,
                             int numTriangles)
{
    // First pass; just copy the common data
    const VertexAttribs* inputVertex = input;
    VertexAttribsTangent* outputVertex = output;
    for ( int i = 0; i < numTriangles * 3; i++ )
    {
        outputVertex->x = inputVertex->x;
        outputVertex->y = inputVertex->y;
        outputVertex->z = inputVertex->z;
        outputVertex->u = inputVertex->u;
        outputVertex->v = inputVertex->v;
        outputVertex->nx = inputVertex->nx;
        outputVertex->ny = inputVertex->ny;
        outputVertex->nz = inputVertex->nz;

        inputVertex++;
        outputVertex++;
    }

    // Second pass; calculate the tangent vectors
    inputVertex = input;
    outputVertex = output;
    for ( int i = 0; i < numTriangles; i++ )
    {
        const VertexAttribs* in0 = inputVertex++;
        const VertexAttribs* in1 = inputVertex++;
        const VertexAttribs* in2 = inputVertex++;

        VertexAttribsTangent* out0 = outputVertex++;
        VertexAttribsTangent* out1 = outputVertex++;
        VertexAttribsTangent* out2 = outputVertex++;

        // Calculate vectors v1 = in1 - in0, v1 = in2 - in0
        float v1[3];
        float v2[3];
        v1[0] = in1->x - in0->x;
        v1[1] = in1->y - in0->y;
        v1[2] = in1->z - in0->z;
        v2[0] = in2->x - in0->x;
        v2[1] = in2->y - in0->y;
        v2[2] = in2->z - in0->z;

        // ..and corresponding texture coord vectors st1 and st2
        float st1[2];
        float st2[2];
        st1[0] = in1->u - in0->u;
        st1[1] = in1->v - in0->v;
        st2[0] = in2->u - in0->u;
        st2[1] = in2->v - in0->v;

        // Calculate the tangent vector
        float tangent[3];
        float coef = 1.0 / ((st1[0] * st2[1]) - (st2[0] * st1[1]));

        tangent[0] = coef * ((v1[0] * st2[1]) - (v2[0] * st1[1]));
        tangent[1] = coef * ((v1[1] * st2[1]) - (v2[1] * st1[1]));
        tangent[2] = coef * ((v1[2] * st2[1]) - (v2[2] * st1[1]));
        NormalizeVector(tangent);

        float bitangent[3];
        bitangent[0] = coef * ((v2[0] * st1[0]) - (v1[0] * st2[0]));
        bitangent[1] = coef * ((v2[1] * st1[0]) - (v1[1] * st2[0]));
        bitangent[2] = coef * ((v2[2] * st1[0]) - (v1[2] * st2[0]));
        NormalizeVector(bitangent);

        // Write the tangent vectors into each of the output vertices
        AdjustTangentVector(out0, tangent, bitangent);
        AdjustTangentVector(out1, tangent, bitangent);
        AdjustTangentVector(out2, tangent, bitangent);
    }
}

void DrawImage2D(const CommonGL::Rect& rect,
                 int viewportWidth, int viewportHeight,
                 float u1, float v1, float u2, float v2)
{
    int x = rect.m_left;
    int y = rect.m_top;
    int width = rect.GetWidth();
    int height = rect.GetHeight();

    // Adjust x/y according to viewport size so that 0,0 is upper left
    x -= viewportWidth / 2;
    y = -y + (viewportHeight / 2) - height;

    VertexAttribsTexCoords vertices[] = {
        { x, y + height, 0,         u1, v1 },
        { x, y, 0,                  u1, v2 },
        { x + width, y, 0,          u2, v2 },
        { x + width, y + height, 0, u2, v1 }
    };

    // set up GL for 2D over drawing the image
    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(NORMAL_INDEX);

    // Upload vertex data for the image rectangle
    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_rectangleIndexBuffer);

    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, x));
    glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, u));

    // draw the image as two triangles
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, NULL);

    glEnableVertexAttribArray(NORMAL_INDEX);
    glEnable(GL_DEPTH_TEST);
}

void DrawImage2D(const CommonGL::Rect& rect,
                 int viewportWidth, int viewportHeight)
{
    DrawImage2D(rect, viewportWidth, viewportHeight, 0.0, 1.0, 1.0, 0.0);
}

void DrawQuad2D(const CommonGL::Rect& rect,
                int viewportWidth, int viewportHeight)
{
    int x = rect.m_left;
    int y = rect.m_top;
    int width = rect.GetWidth();
    int height = rect.GetHeight();

    // Adjust x/y according to viewport size so that 0,0 is upper left
    x -= viewportWidth / 2;
    y = -y + (viewportHeight / 2) - height;

    VertexAttribsCoordsOnly vertices[] = {
        { x, y + height, 0         },
        { x, y, 0                  },
        { x + width, y, 0          },
        { x + width, y + height, 0 }
    };

    // set up GL for 2D over drawing the image
    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(NORMAL_INDEX);
    glDisableVertexAttribArray(TEXCOORD_INDEX);

    // Upload the quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_rectangleIndexBuffer);

    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsCoordsOnly),
                          (const GLvoid*)offsetof(VertexAttribsCoordsOnly, x));

    // draw the image as two triangles
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, NULL);

    glEnableVertexAttribArray(TEXCOORD_INDEX);
    glEnableVertexAttribArray(NORMAL_INDEX);
    glEnable(GL_DEPTH_TEST);
}

void DrawQuad2D(GLuint vertexBuffer, GLuint indexBuffer)
{
    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(NORMAL_INDEX);
    glDisableVertexAttribArray(TEXCOORD_INDEX);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsCoordsOnly),
                          (const GLvoid*)offsetof(VertexAttribsCoordsOnly, x));

    // Draw the fader rect as two triangles
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, NULL);

    glEnableVertexAttribArray(NORMAL_INDEX);
    glEnableVertexAttribArray(TEXCOORD_INDEX);
    glEnable(GL_DEPTH_TEST);
}

bool LoadShaderFromBundle(const char* fileName, GLuint* program)
{
    char buffer[256];
    size_t fileSize;
    char* vertexShader;
    char* fragmentShader;

    LOG_DEBUG("LoadShaderFromBundle(): loading '%s'..", fileName);

    // Load the vertex shader file
    snprintf(buffer, sizeof(buffer), "%s.vsh", fileName);
    if ( !ReadBundleFile(buffer, true, &fileSize, (void**)&vertexShader) )
    {
        LOG_DEBUG("Failed to read vertex shader source file!");
        return false;
    }

#ifdef __BUILD_DESKTOP__
    // On desktop builds, prepend GLSL version header
    char* buf = (char*)malloc(fileSize + 128);
    sprintf(buf, "#version 120\n\n%s\n", vertexShader);
    free(vertexShader);
    vertexShader = buf;
#endif

    // Load the fragment shader file
    snprintf(buffer, sizeof(buffer), "%s.fsh", fileName);
    if ( !ReadBundleFile(buffer, true, &fileSize, (void**)&fragmentShader) )
    {
        LOG_DEBUG("Failed to read fragment shader source file!");
        free(vertexShader);
        return false;
    }

#ifdef __BUILD_DESKTOP__
    // On desktop builds, prepend GLSL version header
    buf = (char*)malloc(fileSize + 128);
    sprintf(buf, "#version 120\n\n%s\n", fragmentShader);
    free(fragmentShader);
    fragmentShader = buf;
#endif

    // Compile & load the shader into OpenGL
    bool ret = LoadShader(program, vertexShader, fragmentShader);

    // Deallocate memory
    free(vertexShader);
    free(fragmentShader);

    return ret;
}

bool Create2DTexture(int width, int height, void* data,
                     GLuint* texture, bool clamp, bool useMipmaps)
{
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    if ( data != NULL )
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    if ( useMipmaps )
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if ( clamp )
    { 
        // Clamp to texture edges
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        // Repeat the texture in both directions
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    int glError = glGetError();
    if ( glError != GL_NO_ERROR )
    {
        LOG_DEBUG("Create2DTexture(): GL error: 0x%x", glError);
        return false;
    }

    return true;
}

bool LoadCubeTextureFromBundle(const char* xnegImage,
                               const char* xposImage,
                               const char* ynegImage,
                               const char* yposImage,
                               const char* znegImage,
                               const char* zposImage,
                               GLuint* texture)
{
    GLuint cubeTexture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, xnegImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }
    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, xposImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }
    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, ynegImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }
    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, yposImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }
    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, znegImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }
    if ( !LoadCubeMapTargetTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, zposImage) )
    {
        glDeleteTextures(1, &cubeTexture);
        return false;
    }

    *texture = cubeTexture;
    return true;
}

bool DepthBufferExtensionPresent()
{
    return ( strstr((const char*)glGetString(GL_EXTENSIONS),
                    DepthTextureExtension) != NULL );
}

bool PackedDepthStencilExtensionPresent()
{
    return ( strstr((const char*)glGetString(GL_EXTENSIONS),
                    PackedDepthStencilExtension) != NULL );
}

bool CreateDepthTextureAndFBO(GLuint* fboId, GLuint* depthTextureId,
                              GLuint* renderBuffer, int width, int height,
                              bool supportRgbShadowTexture)
{
    if ( !DepthBufferExtensionPresent() && !supportRgbShadowTexture )
    {
        LOG_DEBUG("Depth texture not supported");
        return false;
    }

    // Create a framebuffer object
    glGenFramebuffers(1, fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, *fboId);

    // Create a render buffer
    glGenRenderbuffers(1, renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *renderBuffer);

    // Create a texture for storing the depth
    glGenTextures(1, depthTextureId);
    glBindTexture(GL_TEXTURE_2D, *depthTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Remove artifact on the edges of the shadowmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if ( DepthBufferExtensionPresent() )
    {
        // We'll use a depth texture to store the depths in the shadow map
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

        // Attach the depth texture to FBO depth attachment point
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, *depthTextureId, 0);
    }
    else
    {
        // We'll use a RGBA texture into which we pack the depth info
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        // Attach the RGBA texture to FBO color attachment point
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, *depthTextureId, 0);

        // Allocate 16-bit depth buffer
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                              width, height);

        // Attach the render buffer as depth buffer - will be ignored
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, *renderBuffer);
    }

    // check FBO status
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG_DEBUG("FBO not complete: 0x%x", fboStatus);
        return false;
    }

    return true;
}

#ifdef DEBUG
void PrintProgramInfoLog(GLuint program)
{
    int loglen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
    char* msg = new char[loglen + 1];
    if ( msg != NULL )
    {
        glGetProgramInfoLog(program, loglen, NULL, msg);
        LOG_DEBUG("-- shader link messages:\n%s", msg);
        delete msg;
    }
}

void PrintShaderInfoLog(GLuint shader, const char* shaderType)
{
    int loglen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
    char* msg = new char[loglen + 1];
    if ( msg != NULL )
    {
        glGetShaderInfoLog(shader, loglen, NULL, msg);
        LOG_DEBUG("-- %s shader compilation messages:\n%s", shaderType, msg);
        delete msg;
    }
}
#endif

bool LoadShader(GLuint* shaderProgram,
                const char* vertexShaderSource,
                const char* fragmentShaderSource)
{
    GLint compileOk;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileOk);
#ifdef DEBUG
    PrintShaderInfoLog(vertexShader, "vertex");
#endif
    if ( !compileOk )
    {
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileOk);
#ifdef DEBUG
    PrintShaderInfoLog(fragmentShader, "fragment");
#endif
    if ( !compileOk )
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    
    // Bind vertex shader attributes. Note that it is ok to call
    // glBindAttribLocation() even if the program lacks that attribute -
    // this will emit a runtime warning on some platforms though.
    glBindAttribLocation(*shaderProgram, COORD_INDEX, CoordAttrName);
    glBindAttribLocation(*shaderProgram, COLOR_INDEX, ColorAttrName);
    glBindAttribLocation(*shaderProgram, TEXCOORD_INDEX, TexcoordAttrName);
    glBindAttribLocation(*shaderProgram, NORMAL_INDEX, NormalAttrName);
    glBindAttribLocation(*shaderProgram, TANGENT_INDEX, TangentAttrName);
    
    glLinkProgram(*shaderProgram);
    GLint linkOk;
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &linkOk);
#ifdef DEBUG
    PrintProgramInfoLog(*shaderProgram);
#endif

    return linkOk;
}

void UnloadShader(GLuint shaderProgram)
{
    GLuint shaders[2];
    GLsizei count;

    glGetAttachedShaders(shaderProgram, 2, &count, shaders);
    glDeleteProgram(shaderProgram); // detaches shaders
    glDeleteShader(shaders[0]);
    glDeleteShader(shaders[1]);
}

void LogMatrix(const float* matrix)
{
    for ( int i = 0; i < 4; i++ )
    {
        LOG_DEBUG("%f\t%f\t%f\t%f",
                  matrix[i*4 + 0], matrix[i*4 + 1],
                  matrix[i*4 + 2], matrix[i*4 + 3]);
    }
}


