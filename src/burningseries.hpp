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

#ifndef BURNINGSERIES_HPP
#define BURNINGSERIES_HPP

#include "website.hpp"
#include "config.hpp"
#include <vector>
#include <string>

class Burningseries : public Website
{
public:
	explicit Burningseries(const Config &cfg) : Website(cfg) {};
	uint8_t getlinks(std::vector<episodepair> &episodes);

private:
	/*! \param &pages Vector of URLs and providers, pair-wise
		\return 0 on success */
	uint8_t get_episode_pages(std::vector<episodepair> &pages);
};

#endif
