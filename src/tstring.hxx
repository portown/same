// tstring.hxx

#ifndef TSTRING_HXX
#define TSTRING_HXX

#include <string>

#include <tchar.h>


namespace tch
{
    using tstring = std::basic_string<TCHAR>;
}

#endif  // TSTRING_HXX
