// geometry.hxx

#pragma once

#include <windows.h>


namespace same::ui::geometry
{
    using CoordinateType = LONG;

    using Point = POINT;
    using Box   = RECT;
    using Size  = SIZE;

    inline CoordinateType getX(Point const& point) { return point.x; }
    inline CoordinateType getY(Point const& point) { return point.y; }

    inline void setX(Point& point, CoordinateType value) { point.x = value; }
    inline void setY(Point& point, CoordinateType value) { point.y = value; }

    inline CoordinateType getLeft(Box const& box) { return box.left; }
    inline CoordinateType getTop(Box const& box) { return box.top; }
    inline CoordinateType getRight(Box const& box) { return box.right; }
    inline CoordinateType getBottom(Box const& box) { return box.bottom; }

    inline CoordinateType getWidth(Size const& size) { return size.cx; }
    inline CoordinateType getHeight(Size const& size) { return size.cy; }

    inline Point makePoint(CoordinateType x, CoordinateType y)
    {
        Point p;
        setX(p, x);
        setY(p, y);

        return p;
    }

    inline Box makeBox(Point const& minCorner, Point const& maxCorner)
    {
        Box box;
        box.left   = getX(minCorner);
        box.top    = getY(minCorner);
        box.right  = getX(maxCorner);
        box.bottom = getY(maxCorner);

        return box;
    }

    inline Box makeBox(Point const& minCorner, Size const& size)
    {
        Box box;
        box.left   = getX(minCorner);
        box.top    = getY(minCorner);
        box.right  = getX(minCorner) + getWidth(size);
        box.bottom = getY(minCorner) + getHeight(size);

        return box;
    }

    inline Size makeSize(CoordinateType x, CoordinateType y)
    {
        Size size;
        size.cx = x;
        size.cy = y;

        return size;
    }

    inline Point getMinCorner(Box const& box) { return makePoint(box.left, box.top); }
    inline Point getMaxCorner(Box const& box) { return makePoint(box.right, box.bottom); }

    inline CoordinateType getWidth(Box const& box)
    {
        return getRight(box) - getLeft(box);
    }

    inline CoordinateType getHeight(Box const& box)
    {
        return getBottom(box) - getTop(box);
    }

    inline Point offset(Point const& point, Point const& offset)
    {
        Point result = point;
        result.x += getX(offset);
        result.y += getY(offset);

        return result;
    }

    inline Box offset(Box const& box, Point const& offset)
    {
        Box result = box;
        result.left   += getX(offset);
        result.top    += getY(offset);
        result.right  += getX(offset);
        result.bottom += getY(offset);

        return result;
    }
}
