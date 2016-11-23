// Process watch-tvseries.net URLs
/*
 *  Copyright © 2016 tastytea <tastytea@tastytea.de>
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

#ifndef GEEKTV_HPP
#define GEEKTV_HPP

#include "website.hpp"
#include "config.hpp"
#include "global.hpp"
#include <vector>
#include <string>

class Geektv : public Website
{
public:
	explicit Geektv(const Config &cfg) : Website(cfg) {};
	const uint8_t getlinks(std::vector<Global::episodepair> &episodes);

private:
	/*! \param &pages Vector of std::string, containing URLs
		\return 0 on success */
	const uint8_t get_episode_pages(std::vector<std::string> &pages);
};

#endif
