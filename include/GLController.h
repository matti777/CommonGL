#ifndef GLCONTROLLER_H
#define GLCONTROLLER_H

#include <stdio.h>
#include <list>

// include OpenGL API
#include "OpenGLAPI.h"

#include "Rect.h"
#include "BaseWidget.h"

// Forward declarations

/**
 * This class provides some generic OpenGL related functionality.
 * It is implemented in portable C++.
 */
class GLController
{
protected:
    GLController();
    virtual ~GLController();

public:
    /**
     * Perform any actions required to initialize the controller.
     * @return true on success
     */
    virtual bool InitController();

    /** Pauses or resumes rendering. */
    virtual void SetPaused(bool paused) = 0;

    /** Inheriting classes must call when OpenGL viewport is resized */
    virtual void ViewportResized(int width, int height);

    /** Draws the widgets; should be called by implementing class. */
    virtual void DrawWidgets();

    /** Request a redraw. */
    virtual void Redraw() = 0;

    /**
     * Draw everything. This shouldn't be called directly, instead
     * via Redraw().
     * @return true if successful, false if error and should abort
     */
    virtual bool Draw() = 0;

    /**
     * Pointer dragging started.
     *
     * @param x X coordinate where touch started
     * @param y Y coordinate where touch started
     * @param a pointer to a touch object (can be used as an id)
     * @return true if this class handled this event
     */
    virtual bool TouchStarted(const void* touch, int x, int y);

    /**
     * The pointer was dragged (moved) to new screen coordinates.
     *
     * @param y new Y coordinate
     * @param x new X coordinate
     * @param a pointer to a touch object (can be used as an id)
     * @return true if this class handled this event
     */
    virtual bool TouchMoved(const void* touch, int x, int y);

    /**
     * The pointer dragging has ended.
     *
     * @param x X coordinate where touch ended
     * @param y Y coordinate where touch ended
     * @param a pointer to a touch object (can be used as an id)
     * @return true if this class handled this event
     */
    virtual bool TouchEnded(const void* touch, int x, int y);

    /** The screen was pinched (gesture). */
    virtual void PinchGesture(float scaleFactor, float rotationAngle);

    //TODO move this widget management into class called WidgetManager etc
    // and make GLController inherit it

    /** Adds a widget; the callee does NOT assume ownership of the object. */
    virtual void Add(CommonGL::BaseWidget* widget);

    /** Removes a widget. */
    virtual void Remove(CommonGL::BaseWidget* widget);

protected:
    virtual bool InitWidgets();
    virtual void DeinitWidgets();
    CommonGL::BaseWidget* FindWidget(int x, int y);

protected:
    // Whether to automatically init widget system in InitController()
//    bool m_initWidgets;

    // Simple color shader
    GLuint m_simpleColorProgram;
    GLint m_simpleColorMvpLoc;
    GLint m_simpleColorColorLoc;

    // current viewport dimensions
    int m_viewportWidth;
    int m_viewportHeight;
    GLuint m_fullscreenRectVertexBuffer;
    CommonGL::Rect m_fullScreenRect;

    // Whether depth textures are supported
    bool m_hasDepthTextureExtension;

    // Widget shader programs
//    GLuint m_imageWidgetProgram;
    
    // Widget context
    CommonGL::WidgetContext m_widgetContext;

    // List of added widgets
    std::list<CommonGL::BaseWidget*> m_widgets;

    // id of the default frame buffer
    GLuint m_defaultFrameBuffer;

    // Orthographic projection matrix
    float m_orthoProjectionMatrix[16];
};

#endif // GLCONTROLLER_H
