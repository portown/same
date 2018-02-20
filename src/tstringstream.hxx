// tstringstream.hxx

#ifndef TSTRINGSTREAM_HXX
#define TSTRINGSTREAM_HXX

#include <sstream>

#include <tchar.h>


namespace tch
{
    using tostringstream = std::basic_ostringstream<TCHAR>;
}

#endif  // TSTRINGSTREAM_HXX
