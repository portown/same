// surface.cxx

#include "surface.hxx"


namespace
{
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

        unsigned int getWidth() const override
        {
            BITMAP bitmap;
            GetObject(bitmapHandle_, sizeof(BITMAP), &bitmap);

            return static_cast<unsigned int>(bitmap.bmWidth);
        }

        unsigned int getHeight() const override
        {
            BITMAP bitmap;
            GetObject(bitmapHandle_, sizeof(BITMAP), &bitmap);

            return static_cast<unsigned int>(bitmap.bmHeight);
        }

        HDC getDC() const override
        {
            return dcHandle_;
        }

    private:
        HDC     dcHandle_;
        HBITMAP bitmapHandle_;
    };
}


auto same::ui::Surface::create(
    unsigned int const width,
    unsigned int const height)
->std::shared_ptr<Surface>
{
    auto const displayDC    = GetDC(nullptr);
    auto const dcHandle     = CreateCompatibleDC(displayDC);
    auto const bitmapHandle = CreateCompatibleBitmap(displayDC, width, height);
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

void same::ui::Surface::paint(COLORREF const color)
{
    auto const colorBrush = CreateSolidBrush(color);

    auto const dcHandle      = getDC();
    auto const originalBrush = SelectObject(dcHandle, colorBrush);
    PatBlt(dcHandle, 0, 0, getWidth(), getHeight(), PATCOPY);
    SelectObject(dcHandle, originalBrush);

    DeleteObject(colorBrush);
}
