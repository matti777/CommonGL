#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <stdio.h>
#include <string>
#include <math.h>

#include "OpenGLAPI.h"
#include "Rect.h"

// Workaround for Necessitas bug(?) that incorrectly announces DEBUG
// even for release builds
#ifdef QT_NO_DEBUG
  #pragma message ("Disabling DEBUG because of QT_NO_DEBUG")
  #undef DEBUG
#endif

//
// This file provides a global function API for common utilities. Implementation
// for some of them require platform specific code.
//

// Prototypes
extern GLuint g_rectangleIndexBuffer;
extern GLuint g_rectangleCoordsVertexBuffer;

// Macros

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

// PORTABLE C/C++; STDIO/STL/OpenGL

/**
 * Initializes global Common data.
 *
 * @return true on success
 */
bool InitCommonData();

/** Deinitializes global common data. */
void DeinitCommonData();

/**
 * Sets the VertexAttribs struct pointers (glVertexAttribPointer())
 * for COORD_INDEX, TEXCOORD_INDEX, NORMAL_INDEX, assuming an active
 * vertex buffer.
 */
void SetVertexAttribsPointers();

/**
 * Sets the VertexAttribs struct pointers (glVertexAttribPointer())
 * for COORD_INDEX, TEXCOORD_INDEX, assuming an active
 * vertex buffer.
 */
void SetVertexAttribsTexCoordsPointers();

/**
 * Sets the VertexAttribsTangent struct pointers (glVertexAttribPointer())
 * for COORD_INDEX, TEXCOORD_INDEX, NORMAL_INDEX, TANGENT_INDEX,
 * assuming an active vertex buffer.
 */
void SetVertexAttribsTangentPointers();

/**
 * Calculates tangent space tangent vectors.
 *
 * @param input input array of vertices
 * @param output output array of vertices with tangent vectors populated
 * and rest of the data copied
 * @param numTriangles number of triangles in the data; meaning there must be
 * numTriangles*3 elements in both input / output arrays.
 */
void CalculateTangentVectors(const VertexAttribs* input,
                             VertexAttribsTangent* output,
                             int numTriangles);

/**
 * Renders a 2D image on screen. The shader program has to be set up prior
 * to the call and the uniforms fully set up. This method renders the whole
 * texture.
 *
 * @param rect
 * @param viewportWidth
 * @param viewportHeight
 */
void DrawImage2D(const CommonGL::Rect& rect,
                 int viewportWidth, int viewportHeight);

/**
 * Renders a 2D image on screen. The shader program has to be set up prior
 * to the call and the uniforms fully set up. This method can be used to
 * render only a part of the texture, by defining upper left (u1,v1)
 * and lower right (u2,v2) corner texture coordinates. (0.0, 1.0), (1.0, 0.0)
 * would be used to render the whole texture.
 *
 * @param rect
 * @param viewportWidth
 * @param viewportHeight
 */
void DrawImage2D(const CommonGL::Rect& rect,
                 int viewportWidth, int viewportHeight,
                 float u1, float v1, float u2, float v2);

/**
 * Renders a 2D quad with the current shader program.
 *
 * @param rect rect in which to draw
 */
void DrawQuad2D(const CommonGL::Rect& rect,
                int viewportWidth, int viewportHeight);

/**
 * Renders a 2D quad. Useful for drawing "faders" ie single color blankets
 * to create fade in/out effects.
 *
 * @param vertexBuffer
 * @param indexBuffer
 */
void DrawQuad2D(GLuint vertexBuffer, GLuint indexBuffer);

/**
 * Loads 6 named image files into a OpenGL Cube texture.
 *
 * @param texture this will hold a valid texture id on success
 * @return true on success
 */
bool LoadCubeTextureFromBundle(const char* xnegImageName,
                               const char* xposImageName,
                               const char* ynegImageName,
                               const char* yposImageName,
                               const char* znegImageName,
                               const char* zposImageName,
                               GLuint* texture);


/**
 * Loads a shader from bundled resource files.
 *
 * @param fileName should indicate shader file name without extension;
 * two files are read, <fileName>.vsh (vertex shader) and <fileName>.fsh
 * (fragment shader).
 * @param program used to store the program id if successful
 * @return true if successful
 */
bool LoadShaderFromBundle(const char* fileName, GLuint* program);

/**
 * Creates a 2D OpenGL texture.
 *
 * @param width texture width (in pixels)
 * @param height texture height (in pixels)
 * @param data texture data pointer. If NULL, glTexImage2D() is not called
 * and the texture object is merely created with no data.
 * @param texture this will hold a valid texture id on success
 * @param clamp if true, GL_CLAMP_TO_EDGE is set for both s, t
 * @return true on success
 */
bool Create2DTexture(int width, int height, void* data,
                     GLuint* texture, bool clamp, bool useMipmaps);

/** Loads a shader program */
bool LoadShader(GLuint* shaderProgram,
                const char* vertexShaderSource,
                const char* fragmentShaderSource);

/** Detaches shaders from a program, deletes them and the program. */
void UnloadShader(GLuint shaderProgram);

/** Checks for depth buffer extensions. */
bool DepthBufferExtensionPresent();

/** Checks for packed depth + stencil buffer extension. */
bool PackedDepthStencilExtensionPresent();

/** Returns the total amount of physical RAM on the host (in kilobytes). */
size_t GetTotalRam();

/** Creates a depth buffer FBO, usable for Shadow Mapping and such. */
bool CreateDepthTextureAndFBO(GLuint* fboId, GLuint* depthTextureId,
                              GLuint* renderBuffer, int width, int height,
                              bool supportRgbShadowTexture);

/** Prints a 4x4 matrix into log. */
void LogMatrix(const float* matrix);

// PLATFORM SPECIFIC

/**
 * Reads a bundled resource file into the specified buffer.
 * @param fileName file name with no path
 * @param zeropad whether to add extra zero ('\0') in the end (useful when
 * reading C strings)
 * @param size where the amount of bytes read will be stored
 * @param buffer will be allocated if successful; caller must call free() on it.
 * 'size' bytes of data will be copied into this buffer.
 */
bool ReadBundleFile(const char* fileName, bool zeropad, 
                    size_t* size, void** buffer);

/**
 * Loads a named image file into a OpenGL texture.
 *
 * @param imageName image (file) name to load
 * @param texture this will hold a valid texture id on success
 * @param clamp if true, GL_CLAMP_TO_EDGE is set for both s, t
 * @return true on success
 */
bool Load2DTextureFromBundle(const char* imageName, GLuint* texture,
                           bool clamp, bool useMipmaps);

/**
 * Loads a named file from bundle and upload it as one of the cubemap
 * target textures.
 *
 * @param target one of the 6 constants; eg. GL_TEXTURE_CUBE_MAP_POSITIVE_X
 */
bool LoadCubeMapTargetTexture(GLenum target, const char* imageName);

/**
 * Creates a random UUID string.
 */
std::string RandomUuid();

#define LOG_INFO(...) PrintLogDebug(__VA_ARGS__)
#ifdef DEBUG
  #define DEBUG_ASSERT(x) DebugAssert(x)
  #define LOG_DEBUG(...) PrintLogDebug(__VA_ARGS__)
#else
  #define LOG_DEBUG(...)
  #define DEBUG_ASSERT(x) (x)
#endif

void PrintLogDebug(const char* fmt, ...);
void DebugAssert(bool criteria);

#endif // COMMONFUNCTIONS_H
