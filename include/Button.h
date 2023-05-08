#ifndef BUTTON_H
#define BUTTON_H

#include <list>

#include "BaseWidget.h"
#include "OpenGLAPI.h"

namespace CommonGL {

// Forward declarations
class Button;

/**
 * Listener interface for Button events.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
class ButtonListener
{
public:
    virtual ~ButtonListener() {};
    virtual void ButtonPressed(Button* button) = 0;
};

/**
 * A clickable (tappable) button with an image. The button object does not
 * own the OpenGL texture object.
 *
 * @author Matti Dahlbom
 * @since 1.0
 */
class Button : public BaseWidget
{
public: // Construction and destruction
    /** Constructs a button without bounds; must be set later. */
    Button(GLuint texture, ButtonListener* listener = NULL);
    Button(Rect bounds, GLuint texture, ButtonListener* listener = NULL);
    virtual ~Button();

public: // Public API
    /**
     * Adds a listener for button events; ownership of the pointer is
     * not transferred. The listener will start receiving events immediately.
     */
    void AddButtonListener(ButtonListener* listener);

    /** Removes a given listener; no more events will be sent to it. */
    void RemoveButtonListener(ButtonListener* listener);

protected:
    virtual void Render();

private:
    virtual void TouchUpInside(int x, int y);

private: // Data
    GLuint m_texture;
    std::list<ButtonListener*> m_listeners;

    // Provide GLController access to the privates
    friend class ::GLController;
};

} // namespace CommonGL

#endif // BUTTON_H
