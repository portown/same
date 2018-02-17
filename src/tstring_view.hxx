// tstring_view.hxx

#pragma once

#include <string_view>

#include <tchar.h>


namespace tch
{
    using tstring_view = std::basic_string_view<TCHAR>;
}
