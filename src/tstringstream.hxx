// tstringstream.hxx

#pragma once

#include <sstream>

#include <tchar.h>


namespace tch
{
    using tostringstream = std::basic_ostringstream<TCHAR>;
}
