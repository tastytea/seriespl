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

#include "playlist.hpp"
#include "config.hpp"
#include <vector>
#include <sys/types.h>
#include <iostream>

Playlist::Playlist(const Config &cfg)
:	_cfg(cfg)
{}

const uint8_t Playlist::print(const std::vector<episodepair> &episodes) const
{
	uint16_t counter = 1;

	for (const episodepair &epair : episodes)
	{
		switch (_cfg.get_playlist())
		{
			case Config::PL_RAW:
				std::cout << epair.first << std::endl;
				break;
			case Config::PL_M3U:
				if (counter == 1) // Write header
				{
					std::cout << "#EXTM3U\n";
				}
				std::cout << "#EXTINF:-1," << replace_chars(epair.second) << std::endl;
				std::cout << epair.first << std::endl;
				break;
			case Config::PL_PLS:
				if (counter == 1) // Write header
				{
					std::cout << "[playlist]\n";
					std::cout << "Version=2\n";
				}
				std::cout << "File" << counter << "=" << epair.first << std::endl;
				std::cout << "Title" << counter << "=" << epair.second << std::endl;
				std::cout << "Length" << counter << "=-1" << std::endl;
				break;
		}
		++counter;
	}
	
	if (_cfg.get_playlist() == Config::PL_PLS)
	{
		std::cout << "NumberOfEntries=" << episodes.size() << std::endl;
	}

	return 0;
}

const std::string Playlist::replace_chars(const std::string &title) const
{
	size_t pos = 0;
	std::string newtitle = title;

	// Replacing comma with U+201A, SINGLE LOW-9 QUOTATION MARK
	// Because VLC uses commas in titles as separator for metadata
	while ((pos = newtitle.find(',', pos + 1)) != std::string::npos)
	{
		newtitle.replace(pos, 1, "‚");	// The ‚ is not a comma
	}

	return newtitle;
}
