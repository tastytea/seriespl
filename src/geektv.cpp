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

#include "geektv.hpp"
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
#include <limits>	// std::numeric_limits

const uint8_t Geektv::getlinks(std::vector<Global::episodepair> &episodes)
{
	std::vector<std::string> pages;
	uint8_t ret = 0;
	const std::vector<Config::HostingProviders> &providers = _cfg.get_providers();
	const std::map<const Config::HostingProviders, const Config::providerpair>
		&providermap = _cfg.get_providermap();
	std::string provider_re = "(";

	ret = get_episode_pages(pages);
	if (ret != 0)
		return ret;

	for (const Config::HostingProviders &provider : providers)
	{	// Build regular expression for all supported streaming providers
		static bool first = true;
		if (first == false)
		{ // Add | unless it is the first match
			provider_re += "|";
		}
		else
		{
			first = false;
		}
		// Add domain
		provider_re += providermap.at(provider).second;
	}
	provider_re += ")";

	for (const std::string &page : pages)
	{
		// TODO: Error handling
		std::string content = getpage(page);
		std::string title;
		std::string url;
		std::smatch match;

		std::regex reTitle("<h4>Title Episode: (.+)</h4>");
		std::regex_search (content, match, reTitle);
		title = match[1].str();

		std::regex reVideoPage("<a href=\"(https?://geektv.ma/embed/" +
			provider_re + "/[[:digit:]]+)\"");
		std::regex_search (content, match, reVideoPage);
		if (match[1].str() != "")
		{
			content = getpage(to_https(match[1]));

			std::regex reProvider("<iframe src=\"(https?://" + provider_re + "/embed-.+\\.html)\"",
				std::regex_constants::icase);
			std::regex_search (content, match, reProvider);
			url = match[1].str();
		}
		else
		{
			url = "";
		}

		if (url != "")
		{
			episodes.push_back(Global::episodepair(to_https(url), title));
		}
		else
		{
			size_t pos = page.find("-e") + 2;
			std::cerr << "Error: Could not get URL for episode " <<
				page.substr(pos, page.size() - pos - 1) << ".\n";
		}
	}

	return 0;
}

const uint8_t Geektv::get_episode_pages(std::vector<std::string> &pages)
{
	std::string url = _cfg.get_url();
	const std::array<uint16_t, 2> &seasonrange = _cfg.get_season_range();
	std::string content = "";
	const uint8_t &use_current_episode = _cfg.get_use_current_episode();
	std::array<uint16_t, 2> episoderange = _cfg.get_episode_range();
	std::vector<std::string> pages_tmp;


	if (use_current_episode != 0b00)
	{	// replace 'c's with episode numbers
		std::regex reEpisode("https?://geektv.ma/episode/[^/]*-e([[:digit:]]+)/");
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

	// Fix URL if we are on an episode page
	std::regex reFixurl("https?://geektv.ma/episode(/[^/]+)-s([[:digit:]]+)-e[[:digit:]]+/?");
	std::smatch match;
	if (std::regex_search(url, match, reFixurl))
	{
		url = "https://geektv.ma/season-" + match[2].str() + match[1].str() + ".html";
	}

	for (uint16_t season = seasonrange[0]; season <= seasonrange[1]; ++season)
	{
		if (season != 0)
		{	// A season range was selected
			std::regex reSeries("https?://geektv.ma/[^/]+(/[^/]+)(\\.html|/?)");
			std::smatch match;

			// Generate URL for each season
			if (std::regex_search(url, match, reSeries))
			{
				url = "https://geektv.ma/season-" + std::to_string(season) +
					match[1].str() + ".html";
			}
			else
			{
				std::cerr << "Error: Could not generate URL for season " <<
					std::to_string(season) << ".\n";
				return 1;
			}
			content = getpage(url);
		}
		else
		{
			content = getpage(to_https(url));
		}

		// Get episode pages
		std::regex reEpisodePage(
			"<a href=\"https?://geektv.ma/episode/([^/]*-s[[:digit:]]+)-e([[:digit:]]+)/\">");
		std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_re_end;

		if (it_re == it_re_end)
		{ // No matches
			std::cerr << "Error: No episodes found" << std::endl;
			return 4;
		}
		uint16_t episode = std::numeric_limits<uint16_t>::max();
		while (it_re != it_re_end)
		{
			uint16_t found_episode = std::stoi((*it_re)[2]);
			if (episode == std::numeric_limits<uint16_t>::max() &&
				episoderange[1] != std::numeric_limits<uint16_t>::max())
			{ // Set episode to the right starting value if episode range is specified
				episode = episoderange[1];
			}
			if (found_episode >= episoderange[0] &&
				found_episode <= episoderange[1] &&
				found_episode <= episode)
			{
				if (found_episode < episode && episode != std::numeric_limits<uint16_t>::max())
				{	// If found episode is < expected episode, we are missing 1 ore more episode(s)
					// FIXME: Wrong order
					std::cerr << "Error: Could not get URL for episode " << episode;
					if ((episode - 1) != found_episode)
					{ // If more than 1 episode is missing
						std::cerr << "-" << (found_episode + 1);
					}
					std::cerr << "." << std::endl;
				}
				pages_tmp.push_back("https://geektv.ma/episode/" + (*it_re)[1].str() + "-e" +
				(*it_re)[2].str() + "/");
				episode = found_episode - 1;
			}
			++it_re;
		}
		std::vector<std::string>::const_reverse_iterator it;
		for (it = pages_tmp.rbegin(); it != pages_tmp.rend(); ++it)
		{
			pages.push_back(*it);
		}
		pages_tmp.clear();
	}

	return 0;
}
