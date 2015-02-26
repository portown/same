// geometry.hxx

#pragma once

#include <windows.h>


namespace same
{
    namespace ui
    {
        namespace geometry
        {
            using CoordinateType = LONG;

            using Point = POINT;
            using Box   = RECT;
            using Size  = SIZE;

            inline CoordinateType getX(Point const& point) { return point.x; }
            inline CoordinateType getY(Point const& point) { return point.y; }

            inline CoordinateType getLeft(Box const& box) { return box.left; }
            inline CoordinateType getTop(Box const& box) { return box.top; }
            inline CoordinateType getRight(Box const& box) { return box.right; }
            inline CoordinateType getBottom(Box const& box) { return box.bottom; }

            inline CoordinateType getWidth(Size const& size) { return size.cx; }
            inline CoordinateType getHeight(Size const& size) { return size.cy; }

            inline Point makePoint(CoordinateType x, CoordinateType y)
            {
                Point p;
                p.x = x;
                p.y = y;

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

            inline Size makeSize(CoordinateType x, CoordinateType y)
            {
                Size size;
                size.cx = x;
                size.cy = y;

                return size;
            }
        }
    }
}
