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
#include "configfile.hpp"
#include <string>
#include <getopt.h>
#include <cstdlib>	// exit()
#include <unistd.h>	// sleep()
#include <cstring>	// strncmp()
#include <iostream>
#include <regex>

constexpr char Config::version[];	// Defined in config.hpp

Config::Config(const int &argc, const char *argv[])
:	_providers_ssl({ Streamcloud, Vivo, Shared, YouTube, OpenLoad, OpenLoadHD }),
	_providers_nossl({ PowerWatch, CloudTime, AuroraVid, Vidto }),
	_providers(_providers_ssl),
	_providermap
	({
		{Streamcloud, providerpair("Streamcloud", "streamcloud.eu")},
		{Vivo, providerpair("Vivo", "vivo.sx")},
		{Shared, providerpair("Shared", "shared.sx")},
		{YouTube, providerpair("YouTube", "youtube.com")},
		{OpenLoad, providerpair("OpenLoad", "openload.co")},
		{OpenLoadHD, providerpair("OpenLoadHD", "openload.co")},
		{PowerWatch, providerpair("PowerWatch", "powerwatch.pw")},
		{CloudTime, providerpair("CloudTime", "cloudtime.to")},
		{AuroraVid, providerpair("AuroraVid", "auroravid.to")},
		{Vidto, providerpair("Vidto", "vidto.me")}
	}),
	_yt_dl_path("youtube-dl"),
	_useragent(""),
	_episode_range{0, 0},
	_season_range{0, 0},
	_use_current_episode(0),
	_playlist(PL_RAW),
	_direct_url(false),
	_url("")
{
	ConfigFile configfile("seriespl");
	if (configfile.read())
	{
		if (configfile.get_value("streamproviders") != "")	// Before 2.0.0
			populate_providers(configfile.get_value("streamproviders"));
		if (configfile.get_value("hostingproviders") != "")
			populate_providers(configfile.get_value("hostingproviders"));
		if (configfile.get_value("youtube-dl") != "")
			_yt_dl_path = configfile.get_value("youtube-dl");
		if (configfile.get_value("user-agent") != "")
			_useragent = configfile.get_value("user-agent");
	}

	handle_args(argc, argv);
}

void Config::handle_args(const int &argc, const char *argv[])
{
	int opt;
	std::string usage = std::string("usage: ") + argv[0] +
		" [-h] [-V] [-i]|[-p list] [-e episodes] [-s seasons] [-y] URL";
	
	while ((opt = getopt(argc, (char **)argv, "hp:ie:s:f:yVa:")) != -1)
	{
		std::string episodes, seasons;
		size_t pos;

		switch (opt)
		{
			case 'h':	// Help
				std::cout << usage << "\n\n"
				"  -h                   Show this help and exit\n"
				"  -p stream providers  Comma delimited list. Available:\n"
				"                       Streamcloud,Vivo,Shared,YouTube,OpenLoad,OpenLoadHD,"
				                        "PowerWatch,CloudTime,AuroraVid,Vidto\n"
				"  -i                   Use stream providers without SSL support too\n"
				"  -e episodes          Episode range, e.g. 2-5 or 7 or 9-, use c for current\n"
				"  -s episodes          Season range, e.g. 1-2 or 4\n"
				"  -f format            Playlist format. Available: raw, m3u, pls\n"
				"  -y                   Use youtube-dl to print the direct URL of the video file\n"
				"  -a user-agent        Set User-Agent\n"
				"  -V                   Output version and copyright information and exit" << std::endl;
				exit(0);
				break;
			case 'p':	// Provider
				populate_providers(std::string(optarg));
				break;
			case 'i':	// Insecure
				// Add Providers without SSL support
				_providers.insert(_providers.end(),
					_providers_nossl.begin(), _providers_nossl.end());
				break;
			case 'e':	// Episodes
				episodes = optarg;
				pos = episodes.find('-');
				if (pos != std::string::npos)
				{
					try
					{
						if (episodes.substr(0, pos) == "c")
						{
							_use_current_episode |= 1;
						}
						else
						{
							_episode_range[0] = std::stoi( episodes.substr(0, pos) );
						}
						if (episodes.length() > pos + 1)
						{ // If episodes = 5-, output all episodes, beginning with 5
							if (episodes.substr(pos + 1) == "c")
							{
								_use_current_episode |= 2;
							}
							else
							{
								_episode_range[1] = std::stoi( episodes.substr(pos + 1) );
							}
						}
					}
					catch (std::exception &e)
					{ // There is a '-' but no numbers around it
						std::cerr << "Error: Can not decipher episode range, " <<
							"defaulting to all." << std::endl;
						sleep(2);
					}
				}
				else
				{
					try
					{ 
						if (episodes == "c")
						{
							_use_current_episode = 3;
						}
						else
						{ // Is episodes a single number?
							_episode_range[0] = std::stoi(episodes);
							_episode_range[1] = std::stoi(episodes);
						}
					}
					catch (std::exception &e)
					{
						std::cerr << "Error: Can not decipher episode range, " <<
							"defaulting to all." << std::endl;
						sleep(2);
					}
				}
				break;
			case 's':	// Seasons
				seasons = optarg;
				pos = seasons.find('-');
				if (pos != std::string::npos)
				{
					try
					{
						_season_range[0] = std::stoi( seasons.substr(0, pos) );
						_season_range[1] = std::stoi( seasons.substr(pos + 1) );
					}
					catch (std::exception &e)
					{ // There is a '-' but no numbers around it
						std::cerr << "Error: Can not decipher season range, " <<
							"defaulting to selected." << std::endl;
						_season_range[0] = 0;
						_season_range[1] = 0;
						sleep(2);
					}
				}
				else
				{
					try
					{ // Is seasons a single number?
						_season_range[0] = std::stoi(seasons);
						_season_range[1] = std::stoi(seasons);
					}
					catch (std::exception &e)
					{
						std::cerr << "Error: Can not decipher season range, " <<
							"defaulting to selected." << std::endl;
						sleep(2);
					}
				}
				break;
			case 'f':	// Format
				if (strncmp(optarg, "raw", 3) == 0)
					_playlist = PL_RAW;
				else if (strncmp(optarg, "m3u", 3) == 0)
					_playlist = PL_M3U;
				else if (strncmp(optarg, "pls", 3) == 0)
					_playlist = PL_PLS;
				else
					std::cerr << "Playlist format not recognized, defaulting to raw." << std::endl;
				break;
			case 'y':	// youtube-dl
				_direct_url = true;
				break;
			case 'V':	// Version
				std::cout << "seriespl " << Config::version << "\n"
						  << "Copyright © 2016 tastytea <tastytea@tastytea.de>.\n"
						  << "License GPLv2: GNU GPL version 2 <https://www.gnu.org/licenses/gpl-2.0.html>.\n"
						  << "This is free software: you are free to change and redistribute it.\n"
						  << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
				exit(0);
				break;
			case 'a':	// User-Agent
				_useragent = optarg;
				break;
			default:
				std::cerr << usage << std::endl;
				exit(1);
				break;
		}
	}
	if (optind >= argc)
	{
		std::cerr << usage << std::endl;
		exit(1);
	}
	else
	{ // Set user supplied URL
		_url = argv[optind];
		if (_url.find("bs.to") != std::string::npos)
		{
			_website = BurningSeries;
		}
		else
		{
			std::cerr << "Error: Could not determine which website you supplied, " <<
				"defaulting to Burning-Series." << std::endl;
			sleep(2);
			_website = BurningSeries;
		}
	}
}

void Config::populate_providers(const std::string &providerlist)
{
	std::regex reConfig("([[:alnum:]]+)");
	std::sregex_iterator it_re(providerlist.begin(), providerlist.end(), reConfig);
	std::sregex_iterator it_re_end;

	_providers.clear();
	while (it_re != it_re_end)
	{ // Slice providerlist into alphanumeric chunks, iterate through slices
		std::map<HostingProviders, providerpair>::const_iterator it;
		for (it = _providermap.begin(); it != _providermap.end(); ++it)
		{ // Lookup slice in _providermap, add to _providers if found
			if (it->second.first == (*it_re)[1])	// First field of value (Name)
				_providers.push_back(it->first);
		}
		++it_re;
	}
	if (_providers.empty())
	{
		std::cerr << "Error: List of streaming providers is empty." << std::endl;
		exit(2);
	}
}
