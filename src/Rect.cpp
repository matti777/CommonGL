#include "Rect.h"
#include "CommonFunctions.h"

namespace CommonGL {

Rect::Rect()
    : m_left(0),
      m_top(0),
      m_right(0),
      m_bottom(0)
{
}

Rect::Rect(int left, int top, int right, int bottom)
    : m_left(left),
      m_top(top),
      m_right(right),
      m_bottom(bottom)
{
}

Rect::~Rect()
{
}

void Rect::Scale(float scale)
{
    float centerX = (m_right - m_left) / 2;
    float centerY = (m_bottom - m_top) / 2;
    float scaledHalfX = centerX * scale;
    float scaledHalfY = centerY * scale;
    m_left = (int)(centerX - scaledHalfX);
    m_right = (int)(centerX + scaledHalfX);
    m_top = (int)(centerY - scaledHalfY);
    m_bottom = (int)(centerY + scaledHalfY);
}

void Rect::MoveBy(int x, int y)
{
    m_left += x;
    m_top += y;
    m_right += x;
    m_bottom += y;
}

void Rect::MoveTo(int x, int y)
{
    // Calculate difference between left/x and top/y and move the rect by them
    int xdiff = x - m_left;
    int ydiff = y - m_top;
    m_left += xdiff;
    m_top += ydiff;
    m_right += xdiff;
    m_bottom += ydiff;
}

Rect Rect::Centered(int centerX, int centerY, int size)
{
    Rect rect;
    rect.SetCentered(centerX, centerY, size);
    return rect;
}

Rect Rect::Centered(int centerX, int centerY, int width, int height)
{
    Rect rect;
    rect.SetCentered(centerX, centerY, width, height);
    return rect;
}

void Rect::SetCentered(int centerX, int centerY, int size)
{
    int halfsize = (size / 2);
    m_left = centerX - halfsize;
    m_top = centerY - halfsize;
    m_right = centerX + halfsize;
    m_bottom = centerY + halfsize;
}

void Rect::SetCentered(int centerX, int centerY, int width, int height)
{
    int halfWidth = (width / 2);
    int halfHeight = (height / 2);

    m_left = centerX - halfWidth;
    m_top = centerY - halfHeight;
    m_right = centerX + halfWidth;
    m_bottom = centerY + halfHeight;
}

void Rect::Set(int left, int top, int right, int bottom)
{
    m_left = left;
    m_top = top;
    m_right = right;
    m_bottom = bottom;
}

bool Rect::IsInside(int x, int y)
{
//    LOG_DEBUG("left = %d, right = %d, top = %d, bottom = %d, x,y = (%d,%d)",
//              m_left, m_right, m_top, m_bottom, x, y);
    return ((x >= m_left) && (x <= m_right) && (y >= m_top) && (y <= m_bottom));
}

} // namespace
