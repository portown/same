// surface.hxx

#ifndef SAME_UI_SURFACE_HXX
#define SAME_UI_SURFACE_HXX

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <windows.h>

#include "geometry.hxx"


namespace same::ui
{
    struct SurfaceImpl
    {
        virtual geometry::Box box() const = 0;

        virtual HDC getDC() const = 0;
    };
    using ImplPtr = std::shared_ptr<SurfaceImpl>;

    class Surface;
    using SurfacePtr = std::shared_ptr<Surface>;

    class Surface
    {
    public:
        static auto create(geometry::Size const& size)->SurfacePtr;
        static auto fromBitmapFile(std::wstring_view fileName)->SurfacePtr;
        static auto fromBitmapResource(HINSTANCE instanceHandle, WORD resourceId)->SurfacePtr;
        static auto onPaint(HWND windowHandle, PAINTSTRUCT& paintStruct)->SurfacePtr;

        geometry::Box box() const { return impl_->box(); }

        HDC getDC() const { return impl_->getDC(); }

        void paint(COLORREF color);
        void blitTo(Surface& surface) const;
        void blitTo(Surface& surface, geometry::Point const&) const;

        auto view(geometry::Box const& box) const->SurfacePtr;

    private:
        template <class T, class... Args>
        static auto create(Args&& ... args)->SurfacePtr
        {
            ImplPtr    impl(new T(std::forward<Args>(args) ...));
            SurfacePtr surface(new Surface(impl));
            return surface;
        }

        explicit Surface(ImplPtr impl) : impl_ {impl}
        {}

    private:
        ImplPtr impl_;
    };
}

#endif  // SAME_UI_SURFACE_HXX
