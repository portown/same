// surface.hxx

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <windows.h>


namespace same
{
    namespace ui
    {
        struct SurfaceImpl
        {
            virtual ~SurfaceImpl() {}

            virtual unsigned int getWidth() const  = 0;
            virtual unsigned int getHeight() const = 0;

            virtual HDC getDC() const = 0;
        };

        class Surface
        {
        public:
            static auto create(unsigned int width, unsigned int height)->std::shared_ptr<Surface>;
            static auto fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>;
            static auto fromBitmapResource(HINSTANCE instanceHandle, WORD resourceId)->std::shared_ptr<Surface>;
            static auto onPaint(HWND windowHandle, PAINTSTRUCT & paintStruct)->std::shared_ptr<Surface>;

            unsigned int getWidth() const { return impl_->getWidth(); }
            unsigned int getHeight() const { return impl_->getHeight(); }

            HDC getDC() const { return impl_->getDC(); }

            void paint(COLORREF color);

        private:
            template <class T, class ... Args>
            static auto create(Args ... args)->std::shared_ptr<Surface>
            {
                std::unique_ptr<SurfaceImpl> impl(new T(args ...));
                std::shared_ptr<Surface>     surface(new Surface(std::move(impl)));
                return surface;
            }

            explicit Surface(std::unique_ptr<SurfaceImpl> impl) : impl_ { std::move(impl) }
            {}

        private:
            std::unique_ptr<SurfaceImpl> impl_;
        };
    }
}
