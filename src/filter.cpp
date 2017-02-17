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

#include "filter.hpp"
#include "config.hpp"
#include "global.hpp"
#include <string>
#include <vector>
#include <sys/types.h>
#include <cstdio>		// popen()
#include <sys/wait.h>	// WEXITSTATUS()
#include <iostream>

Filter::Filter(const Config &cfg)
:	_cfg(cfg)
{
	Global::debug("filter.cpp");
}

const uint8_t Filter::youtube_dl(std::vector<Global::episodepair> &episodes)
{
	Global::debug("Calling youtube-dl...");
	for (Global::episodepair &epair : episodes)
	{
		FILE *ytdl;
		char buffer[256];
		std::string result;
		int status = 0;

		ytdl = popen((_cfg.get_yt_dl_path() + " --get-url " + epair.first).c_str(), "r");
		if (ytdl == NULL)
		{
			std::cerr << "Error: Can not spawn process for youtube-dl" << std::endl;
			return 4;
		}

		while(fgets(buffer, sizeof(buffer), ytdl) != NULL)
		{
			result += buffer;
		}
		status = pclose(ytdl);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			std::cerr << "Error: youtube-dl returned non-zero exit code ("
				<< WEXITSTATUS(status) << "), leaving URL untouched." << std::endl;
		}	// If youtube-dl didn't work, leave url untouched
		else
		{
			epair.first = result.substr(0, result.find_last_not_of("\r\n") + 1);
		}
	}

	return 0;
}

