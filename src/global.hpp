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

#include "config/config.hpp"

#ifdef DEBUG
	#define ttdebug std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] DEBUG: "
#else
	#define ttdebug false && std::cerr
#endif
#define tterror std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] ERROR: "

namespace global
{
	static constexpr char version[] = "2.4.0";
	static constexpr char git_commit[] = "8a582cbc1c7015229765058bf5855c3eb32fe238";

	// Error codes
	enum Error
	{
		None = 0,
		InvalidArgument = 1,
		ConfigError = 1,
		FeatureGone = 5,
		ParseError = 6,
		IOError = 7
	};
	// URL, provider|title
	typedef std::pair<std::string, const std::string> episodepair;

	extern Config config;	// Defined in config.cpp
}

#endif
