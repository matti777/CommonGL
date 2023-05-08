#include "Container.h"
#include "CommonFunctions.h"

namespace CommonGL {

Container::Container(Rect bounds, WidgetTouchListener* listener)
    : BaseWidget(TypeContainer, bounds),
      m_color()
{
    AddTouchListener(listener);
}

Container::Container(WidgetTouchListener* listener)
    : BaseWidget(TypeContainer, Rect()),
      m_color()
{
    AddTouchListener(listener);
}

Container::~Container()
{
    m_children.clear();
}

void Container::Add(BaseWidget* child)
{
    m_children.push_front(child);
    child->m_parent = this;
}

void Container::Remove(BaseWidget* child)
{
    m_children.remove(child);
    child->m_parent = NULL;
}

void Container::Render()
{
    glUniform4fv(m_context->m_colorShaderColorLoc, 1, m_color);
    //DrawQuad2D(m_context->m_fullscreenRectVertexBuffer, g_rectangleIndexBuffer);
    DrawQuad2D(m_bounds,
               m_context->m_viewportWidth, m_context->m_viewportHeight);
}

} // namespace CommonGL
