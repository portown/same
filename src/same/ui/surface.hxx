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
            static auto create(unsigned int width, unsigned int height)->std::shared_ptr<Surface>;
            static auto fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>;
            static auto fromBitmapResource(HINSTANCE instanceHandle, WORD resourceId)->std::shared_ptr<Surface>;

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
