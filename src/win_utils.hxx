// win_utils.hxx

#ifndef WIN_UTILS_HXX
#define WIN_UTILS_HXX

#include <optional>
#include <string>
#include <utility>

#include <windows.h>


namespace win
{
    inline auto loadString(::HINSTANCE instance, ::UINT resourceId, std::size_t capacity = 256u)
    ->std::optional<std::wstring>
    {
        std::wstring str(capacity, L'\0');
        using size_type = decltype(str)::size_type;

        auto length = ::LoadString(instance, resourceId, str.data(), str.size());
        if (length == 0) return std::nullopt;
        while (static_cast<size_type>(length) == str.size())
        {
            str.resize(static_cast<size_type>(str.size() * 1.5));
            length = ::LoadString(instance, resourceId, str.data(), str.size());
            if (length == 0) return std::nullopt;
        }
        str.erase(length);
        return std::make_optional(std::move(str));
    }
}

#endif  // WIN_UTILS_HXX
