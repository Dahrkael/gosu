//! \file Inspection.hpp
//! A special set of functions designed for tuning Gosu games.

#ifndef GOSU_INSPECTION_HPP
#define GOSU_INSPECTION_HPP

namespace Gosu
{
    //! Returns the current framerate, as determined by an unspecified and possibly
    //! horrible algorithm.
	__declspec(dllexport) int fps();
}

#endif
