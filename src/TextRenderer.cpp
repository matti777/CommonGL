#include <string.h>

#include "TextRenderer.h"
#include "GLController.h"
#include "CommonFunctions.h"
#include "MatrixOperations.h"

// Number of vertices (for glDrawArays()) per character
static const int VerticesPerChar = 6;

TextRenderer::TextRenderer(GLuint program, GLuint mvpLoc, GLuint textureLoc,
                           GLuint indexBuffer, int fontHeight)
    : m_indexBuffer(indexBuffer),
      m_fontTextureAtlas(0),
      m_textProgram(program),
      m_textProgramTextureLoc(textureLoc),
      m_textProgramMvpLoc(mvpLoc),
      m_numAlphabet(0),
      m_alphabet(NULL),
      m_fontHeight(fontHeight),
      m_vertexBuffer(0),
      m_textVertices(NULL),
      m_textVerticesCapacityInChars(0)
{
    memset(&m_textVertices, 0, sizeof(m_textVertices));
}

TextRenderer::~TextRenderer()
{
    glDeleteTextures(1, &m_fontTextureAtlas);
    glDeleteBuffers(1, &m_vertexBuffer);

    delete m_textVertices;
    delete m_alphabet;
}

bool TextRenderer::Setup()
{
    EnsureCapacity(128);
    glGenBuffers(1, &m_vertexBuffer);

    return true;
}

void TextRenderer::Prepare()
{
    SetProgram();
    SetTexture();
}

void TextRenderer::SetProgram()
{    
    glUseProgram(m_textProgram);
    glUniformMatrix4fv(m_textProgramMvpLoc, 1, GL_FALSE, m_projectionMatrix);
}

void TextRenderer::SetTexture()
{
    SetTexture(m_fontTextureAtlas);
}

void TextRenderer::SetTexture(GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(m_textProgramTextureLoc, 0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void TextRenderer::ViewportResized(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    MatrixOrthographicProjection(m_projectionMatrix, 
                                 -m_viewportWidth/2, m_viewportWidth/2,
                                 -m_viewportHeight/2, m_viewportHeight/2,
                                 -m_viewportWidth/2, m_viewportWidth/2);
}

void TextRenderer::EnsureCapacity(size_t capacityInChars)
{
    if ( m_textVerticesCapacityInChars < capacityInChars )
    {
        delete m_textVertices;
        m_textVertices = NULL;
        size_t capacity = capacityInChars * VerticesPerChar;
        m_textVertices = new (std::nothrow) VertexAttribsTexCoords[capacity];
        memset(m_textVertices, 0, sizeof(VertexAttribsTexCoords) * capacity);
        m_textVerticesCapacityInChars = capacityInChars;
    }
}

inline const AlphabetCharInfo* TextRenderer::FindCharInfo(wchar_t c)  const
{
    for ( int i = 0; i < m_numAlphabet; i++ )
    {
        AlphabetCharInfo* info = (m_alphabet + i);
        if ( c == info->m_char )
        {
            return info;
        }
    }

    return NULL;
}

int TextRenderer::GetTextWidth(const char* text, float scale)
{
    int textWidth = 0;
    const size_t textLength = strlen(text);
    
    for ( unsigned int i = 0; i < textLength; i++ )
    {
//        LOG_DEBUG("char = %c", (wchar_t)text[i]);
        const AlphabetCharInfo* charInfo = FindCharInfo((wchar_t)text[i]);
        if ( charInfo == NULL )
        {
            // the requested character is not supported and will be skipped
            LOG_DEBUG("Undefined character: '%c'", (wchar_t)text[i]);
            continue;
        }
        else 
        {
            textWidth += charInfo->m_width;
        }
    }
    
    return roundf(textWidth * scale);
}

void TextRenderer::DrawText(int x, int y, const char* text, float scale)
{
    LOG_GL_ERROR();

    // adjust x/y according to viewport size so that 0,0 is upper left
    x -= (m_viewportWidth / 2);
    y = -y + (m_viewportHeight / 2);

    int textLength = strlen(text);
    EnsureCapacity(textLength);
    VertexAttribsTexCoords* attrib = m_textVertices;
    size_t charCount = 0;

    // Create vertex data for every valid char in the string
    for ( int i = 0; i < textLength; i++ )
    {
        const AlphabetCharInfo* charInfo = FindCharInfo(text[i]);
        if ( charInfo == NULL )
        {
            // the requested character is not supported and will be skipped
            LOG_DEBUG("Undefined character: '%c'", (wchar_t)text[i]);
            continue;
        }

        int width = (int)(charInfo->m_width * scale);
        int height = (int)(charInfo->m_height * scale);
        
        // Triangle 1
        // top left
        attrib[0].x = x;
        attrib[0].y = y + height;
        attrib[0].u = charInfo->m_uleft;
        attrib[0].v = charInfo->m_vtop;

        // bottom left
        attrib[1].x = x;
        attrib[1].y = y;
        attrib[1].u = charInfo->m_uleft;
        attrib[1].v = charInfo->m_vbottom;

        // top right
        attrib[2].x = x + width;
        attrib[2].y = y + height;
        attrib[2].u = charInfo->m_uright;
        attrib[2].v = charInfo->m_vtop;

        // Triangle 2
        // top right
        attrib[3].x = x + width;
        attrib[3].y = y + height;
        attrib[3].u = charInfo->m_uright;
        attrib[3].v = charInfo->m_vtop;

        // bottom left
        attrib[4].x = x;
        attrib[4].y = y;
        attrib[4].u = charInfo->m_uleft;
        attrib[4].v = charInfo->m_vbottom;

        // bottom right
        attrib[5].x = x + width;
        attrib[5].y = y;
        attrib[5].u = charInfo->m_uright;
        attrib[5].v = charInfo->m_vbottom;

        x += width;
        attrib += VerticesPerChar;
        charCount++;
    }

    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(NORMAL_INDEX);
    
    // Upload the created vertex data into the VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(VertexAttribsTexCoords) * VerticesPerChar * charCount,
                 m_textVertices, GL_STREAM_DRAW);

    glVertexAttribPointer(COORD_INDEX, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, x));
    glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttribsTexCoords),
                          (const GLvoid*)offsetof(VertexAttribsTexCoords, u));

    // Draw all the chars as two triangles each
    glDrawArrays(GL_TRIANGLES, 0, charCount * VerticesPerChar);
    
    glEnableVertexAttribArray(NORMAL_INDEX);
    glEnable(GL_DEPTH_TEST);

    LOG_GL_ERROR();
}

