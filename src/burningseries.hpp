// Process bs.to URLs
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

#ifndef BURNINGSERIES_HPP
#define BURNINGSERIES_HPP

#include "website.hpp"
#include "config.hpp"
#include "global.hpp"
#include <vector>
#include <string>

class Burningseries : public Website
{
public:
	explicit Burningseries(const Config &cfg) : Website(cfg) {};
	const uint8_t getlinks(std::vector<Global::episodepair> &episodes);

private:
	/*! \param &pages Vector of std::pair<std::string, std::string>, containing URLs and providers
		\return 0 on success */
	const uint8_t get_episode_pages(std::vector<Global::episodepair> &pages);
};

#endif
