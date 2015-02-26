// surface.hxx

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <windows.h>

#include "geometry.hxx"


namespace same
{
    namespace ui
    {
        struct SurfaceImpl
        {
            virtual ~SurfaceImpl() {}

            virtual geometry::Box box() const = 0;

            virtual HDC getDC() const = 0;
        };

        class Surface
        {
        public:
            static auto create(geometry::Size const & size)->std::shared_ptr<Surface>;
            static auto fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>;
            static auto fromBitmapResource(HINSTANCE instanceHandle, WORD resourceId)->std::shared_ptr<Surface>;
            static auto onPaint(HWND windowHandle, PAINTSTRUCT & paintStruct)->std::shared_ptr<Surface>;

            geometry::Box box() const { return impl_->box(); }

            HDC getDC() const { return impl_->getDC(); }

            void paint(COLORREF color);
            void blitTo(Surface& surface) const;

        private:
            template <class T, class ... Args>
            static auto create(Args && ... args)->std::shared_ptr<Surface>
            {
                std::unique_ptr<SurfaceImpl> impl(new T(std::forward<Args>(args) ...));
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
