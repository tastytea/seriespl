// Global constants and typedefs
/*
 *  Copyright © 2016-2017 tastytea <tastytea@tastytea.de>
 *
 *  This file is part of seriespl.
 *
 *  seriespl is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 *
 *  seriespl is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with seriespl. If not, see <https://www.gnu.org/licenses/gpl-2.0.html>.
 *
 ******************************************************************************/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <utility>		// std::pair
#include <string>
#include <iostream>

#include "config.hpp"

#ifdef DEBUG
	#define ttdebug std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] DEBUG: "
#else
	#define ttdebug false && std::cerr
#endif
#define tterror std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] ERROR: "

namespace global
{
	static constexpr char version[] = "2.3.5";
	static constexpr char git_commit[] = "86388bbd6e081ddd644a22fc1d99d75719033448";

	template <typename T>
	constexpr auto toi(T e) noexcept
		-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
	{
		return static_cast<std::underlying_type_t<T>>(e);
	}
	// Error codes
	enum Error
	{
		None = 0,
		InvalidArgument = 1,
		FeatureGone = 5,
		ParseError = 6,
		ConfigError = 1
	};
	// URL, provider|title
	typedef std::pair<std::string, const std::string> episodepair;

	extern Config config;	// Defined in config.cpp
}

#endif
