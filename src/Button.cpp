#include "Button.h"
#include "CommonFunctions.h"

namespace CommonGL {

Button::Button(GLuint texture, ButtonListener* listener)
    : BaseWidget(TypeButton, Rect()),
      m_texture(texture)
{
    if ( listener != NULL )
    {
        m_listeners.push_back(listener);
    }
}

Button::Button(Rect bounds, GLuint texture, ButtonListener* listener)
    : BaseWidget(TypeButton, bounds),
      m_texture(texture)
{
    if ( listener != NULL )
    {
        m_listeners.push_back(listener);
    }
}

Button::~Button()
{
    m_listeners.clear();
}

void Button::AddButtonListener(ButtonListener* listener)
{
    m_listeners.push_back(listener);
}

void Button::RemoveButtonListener(ButtonListener* listener)
{
    m_listeners.remove(listener);
}

void Button::TouchUpInside(int, int)
{
    std::list<ButtonListener*>::iterator iter;
    for ( iter = m_listeners.begin(); iter != m_listeners.end(); iter++ )
    {
        (*iter)->ButtonPressed(this);
    }
}

void Button::Render()
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(m_context->m_textureShaderTextureLoc, 0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    DrawImage2D(TransformedRect(), m_context->m_viewportWidth,
                m_context->m_viewportHeight);
}

} // namespace CommonGL

