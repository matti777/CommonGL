#ifndef OPENGLAPI_H
#define OPENGLAPI_H

#if defined(QT_OPENGL_LIB)
  #if defined(__USE_QTOPENGL__)
    #include <QtOpenGL> // Use Qt OpenGL - for Qt Quick/QQuickItem
  #else
    #if defined(__BUILD_DESKTOP__)
      #include <GL/glew.h> // Use GLEW
    #else
      #include <GLES/gl.h>
      #include <GLES/glext.h>
    #endif
    #include <QGLWidget>
  #endif
#elif defined(__BUILD_IOS__)
  #import <OpenGLES/ES2/gl.h>
  #import <OpenGLES/ES2/glext.h>
#elif defined(__TIZEN__)
  #include <egl.h>
  #include <eglext.h>
  #include <gl2.h>
  #include <gl2ext.h>
#else
  // others will use gl.h
  #include <GL/gl.h>
//  #include <EGL/egl.h>
#endif

// Misc OpenGL ES related constants

#ifndef GL_DEPTH24_STENCIL8
  #ifdef GL_DEPTH24_STENCIL8_OES
    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
  #else
    #error "Could not find GL_DEPTH24_STENCIL8!"
  #endif
#endif

#define _SFY(x) #x
#define STRINGIFY(x) _SFY(x)

#ifdef DEBUG
#define LOG_GL_ERROR(x) { \
  int glerror = glGetError(); \
  if ( glerror != GL_NO_ERROR ) \
    LOG_DEBUG(STRINGIFY(__FILE__) ":" STRINGIFY(__LINE__) \
    " GL error: 0x%x", glerror); }

#else
#define LOG_GL_ERROR(x)
#endif

// Depth texture extension identifier
static const char* const DepthTextureExtension = "OES_depth_texture";

// Packed depth / stencil buffer extension
static const char* const PackedDepthStencilExtension = "OES_packed_depth_stencil";

/** Indices to bind different OpenGL vertex attributes to. */
enum AttribIndex
{
    COORD_INDEX = 0,
    COLOR_INDEX = 1,
    TEXCOORD_INDEX = 2,
    NORMAL_INDEX = 3,
    TANGENT_INDEX = 4
};

// GLSL fixed attribute names that match to AttribIndex values
const char* const CoordAttrName = "in_coord";
const char* const ColorAttrName = "in_color";
const char* const TexcoordAttrName = "in_texCoord";
const char* const NormalAttrName = "in_normal";
const char* const TangentAttrName = "in_tangent";

// Id of the default framebuffer (screen/backbuffer).
// NOTE this is not usable on iOS where you have to supply the default
// frame buffer yourself.
const GLuint DefaultFramebufferId = 0;

// Invalid uniform value
const GLint InvalidUniformLocation = -1;

// Common data types

/** Describes a 3-dimensional vector. */
struct Vector3
{
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    float x, y, z;
};

/** Describes a homogeneous 3-dimensional vector. */
struct Vector4
{
    float x, y, z, w;
};

/** Descriptor for vertex attributes: coordinates, texture, normal */
struct VertexAttribs
{
    GLfloat x, y, z;
    GLfloat u, v;
    GLfloat nx, ny, nz;
};

/** Descriptor for vertex attributes with tangent vectors. */
struct VertexAttribsTangent
{
    GLfloat x, y, z;
    GLfloat u, v;
    GLfloat nx, ny, nz;
    // Tangent space vector and its handedness (tw) which is 1.0 or -1.0;
    // when bitangent is being calculated, it must be multiplied by this:
    // B = cross(N, T.xyz) * T.w
    GLfloat tx, ty, tz, tw;
};

/** Descriptor for vertex attributes: coordinates only */
struct VertexAttribsCoordsOnly
{
    GLfloat x, y, z;
};

/** Descriptor for vertex attributes: coordinates+texcoords only */
struct VertexAttribsTexCoords
{
    GLfloat x, y, z;
    GLfloat u, v;
};

#endif // OPENGLAPI_H
