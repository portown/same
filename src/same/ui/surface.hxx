// surface.hxx

#pragma once

#include <memory>
#include <string>

#include <windows.h>


namespace same
{
    namespace ui
    {
        class Surface
        {
        public:
            static auto fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>;

            explicit Surface() = default;
            explicit Surface(HDC dcHandle, HBITMAP bitmapHandle);

            Surface(Surface const&)            = delete;
            Surface& operator=(Surface const&) = delete;

            Surface(Surface&&)            = default;
            Surface& operator=(Surface&&) = default;

            HDC     dcHandle_;
            HBITMAP bitmapHandle_;
        };
    }
}
