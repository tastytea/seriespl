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
 *	with seriespl.  If not, see <https://www.gnu.org/licenses/gpl-2.0.html>.
 *
 ******************************************************************************/


#include "seriespl.hpp"
#include <iostream>

void Seriespl::print_playlist(const PlaylistFormat &playlist, const std::string &url,
							  const std::string &title)
{
	static unsigned short counter = 1;
	std::string newtitle = title;
	size_t pos = 0;
	std::string newurl;

	if (direct_url)
	{
		newurl = get_direct_url(url);
	}
	else
	{
		newurl = url;
	}

	switch (playlist)
	{
		case PL_RAW:
			std::cout << newurl << std::endl;
			break;
		case PL_M3U:
			if (counter == 1) // Write header
			{
				std::cout << "#EXTM3U" << std::endl;
			}
			// Replacing comma with U+201A, SINGLE LOW-9 QUOTATION MARK
			// Because VLC uses commas in titles as separator for metadata
			while ((pos = title.find(',', pos + 1)) != std::string::npos)
			{
				newtitle.replace(pos, 1, "‚");	// The ‚ is not a comma
			}
			std::cout << "#EXTINF:-1," << newtitle << std::endl;
			std::cout << newurl << std::endl;
			break;
		case PL_PLS:
			if (counter == 1) // Write header
			{
				std::cout << "[playlist]" << std::endl;
				std::cout << "Version=2" << std::endl;
			}
			if (newurl == "NUMBER_OF_EPISODES")
			{
				std::cout << "NumberOfEntries=" << counter - 1 << std::endl;
			}
			else
			{
				std::cout << "File" << counter << "=" << newurl << std::endl;
				std::cout << "Title" << counter << "=" << title << std::endl;
				std::cout << "Length" << counter << "=-1" << std::endl;
			}
			break;
	}
	++counter;
}

void Seriespl::print_playlist(const PlaylistFormat &playlist, const std::string &url)
{
	return print_playlist(playlist, url, "");
}
