#include "BaseWidget.h"
#include "GLController.h"
#include "CommonFunctions.h"

namespace CommonGL {

BaseWidget::BaseWidget(WidgetType type, Rect bounds)
    : m_type(type),
      m_parent(NULL),
      m_bounds(bounds),
      m_isVisible(true),
      m_context(NULL)
{
}

BaseWidget::~BaseWidget()
{
    if ( m_context != NULL )
    {
        m_context->m_glController->Remove(this);
    }
    m_touchListeners.clear();
}

void BaseWidget::TouchUpInside(int, int)
{
    // No implementation
}

void BaseWidget::TouchDownInside(int x, int y)
{
    LOG_DEBUG("TouchDownInside() type: %d, num touchlisteners: %d",
              m_type, m_touchListeners.size());

    // Convert touch point from viewport coordinates to local coordinates
    x -= m_context->m_viewportWidth;
    y -= m_context->m_viewportHeight;

    // Notify touch listeners
    std::list<WidgetTouchListener*>::iterator iter;
    for ( iter = m_touchListeners.begin();
          iter != m_touchListeners.end(); iter++ )
    {
        (*iter)->WidgetTouched(this, x, y);
    }
}

Rect BaseWidget::TransformedRect()
{
    Rect rect(m_bounds);
    BaseWidget* parent = m_parent;
    while ( parent != NULL )
    {
        Rect parentBounds = parent->GetBounds();
        rect.MoveBy(parentBounds.m_left, parentBounds.m_top);
        parent = parent->m_parent;
    }

    return rect;
}

bool BaseWidget::HitTest(int x, int y)
{
    if ( m_parent == NULL )
    {
        return m_bounds.IsInside(x, y);
    }
    else
    {
        return TransformedRect().IsInside(x, y);
    }
}

void BaseWidget::SetBounds(Rect bounds)
{
    m_bounds = bounds;
}

Rect BaseWidget::GetBounds()
{
    return m_bounds;
}

void BaseWidget::SetVisible(bool visible)
{
    m_isVisible = visible;
}

void BaseWidget::SetContext(WidgetContext* context)
{
    m_context = context;
}

void BaseWidget::AddTouchListener(WidgetTouchListener* listener)
{
    LOG_DEBUG("AddTouchListener() adding");
    m_touchListeners.push_front(listener);
}

void BaseWidget::RemoveTouchListener(WidgetTouchListener* listener)
{
    m_touchListeners.remove(listener);
}

} // namespace CommonGL
