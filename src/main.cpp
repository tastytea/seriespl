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

#include "config.hpp"
#include "burningseries.hpp"
#include "filter.hpp"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[])
{
	Config cfg(argc, argv);
	if (cfg.get_website() == Config::BurningSeries)
	{
		Burningseries website(cfg);
		std::vector<Website::episodepair> episodes;
		Filter filter(cfg);

		website.getlinks(episodes);
		if (cfg.get_direct_url())
		{
			filter.youtube_dl(episodes);
		}
		for (const Website::episodepair &epair : episodes)
		{
			std::cout << epair.first << " - " << epair.second << std::endl;
		}
	}

	return 0;
}
