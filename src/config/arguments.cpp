// Handle command line arguments, declared in config.hpp
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

#include <string>
#include <iostream>
#include <cstdlib>	// exit()
#include <cstring>	// strncmp()
#include <getopt.h>
#include <unistd.h>		// sleep()

#include "config.hpp"
#include "../global.hpp"

void Config::handle_args(const int &argc, const char *argv[])
{
	int opt;
	const std::string usage = std::string("usage: ") + argv[0] +
		" [-h]|[-V] [-i]|[-p list] [-e episodes] [-s seasons] [-y] URL";

	ttdebug << "Parsing arguments...\n";
	while ((opt = getopt(argc, (char **)argv, "hp:ie:s:f:yVa:rtd:A")) != -1)
	{
		std::string episodes, seasons;
		size_t pos;

		switch (opt)
		{
			case 'h':	// Help
				std::cout << usage << "\n\n"
				"  -h                   Show this help and exit\n"
				"  -p hosting providers Comma delimited list. Available:\n"
				"                       Streamcloud,Vivo,Shared,YouTube,OpenLoad,OpenLoadHD,"
				                        "PowerWatch,CloudTime,AuroraVid,Vidto,VoDLocker\n"
				"  -i                   Use hosting providers without SSL support too\n"
				"  -e episodes          Episode range, e.g. 2-5 or 7 or 9-, use c for current\n"
				"  -s episodes          Season range, e.g. 1-2 or 4\n"
				"  -f format            Playlist format. Available: raw, m3u, pls\n"
				"  -y                   Use youtube-dl to print the direct URL of the video file\n"
				"  -a user-agent        Set User-Agent\n"
				"  -r                   Resolve redirections\n"
				"  -d delay             Delay in seconds between connection attempts\n"
				"  -A                   Abort on error\n"
				"  -V                   Output version and copyright information and exit"
				<< std::endl;
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
						{ // if c-
							_use_current_episode |= 0b01;
						}
						else
						{
							_episode_range[0] = std::stoi(episodes.substr(0, pos));
						}
						if (episodes.length() > pos + 1)
						{ // If episodes = 5-, output all episodes, beginning with 5
							if (episodes.substr(pos + 1) == "c")
							{ // If -c
								_use_current_episode |= 0b10;
							}
							else
							{
								_episode_range[1] = std::stoi(episodes.substr(pos + 1));
							}
						}
					}
					catch (const std::exception &e)
					{ // There is a '-' but no numbers around it
						ttdebug << "Error parsing -e: " << e.what() << '\n';
						tterror << "Can not decipher episode range\n";
						if (_errors_fatal)
							exit(global::Error::ParseError);
						tterror << "Defaulting to all\n";
						sleep(2);
					}
				}
				else
				{
					try
					{ 
						if (episodes == "c")
						{
							_use_current_episode = 0b11;
						}
						else
						{ // episodes is a single number
							_episode_range[0] = std::stoi(episodes);
							_episode_range[1] = std::stoi(episodes);
						}
					}
					catch (const std::exception &e)
					{
						ttdebug << "Error parsing -e: " << e.what() << '\n';
						tterror << "Can not decipher episode range\n";
						if (_errors_fatal)
							exit(global::Error::ParseError);
						tterror << "Defaulting to all\n";
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
						_season_range[0] = std::stoi(seasons.substr(0, pos));
						_season_range[1] = std::stoi(seasons.substr(pos + 1));
					}
					catch (const std::exception &e)
					{ // There is a '-' but no numbers around it
						ttdebug << "Error parsing -s: " << e.what() << '\n';
						tterror << "Can not decipher season range\n";
						if (_errors_fatal)
							exit(global::Error::ParseError);
						tterror << "Defaulting to supplied\n";
						_season_range[0] = 0;
						_season_range[1] = 0;
						sleep(2);
					}
				}
				else
				{
					try
					{ // seasons is a single number
						_season_range[0] = std::stoi(seasons);
						_season_range[1] = std::stoi(seasons);
					}
					catch (const std::exception &e)
					{
						ttdebug << "Error parsing -s: " << e.what() << '\n';
						tterror << "Can not decipher season range\n";
						if (_errors_fatal)
							exit(global::Error::ParseError);
						tterror << "Defaulting to supplied\n";
					}
				}
				break;
			case 'f':	// Format
				if (strncmp(optarg, "raw", 3) == 0)
					_playlist = PlaylistFormat::PL_RAW;
				else if (strncmp(optarg, "m3u", 3) == 0)
					_playlist = PlaylistFormat::PL_M3U;
				else if (strncmp(optarg, "pls", 3) == 0)
					_playlist = PlaylistFormat::PL_PLS;
				else
					tterror << "Playlist format not recognized\n";
					if (_errors_fatal)
						exit(global::Error::ParseError);
					tterror << "Defaulting to raw\n";
					sleep(2);
				break;
			case 'y':	// youtube-dl
				_direct_url = true;
				break;
			case 'V':	// Version
				std::cout <<
					"seriespl " << global::version << " (git commit: " << global::git_commit << ")\n"
					"Copyright © 2016-2017 tastytea <tastytea@tastytea.de>.\n"
					"License GPLv2: GNU GPL version 2 <https://www.gnu.org/licenses/gpl-2.0.html>.\n"
					"This is free software: you are free to change and redistribute it.\n"
					"There is NO WARRANTY, to the extent permitted by law.\n";
				exit(0);
				break;
			case 'a':	// User-Agent
				_useragent = optarg;
				break;
			case 'r':	// Resolve
				_resolve = true;
				break;
			case 't':	// Tor
				tterror << "Inbuilt Tor support is no longer available."
					"Please use the socks_proxy environment variable instead.\n";
				exit(global::Error::FeatureGone);
				break;
			case 'd':	// Delay
				try
				{
					_delay = std::stoi(optarg);
					if (_delay != std::stoi(optarg))
					{ // If out of range
						throw std::out_of_range("delay");
					}
				}
				catch (const std::invalid_argument &e)
				{
					tterror << "-d: Invalid argument" << '\n';
					exit(global::Error::InvalidArgument);
				}
				catch (const std::out_of_range &e)
				{
					ttdebug << "Out of range: " << e.what() << '\n';
					tterror << "-d: Seconds have to be between 0 and 65535\n";
					exit(global::Error::InvalidArgument);
				}
				break;
			case 'A':	// Abort
				_errors_fatal = true;
				break;
			default:
				std::cerr << usage << '\n';
				exit(global::Error::InvalidArgument);
				break;
		}
	}
	if (optind >= argc)
	{
		std::cerr << usage << '\n';
		exit(global::Error::InvalidArgument);
	}
	else
	{ // Set user supplied URL
		_url = argv[optind];
		if (_url.find("bs.to") != std::string::npos)
		{
			_website = Config::Websites::BurningSeries;
		}
		else if (_url.find("geektv.ma") != std::string::npos)
		{
			_website = Config::Websites::GeekTV;
		}
		else
		{
			tterror << "Could not determine which website you supplied\n";
			if (_errors_fatal)
				exit(global::Error::ParseError);
			tterror << "Defaulting to Burning-Series.\n";
			sleep(2);
			_website = Websites::BurningSeries;
		}
	}
}
