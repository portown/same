// surface.cxx

#include "surface.hxx"


auto same::ui::Surface::fromBitmapFile(std::string const & fileName)->Surface
{
    auto bitmapHandle = static_cast<HBITMAP>(LoadImage(nullptr, fileName.c_str(), IMAGE_BITMAP, 0, 0,
                                                       LR_CREATEDIBSECTION | LR_LOADFROMFILE));

    return Surface { CreateCompatibleDC(nullptr), bitmapHandle };
}

same::ui::Surface::Surface(HDC const dcHandle, HBITMAP const bitmapHandle)
    : dcHandle_ { dcHandle }, bitmapHandle_ { bitmapHandle }
{
    SelectObject(dcHandle_, bitmapHandle_);
}
