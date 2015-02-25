// surface.cxx

#include "surface.hxx"


auto same::ui::Surface::fromBitmapFile(std::string const & fileName)->std::shared_ptr<Surface>
{
    auto const bitmapHandle = static_cast<HBITMAP>(LoadImage(nullptr, fileName.c_str(), IMAGE_BITMAP, 0, 0,
                                                             LR_CREATEDIBSECTION | LR_LOADFROMFILE));
    auto const dcHandle = CreateCompatibleDC(nullptr);

    SelectObject(dcHandle, bitmapHandle);

    return std::make_shared<Surface>(dcHandle, bitmapHandle);
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
    SelectObject(dcHandle, bitmapHandle);

    return std::make_shared<Surface>(dcHandle, bitmapHandle);
}

same::ui::Surface::Surface(HDC const dcHandle, HBITMAP const bitmapHandle)
    : dcHandle_ { dcHandle }, bitmapHandle_ { bitmapHandle }
{
}
