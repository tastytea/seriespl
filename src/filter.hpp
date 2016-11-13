/*
 *	Copyright © 2016 tastytea <tastytea@tastytea.de>
 *
 *	This file is part of seriespl.
 *
 *	seriespl is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2 of the License.
 *
 *	seriespl is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with seriespl. If not, see <https://www.gnu.org/licenses/gpl-2.0.html>.
 *
 ******************************************************************************/

#ifndef FILTER_HPP
#define FILTER_HPP

#include "config.hpp"
#include <string>
#include <vector>
#include <sys/types.h>
#include <utility>		// std::pair

class Filter
{
public:
	// URL, title
	typedef std::pair<std::string, const std::string> episodepair;

	/*! \param &cfg Config object */
	explicit Filter(const Config &cfg);
	/*!	\param &episodes Vector of std::pair<std::string, std::string>
		containing URLs and titles for episodes. URLs will be replaced.
		\return 0 on success */
	const uint8_t youtube_dl(std::vector<episodepair> &episodes);

private:
	const Config &_cfg;
};

#endif
