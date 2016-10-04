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

#include <memory>
#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <vector>
#include <sstream>
#include <unistd.h>	// sleep()
#include <map>
#include <utility>
#include <cstdio>

#include "seriespl.hpp"
#include "http.hpp"
#include "config.hpp"

Seriespl::Seriespl()
	// Set default list of active streaming providers, SSL only
:	Providers({ Streamcloud, Vivo, Shared, YouTube, OpenLoad }),
	providermap
	({
		{Streamcloud, providerpair("Streamcloud", "streamcloud.eu")},
		{Vivo, providerpair("Vivo", "vivo.sx")},
		{Shared, providerpair("Shared", "shared.sx")},
		{YouTube, providerpair("YouTube", "www.youtube.com")},
		{OpenLoad, providerpair("OpenLoad", "openload.co")},
		{PowerWatch, providerpair("PowerWatch", "powerwatch.pw")},
		{CloudTime, providerpair("CloudTime", "www.cloudtime.to")},
		{AuroraVid, providerpair("AuroraVid", "auroravid.to")},
		{Vidto, providerpair("Vidto", "vidto.me")}
	})
{
	// read config and set streaming providers, if specified
	if (Config::read(config))
	{
		if (config["streamproviders"] != "")
			populate_providers(config["streamproviders"]);
		if (config["youtube-dl"] != "")
			yt_dl_path = config["youtube-dl"];
	}
}

Seriespl::~Seriespl() {}

int Seriespl::run(int argc, char const *argv[])
{
	int ret = handle_args(argc, argv);
	std::string content;

	if (ret > 0)
	{
		return ret;
	}
	else if (ret < 0)
	{
		return 0;
	}

	set_service();

	if (service == BurningSeries)
	{
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
				content = GetHTTP::getpage(directoryurl);
			}
			else
			{ // If no season range was selected, use supplied URL
				content = GetHTTP::getpage(directoryurl);
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
							if (direct_url)
							{
								print_playlist(playlist, get_direct_url(streamurl), streamtitle);
							}
							else
							{
								print_playlist(playlist, streamurl, streamtitle);
							}

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
	}

	return 0;
}

void Seriespl::set_service()
{
	if (directoryurl.find("bs.to") != std::string::npos)
	{
		service = BurningSeries;
	}
	else
	{
		std::cerr << "Error: Could not determine which website you specified, " <<
			"defaulting to Burning-Series." << std::endl;
		sleep(2);
		service = BurningSeries;
	}
}

void Seriespl::populate_providers(const std::string &providerlist)
{
	std::istringstream ss;

	ss.str(providerlist);
	Providers.clear();

	std::regex reConfig("([[:alnum:]]+)");
	std::sregex_iterator it_re(providerlist.begin(), providerlist.end(), reConfig);
	std::sregex_iterator it_re_end;

	while (it_re != it_re_end)
	{
		std::map<StreamProviders, providerpair>::const_iterator it;
		for (it = providermap.begin(); it != providermap.end(); ++it)
		{
			if (it->second.first == (*it_re)[1])
				Providers.push_back(it->first);
		}
		++it_re;
	}
	if (Providers.empty())
	{
		std::cerr << "Error: List of streaming providers is empty." << std::endl;
		exit(2);
	}
}

std::string Seriespl::getlink(const std::string &url, const StreamProviders &provider,
							  std::string &title)
{ // Takes URL of episode-page and streaming provider, returns URL of stream-page or "" on error
	std::string content = GetHTTP::getpage(url);
	std::string streamurl = "";

	if (service == BurningSeries)
	{
		std::regex reStreamPage("(<a href| src)=[\"\'](https?://" +
			providermap.at(provider).second + "/.*)[\"\']( target=|></iframe>)");
		std::regex reTitle(
			std::string("<h2 id=\"titleGerman\">(.*)") +
				"[[:space:]]+<small id=\"titleEnglish\" lang=\"en\">(.*)</small>");
		std::smatch match;

		if (std::regex_search(content, match, reStreamPage))
		{
			streamurl = match[2].str();
			
			if (provider == Streamcloud ||
				provider == Vivo ||
				provider == Shared ||
				provider == YouTube)
			{ // Make sure we use SSL where supported
				if (streamurl.find("https") == std::string::npos)
				{
					streamurl = "https" + streamurl.substr(4, std::string::npos);
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
	} // service-if

	return streamurl;
}

std::string Seriespl::getlink(const std::string &url, const StreamProviders &provider)
{
	std::string title;
	return getlink(url, provider, title);
}

std::string Seriespl::get_direct_url(std::string &providerurl)
{ // Use youtube-dl to print the direct URL of the video file
	FILE *ytdl;
	char buffer[256];
	std::string result;

	if(!(ytdl = popen(( yt_dl_path + " --get-url " + providerurl).c_str(), "r")))
	{
		std::cerr << "Error: Can not spawn process for youtube-dl" << std::endl;
		return "";
	}

	while(fgets(buffer, sizeof(buffer), ytdl) != NULL)
	{
		result += buffer;
	}
	if (pclose(ytdl) != 0)
	{
		std::cerr << "Error: youtube-dl returned non-zero exit code" << std::endl;
		return "";
	}

	return result.substr(0, result.find_last_not_of("\r\n") + 1);
}
