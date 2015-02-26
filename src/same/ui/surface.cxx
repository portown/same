// surface.cxx

#include "surface.hxx"


namespace
{
    namespace geom = same::ui::geometry;

    class SurfaceDDBImpl : public same::ui::SurfaceImpl
    {
    public:
        explicit SurfaceDDBImpl(HDC const dcHandle, HBITMAP const bitmapHandle)
            : dcHandle_ { dcHandle }, bitmapHandle_ { bitmapHandle }
        {
            SelectObject(dcHandle, bitmapHandle);
        }

        ~SurfaceDDBImpl() override
        {
            DeleteObject(bitmapHandle_);
            DeleteDC(dcHandle_);
        }

        geom::Box box() const override
        {
            BITMAP bitmap;
            GetObject(bitmapHandle_, sizeof(BITMAP), &bitmap);

            return geom::makeBox(
                geom::makePoint(0, 0),
                geom::makePoint(bitmap.bmWidth, bitmap.bmHeight));
        }

        HDC getDC() const override
        {
            return dcHandle_;
        }

    private:
        HDC     dcHandle_;
        HBITMAP bitmapHandle_;
    };

    class SurfacePaintImpl : public same::ui::SurfaceImpl
    {
    public:
        explicit SurfacePaintImpl(HWND const windowHandle, PAINTSTRUCT& paintStruct)
            : windowHandle_ { windowHandle }, paintStruct_ { paintStruct }
        {
            dcHandle_ = BeginPaint(windowHandle, &paintStruct);
        }

        ~SurfacePaintImpl() override
        {
            EndPaint(windowHandle_, &paintStruct_);
        }

        geom::Box box() const override
        {
            RECT rect;
            GetClientRect(windowHandle_, &rect);

            return rect;
        }

        HDC getDC() const override
        {
            return dcHandle_;
        }

    private:
        HWND         windowHandle_;
        PAINTSTRUCT& paintStruct_;
        HDC          dcHandle_;
    };
}


auto same::ui::Surface::create(geometry::Size const & size)
->std::shared_ptr<Surface>
{
    using namespace same::ui::geometry;

    auto const displayDC    = GetDC(nullptr);
    auto const dcHandle     = CreateCompatibleDC(displayDC);
    auto const bitmapHandle = CreateCompatibleBitmap(displayDC, getWidth(size), getHeight(size));
    ReleaseDC(nullptr, displayDC);

    if (dcHandle == nullptr || bitmapHandle == nullptr)
    {
        return nullptr;
    }

    return create<SurfaceDDBImpl>(dcHandle, bitmapHandle);
}

auto same::ui::Surface::fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>
{
    auto const bitmapHandle = static_cast<HBITMAP>(LoadImage(nullptr, fileName.c_str(), IMAGE_BITMAP, 0, 0,
                                                             LR_CREATEDIBSECTION | LR_LOADFROMFILE));
    auto const dcHandle = CreateCompatibleDC(nullptr);

    return create<SurfaceDDBImpl>(dcHandle, bitmapHandle);
}

auto same::ui::Surface::fromBitmapResource(
    HINSTANCE const instanceHandle,
    WORD const resourceId)
->std::shared_ptr<Surface>
{
    auto const dcHandle     = CreateCompatibleDC(nullptr);
    auto const bitmapHandle = static_cast<HBITMAP>(LoadImage(instanceHandle,
                                                             MAKEINTRESOURCE(resourceId), IMAGE_BITMAP, 0, 0,
                                                             LR_SHARED | LR_DEFAULTSIZE));

    return create<SurfaceDDBImpl>(dcHandle, bitmapHandle);
}

auto same::ui::Surface::onPaint(
    HWND const windowHandle,
    PAINTSTRUCT & paintStruct)
->std::shared_ptr<Surface>
{
    return create<SurfacePaintImpl>(windowHandle, paintStruct);
}

void same::ui::Surface::paint(COLORREF const color)
{
    auto const colorBrush = CreateSolidBrush(color);

    auto const dcHandle      = getDC();
    auto const originalBrush = SelectObject(dcHandle, colorBrush);
    auto const box           = this->box();
    PatBlt(dcHandle, geom::getLeft(box), geom::getTop(box), geom::getRight(box), geom::getBottom(box), PATCOPY);
    SelectObject(dcHandle, originalBrush);

    DeleteObject(colorBrush);
}

void same::ui::Surface::blitTo(Surface& surface) const
{
    auto const box = this->box();
    BitBlt(surface.getDC(), 0, 0, geom::getRight(box), geom::getBottom(box),
           getDC(), geom::getLeft(box), geom::getTop(box), SRCCOPY);
}
