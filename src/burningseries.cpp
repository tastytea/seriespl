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
 *	You should have received a copy of the GNU General Public License
 *	along with seriespl. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <map>

#include "seriespl.hpp"


int Seriespl::burningseries()
{
	std::string content;
	std::string provider_re = "(";
	std::vector<StreamProviders>::iterator it;

	for (it = Providers.begin(); it != Providers.end(); ++it)
	{ // Build regular expression for all supported streaming providers
		if (it != Providers.begin())
		{ // Add | unless it is the first match
			provider_re += "|";
		}
		provider_re += providermap.at(*it).first;
	}
	provider_re += ")";

	for (short season = startSeason; season <= endSeason; ++season)
	{
		if (season != -1)
		{ // A season range was selected
			//FIXME: If season is higher than available seasons, season 1 is returned
			std::regex reSeries("(https://bs.to/serie/[^/]*/).*");
			std::smatch match;
		
			// Generate URL for each season
			if (std::regex_search(directoryurl, match, reSeries))
			{
				directoryurl = match[1].str() + std::to_string(season);
			}
			else
			{
				directoryurl = directoryurl + "/" + std::to_string(season);
			}
			content = getpage(directoryurl);
		}
		else
		{ // If no season range was selected, use supplied URL
			content = getpage(directoryurl);
		}

		if (current_episode != 0)
		{
			std::regex reEpisode("https://bs.to/serie/[^/]*/[[:digit:]]+/([[:digit:]]+)-.*");
			std::smatch match;

			if (std::regex_search(directoryurl, match, reEpisode))
			{
				if ((current_episode & 1) != 0)
				{
					startEpisode = std::stoi(match[1].str());
				}
				if ((current_episode & 2) != 0)
				{
					endEpisode = std::stoi(match[1].str());
				}
			}
			else
			{
				std::cerr << "Error: Could not extract current episode." << std::endl;
				return 3;
			}
		}

		std::regex reEpisodePage("href=\"(serie/.*/[[:digit:]]+/([[:digit:]]+)-.*/(" +
			provider_re + ")-[0-9])\">(" + provider_re + ")?</a>");
		std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_re_end;

		if (it_re == it_re_end)
		{ // No matches
			std::cerr << "Error: No episodes found" << std::endl;
			return 4;
		}
		while (it_re != it_re_end)
		{ // 1 == link, 2 == episode, 3 == provider
			static short episode = 0;
			if (episode == 0 && startEpisode > 0)
			{ // Set episode to the right starting value if episode range is specified
				episode = startEpisode - 1;
			}
			if (std::stoi((*it_re)[2]) >= startEpisode &&
				std::stoi((*it_re)[2]) <= endEpisode &&
				std::stoi((*it_re)[2]) != episode)
			{
				if ( stoi((*it_re)[2]) > (episode + 1) )
				{ // If current episode is > last episode + 1, we are missing an episode
					std::cerr << "Error: Could not get URL for episode " << (episode + 1);
					if ((episode + 2) != stoi((*it_re)[2]))
					{ // If more than 1 episode is missing
						std::cerr << "-" << (stoi((*it_re)[2]) - 1);
					}
					std::cerr << "." << std::endl;
				}
				std::string episodelink = "https://bs.to/" + (*it_re)[1].str();
				std::map<StreamProviders, providerpair>::const_iterator it;
				for (it = providermap.begin(); it != providermap.end(); ++it)
				{
					if (it->second.first == (*it_re)[3])
					{
						std::string streamtitle;
						std::string streamurl = getlink(episodelink, it->first, streamtitle);
						print_playlist(playlist, get_direct_url(streamurl), streamtitle);

					}
				}
				episode = std::stoi((*it_re)[2]);
			}
			++it_re;
		} // Iterating through matches
		if (playlist == PL_PLS)
		{ // Print NumberOfEntries
			print_playlist(PL_PLS, "NUMBER_OF_EPISODES");
		}
	}

	return 0;
}
