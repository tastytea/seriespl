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

#include "config.hpp"
#include "global.hpp"
#include "burningseries.hpp"
#include "filter.hpp"
#include "playlist.hpp"
#include <iostream>
#include <vector>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
	Config cfg(argc, argv);
	if (cfg.get_website() == Config::BurningSeries)
	{
		uint8_t ret;

		Burningseries website(cfg);
		std::vector<Global::episodepair> episodes;
		Filter filter(cfg);
		Playlist playlist(cfg);

		ret = website.getlinks(episodes);
		if (ret != 0)
			return ret;
		if (cfg.get_direct_url())
		{
			ret = filter.youtube_dl(episodes);
			if (ret != 0)
				return ret;
		}
		ret = playlist.print(episodes);
		if (ret != 0)
			return ret;
	}

	return 0;
}
