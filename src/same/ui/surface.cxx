// surface.cxx

#include "surface.hxx"


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

    return create(dcHandle, bitmapHandle);
}

auto same::ui::Surface::fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>
{
    auto const bitmapHandle = static_cast<HBITMAP>(LoadImage(nullptr, fileName.c_str(), IMAGE_BITMAP, 0, 0,
                                                             LR_CREATEDIBSECTION | LR_LOADFROMFILE));
    auto const dcHandle = CreateCompatibleDC(nullptr);

    return create(dcHandle, bitmapHandle);
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

    return create(dcHandle, bitmapHandle);
}

auto same::ui::Surface::create(HDC const dcHandle, HBITMAP const bitmapHandle)->std::shared_ptr<Surface>
{
    std::shared_ptr<Surface> p(new Surface(dcHandle, bitmapHandle), &Surface::destroy);

    return p;
}

void same::ui::Surface::destroy(Surface* const p)
{
    DeleteObject(p->bitmapHandle_);
    DeleteDC(p->dcHandle_);

    delete p;
}

same::ui::Surface::Surface(HDC const dcHandle, HBITMAP const bitmapHandle)
    : dcHandle_ { dcHandle }, bitmapHandle_ { bitmapHandle }
{
    SelectObject(dcHandle, bitmapHandle);
}
