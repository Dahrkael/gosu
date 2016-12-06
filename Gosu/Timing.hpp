//! \file Timing.hpp
//! Functions for timing.

#ifndef GOSU_TIMING_HPP
#define GOSU_TIMING_HPP

namespace Gosu
{
    //! Freezes the current thread for at least the specified time.
	__declspec(dllexport) void sleep(unsigned milliseconds);

    //! Incrementing, possibly wrapping millisecond timer.
	__declspec(dllexport) unsigned long milliseconds();
}

#endif
