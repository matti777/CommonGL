#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <stdlib.h>

#include "OpenGLAPI.h"

/** Describes a renderable character. */
struct AlphabetCharInfo
{
    // Character that this entry represents
    wchar_t m_char;
    
    // Width of the character in pixels
    int m_width;
    
    // Height of the character in pixels
    int m_height;
    
    // Texture u,v coordinates to the font texture atlas
    float m_vtop;
    float m_vbottom;
    float m_uleft;
    float m_uright;
};

/**
 * Provides functionality to render text through OpenGL using a prebaked
 * font texture atlas. This class is to be subclassed by the font-specific
 * implementation.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class TextRenderer
{
public:
    virtual ~TextRenderer();

protected: // Construction
    TextRenderer(GLuint program, GLuint mvpLoc, GLuint textureLoc,
                 GLuint indexBuffer, int fontHeight);

public: // Public API
    /** 
     * Draws text at the specified location with the given scaling. 
     *
     * @param x x screen coordinate for text left edge
     * @param y y screen coordinate for text bottom edge
     */
    void DrawText(int x, int y, const char* text, float scale);

    /** 
     * Returns the width (in pixels) to required to render the given string
     * using the given scale.
     */
    int GetTextWidth(const char* text, float scale);
    
    /**
     * Prepares text rendering; essentially, sets the program and uploads
     * required uniforms. Must be called prior to any text rendering.
     * This function internally calls SetProgram() + SetTexture().
     */
    void Prepare();

    /**
     * Sets the shader program for rendering text / 2D. Also sets the
     * MVP matrix uniform to orthogonal projection.
     */
    void SetProgram();

    /**
     * Sets the text rendering texture.
     */
    void SetTexture();

    /**
     * Sets custom texture; can be used to set up any 2D drawing.
     */
    void SetTexture(GLuint texture);

    /**
     * Updates projection matrix etc.
     */
    virtual void ViewportResized(int width, int height);

    /** Sets up resources. Must be called by the subclass. */
    virtual bool Setup();

    /** Returns the unscaled font height. */
    int GetFontHeight() const { return m_fontHeight; }

private:
    const AlphabetCharInfo* FindCharInfo(wchar_t c) const;
    void EnsureCapacity(size_t capacityInChars);

protected: // Data
    // Screen dimensions
    int m_viewportWidth;
    int m_viewportHeight;

    // Projection matrix (typically orthographic projection)
    float m_projectionMatrix[16];

    // OpenGL resources
    GLuint m_indexBuffer;
    GLuint m_fontTextureAtlas;
    GLuint m_textProgram;
    GLuint m_textProgramTextureLoc;
    GLuint m_textProgramMvpLoc;

    // Alphabet data
    int m_numAlphabet;
    AlphabetCharInfo* m_alphabet;
    int m_fontHeight;
    
    // Character construct for rendering 
    //VertexAttribsTexCoords m_textVertices[4];
    GLuint m_vertexBuffer;
    VertexAttribsTexCoords* m_textVertices;
    size_t m_textVerticesCapacityInChars;
};

#endif // TEXTRENDERER_H
