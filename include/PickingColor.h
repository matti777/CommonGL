#ifndef PICKINGCOLORS_H
#define PICKINGCOLORS_H

/**
 * Represents a pickable color. The color values are 5 bits per component
 * to allow for less than 32 bit rendering resolutions.
 */
class PickingColor
{
public:
    // if enabled, would need to work around the compiler warning
//    PickingColor()
//        : m_red(0),
//          m_green(0),
//          m_blue(0)
//    {
//    }

//    PickingColor(int red, int green, int blue)
//        : m_red(red),
//          m_green(green),
//          m_blue(blue)
//    {
//    }

    PickingColor& operator=(const PickingColor& other)
    {
        m_red = other.m_red;
        m_green = other.m_green;
        m_blue = other.m_blue;
        return *this;
    }

    /**
     * Checks if the given color components match this picking
     * color. The color components must be 8 bits each, in the 0-255 range.
     */
    bool Matches(int red, int green, int blue) const
    {
        // Compare the downshifted components to the picking color's 5bit ones
        return ( ((red >> 3) == m_red) &&
                 ((green >> 3) == m_green) &&
                 ((blue >> 3) == m_blue) );
    }

    /**
     * Converts the color into a float-based OpenGL color. The parameter must
     * hold 3 floats.
     */
    void ToFloats(float* floats) const
    {
        // convert the 0..31 range 5bit values into 0..1 floats
        floats[0] = m_red / 31.0;
        floats[1] = m_green / 31.0;
        floats[2] = m_blue / 31.0;
    }

public:
    // 5 bit color components
    int m_red;
    int m_green;
    int m_blue;
};

#endif // PICKINGCOLORS_H
