#ifndef CONTAINER_H
#define CONTAINER_H

#include <list>

#include "BaseWidget.h"

namespace CommonGL {

/**
 * GL widget that may contain child widgets. Any child widgets will function
 * within the local coordinate system of their parent Container.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
class Container : public BaseWidget
{
public: // Construction and destruction
    Container(Rect bounds, WidgetTouchListener* = NULL);
    Container(WidgetTouchListener* = NULL);
    virtual ~Container();

public: // Public API
    virtual void Add(BaseWidget* child);
    virtual void Remove(BaseWidget* child);

    /**
     * Returns a pointer to the background color of this Container;
     * contains 4 values for RGBA.
     */
    virtual float* ColorPtr() { return m_color; }

protected:
    virtual void Render();

private: // Data
    std::list<BaseWidget*> m_children;
    float m_color[4];
};

} // namespace CommonGL

#endif // CONTAINER_H
