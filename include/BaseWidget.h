#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <list>

#include "Rect.h"
#include "OpenGLAPI.h"

// Forward declarations
class GLController;

namespace CommonGL {

// Forward declarations
class BaseWidget;

/**
 * Environment context for widgets.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
struct WidgetContext
{
    // Viewport dimensions
    int m_viewportWidth;
    int m_viewportHeight;
    GLuint m_fullscreenRectVertexBuffer;

    // Shader program for rendering a widget with texture
    GLuint m_textureShaderProgram;
    GLint m_textureShaderMvpLoc;
    GLint m_textureShaderTextureLoc;
    GLint m_textureShaderHighlightLoc;

    // Shader program for rendering a widget with color
    GLuint m_colorShaderProgram;
    GLint m_colorShaderMvpLoc;
    GLint m_colorShaderColorLoc;

    // The widget currently being pressed, NULL if none
    BaseWidget* m_pressedWidget;

    // Pointer to a GLController that manages the widgets
    GLController* m_glController;
};

/**
 * Touch listener for widgets.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
class WidgetTouchListener
{
public:
    /**
     * A widget received a touch down event.
     *
     * @param widget widget being touched
     * @param x touch point x coordinate (in widget's coordinate space)
     * @param y touch point y coordinate (in widget's coordinate space)
     */
    virtual void WidgetTouched(BaseWidget* widget, int x, int y) = 0;
};

/**
 * Base class for the GL based widgets.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
class BaseWidget
{
public:
    enum WidgetType {
        TypeContainer,
        TypeButton
    };

public: // Public API
    /** Checks if a given coordinate is within the bounds of this widget. */
    virtual bool HitTest(int x, int y);

    /** Sets new bounds rectangle for the widget. */
    virtual void SetBounds(Rect bounds);

    /** Returns the widget's bounds. */
    virtual Rect GetBounds();
    virtual Rect& BoundsRef() { return m_bounds; }

    /** Sets visibility of the widget. */
    virtual void SetVisible(bool visible);

    /** Returns the visibility of the widget. */
    virtual bool IsVisible() { return m_isVisible; }

    virtual void AddTouchListener(WidgetTouchListener* listener);
    virtual void RemoveTouchListener(WidgetTouchListener* listener);

    /**
     * Sets a widget context which supplies data required to render the widgets.
     * A widget with a context set is 'bound' ie. being part of active
     * processing. A widget with NULL context is 'unbound'.
     */
    virtual void SetContext(WidgetContext* context);

protected: // Construction and destruction
    BaseWidget(WidgetType type, Rect bounds);
    virtual ~BaseWidget();

protected:
    // Returns widget bounds transformed by the widgets' parents
    Rect TransformedRect();
    virtual void Render() = 0;

private:
    // These get called by GLController; the coordinates are viewport
    // coordinates. Classes inheriting this should NOT keep any state with
    // these; TouchDownInside() will NOT always follow TouchUpInside()
    virtual void TouchUpInside(int x, int y);
    virtual void TouchDownInside(int x, int y);

protected: // Data
    WidgetType m_type;
    BaseWidget* m_parent;
    Rect m_bounds;
    bool m_isVisible;
    std::list<WidgetTouchListener*> m_touchListeners;

    // Environment context
    WidgetContext* m_context;

    // Allow GLController access to privates
    friend class ::GLController;

    // Allow inheritants access to m_parent
    friend class Container;
};

} // namespace CommonGL

#endif // BASEWIDGET_H
