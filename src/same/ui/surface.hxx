// surface.hxx

#pragma once

#include <string>

#include <windows.h>


namespace same
{
    namespace ui
    {
        class Surface
        {
        public:
            static auto fromBitmapFile(std::string const & fileName)->Surface;

            Surface() = default;

            Surface(Surface const&)            = delete;
            Surface& operator=(Surface const&) = delete;

            Surface(Surface&&)            = default;
            Surface& operator=(Surface&&) = default;

        private:
            explicit Surface(HDC dcHandle, HBITMAP bitmapHandle);

        public:
            HDC     dcHandle_;
            HBITMAP bitmapHandle_;
        };
    }
}
