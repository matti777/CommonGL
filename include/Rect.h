#ifndef RECT_H
#define RECT_H

namespace CommonGL {
    
/**
 * Simple 2D rectangle to use for 2D drawing, detecting screen regions etc.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class Rect
{
public: // Construction and destruction
    Rect(int left, int top, int right, int bottom);
    Rect();
    virtual ~Rect();

public: // Public API
    static Rect Centered(int centerX, int centerY, int size);
    static Rect Centered(int centerX, int centerY, int width, int height);

    void Scale(float scale);
    void Set(int left, int top, int right, int bottom);

    /** Moves the rect by a relative amount of (x,y). */
    void MoveBy(int x, int y);

    /** Moves the rect to an absolute position of left,top = (x,y). */
    void MoveTo(int x, int y);

    /**
     * Sets the rect into a square from center coordinates and a size value;
     * the size value represents both width and height.
     */
    void SetCentered(int centerX, int centerY, int size);

    /**
     * Sets the rect into a quad from center coordinates and size values.
     */
    void SetCentered(int centerX, int centerY, int width, int height);

    /** Detects if the given coordinates are inside the rect. */
    bool IsInside(int x, int y);

    int GetWidth() const { return m_right - m_left; }
    int GetHeight() const { return m_bottom - m_top; }

public:
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;
};

} // namespace CommonGL

#endif // RECT_H
