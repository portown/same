// surface.hxx

#pragma once

#include <windows.h>


namespace same
{
    namespace ui
    {
        class Surface
        {
        public:
            Surface() = default;

            Surface(Surface const&)            = delete;
            Surface& operator=(Surface const&) = delete;

            Surface(Surface&&)            = default;
            Surface& operator=(Surface&&) = default;

            HDC     dcHandle_;
            HBITMAP bitmapHandle_;
        };
    }
}
