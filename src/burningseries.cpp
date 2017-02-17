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

#include "burningseries.hpp"
#include "website.hpp"
#include "config.hpp"
#include "global.hpp"
#include <string>
#include <vector>
#include <map>
#include <array>
#include <sys/types.h>
#include <regex>
#include <iostream>

const uint8_t Burningseries::getlinks(std::vector<Global::episodepair> &episodes)
{
	Global::debug("burningseries.cpp");
	std::vector<Global::episodepair> pages;
	uint8_t ret;
	const std::map<const Config::HostingProviders, const Config::providerpair>
		&providermap = _cfg.get_providermap();

	ret = get_episode_pages(pages);
	if (ret != 0)
	{
		return ret;
	}

	for (const Global::episodepair &epair : pages)
	{
		std::map<const Config::HostingProviders, const Config::providerpair>::const_iterator itm;
		for (itm = providermap.begin(); itm != providermap.end(); ++itm)
		{
			if (itm->second.first == epair.second)
			{	// Look up provider of found link in providermap
				const Config::HostingProviders provider = itm->first;
				std::string content = getpage(epair.first);
				std::regex reHosterPage(
					"(<a href| src)=[\"\'](https?://bs.to/out/.*)[\"\']( target=|></iframe>)");
				std::regex reTitle(std::string("<h2 id=\"titleGerman\">(.*)") +
					"[[:space:]]+<small id=\"titleEnglish\" lang=\"en\">(.*)</small>");
				std::smatch match;
				std::string hosterurl;
				std::string title;

				if (std::regex_search(content, match, reHosterPage))
				{
					hosterurl = match[2].str();
					if (_cfg.get_resolve())
					{
						resolve_redirect(hosterurl);
					}

					for (const Config::HostingProviders &provider_ssl : _cfg.get_providers_ssl())
					{ // Make sure we use SSL where supported
						if (provider_ssl == provider)
						{
							if (hosterurl.find("https") == std::string::npos)
							{ // Replace "http" with "https"
								hosterurl = "https" + hosterurl.substr(4, std::string::npos);
							}
						}
					}
				}
				else
				{
					std::cerr << "Error extracting stream" << std::endl;
				}

				if (std::regex_search(content, match, reTitle))
				{
					if (match[1].str() != "")		// German
						title = match[1].str();
					else if (match[2].str() != "")	// English
						title = match[2].str();
				}
				episodes.push_back(Global::episodepair(hosterurl, title));
			}
		}
	}

	return 0;
}

const uint8_t Burningseries::get_episode_pages(std::vector<Global::episodepair> &pages)
{
	std::string provider_re = "(";
	std::vector<Config::HostingProviders>::const_iterator it;
	const std::vector<Config::HostingProviders> &providers = _cfg.get_providers();
	const std::map<const Config::HostingProviders, const Config::providerpair>
		&providermap = _cfg.get_providermap();
	const std::array<uint16_t, 2> &seasonrange = _cfg.get_season_range();
	std::array<uint16_t, 2> episoderange = _cfg.get_episode_range();
	std::string url = _cfg.get_url();
	std::string content;
	const uint8_t &use_current_episode = _cfg.get_use_current_episode();

	for (it = providers.begin(); it != providers.end(); ++it)
	{ // Build regular expression for all supported streaming providers
		if (it != providers.begin())
		{ // Add | unless it is the first match
			provider_re += "|";
		}
		provider_re += providermap.at(*it).first;
	}
	provider_re += ")";

	for (uint16_t season = seasonrange[0]; season <= seasonrange[1]; ++season)
	{
		if (season != 0)
		{ // A season range was selected
			// NOTE: If season is higher than available seasons, season 1 is returned
			std::regex reSeries("(https://bs.to/serie/[^/]*/).*");
			std::smatch match;
		
			// Generate URL for each season
			if (std::regex_search(url, match, reSeries))
			{
				url = match[1].str() + std::to_string(season);
			}
			else
			{
				url = url + "/" + std::to_string(season);
			}
			content = getpage(url);
		}
		else
		{ // If no season range was selected, use supplied URL
			content = getpage(url);
		}

		if (use_current_episode != 0b00)
		{ // replace 'c's with episode numbers
			std::regex reEpisode("https://bs.to/serie/[^/]*/[[:digit:]]+/([[:digit:]]+)-.*");
			std::smatch match;

			if (std::regex_search(url, match, reEpisode))
			{
				if ((use_current_episode & 0b01) != 0)
				{ // Start from current episode
					episoderange[0] = std::stoi(match[1].str());
				}
				if ((use_current_episode & 0b10) != 0)
				{ // End with current episode
					episoderange[1] = std::stoi(match[1].str());
				}
			}
			else
			{
				std::cerr << "Error: Could not extract current episode." << std::endl;
				return 3;
			}
		}

		std::regex reEpisodePage("href=\"(serie/.*/[[:digit:]]+/([[:digit:]]+)-.*/(" +
			provider_re + "))\">(" + provider_re + ")?</a>");
		std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_re_end;

		if (it_re == it_re_end)
		{ // No matches
			std::cerr << "Error: No episodes found" << std::endl;
			return 3;
		}
		uint16_t episode = 1;
		while (it_re != it_re_end)
		{ // 1 == link, 2 == episode, 3 == provider
			uint16_t found_episode = std::stoi((*it_re)[2]);
			if (episode == 1 && episoderange[0] > 1)
			{ // Set episode to the right starting value if episode range is specified
				episode = episoderange[0];
			}

			if (found_episode >= episoderange[0] &&
				found_episode <= episoderange[1] &&
				found_episode >= episode)
			{
				if (found_episode > episode)
				{ // If found episode is > expected episode, we are missing 1 ore more episode(s)
					std::cerr << "Error: Could not get URL for episode " << episode;
					if ((episode + 1) != found_episode)
					{ // If more than 1 episode is missing
						std::cerr << "-" << (found_episode - 1);
					}
					std::cerr << "." << std::endl;
				}

				// Put URL and provider to episode pages into &pages
				pages.push_back(Global::episodepair("https://bs.to/" + (*it_re)[1].str(),
					(*it_re)[3].str()));

				episode = found_episode + 1;
			}
			else if (found_episode > episoderange[1])
			{
				break;
			}
			++it_re;
		} // Iterating through matches
	}

	return 0;
}
