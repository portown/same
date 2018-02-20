// tstring_view.hxx

#ifndef TSTRING_VIEW_HXX
#define TSTRING_VIEW_HXX

#include <string_view>

#include <tchar.h>


namespace tch
{
    using tstring_view = std::basic_string_view<TCHAR>;
}

#endif  // TSTRING_VIEW_HXX
