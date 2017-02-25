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

#include "config.hpp"
#include "configfile.hpp"
#include "global.hpp"
#include <string>
#include <getopt.h>
#include <cstdlib>	// exit()
#include <unistd.h>	// sleep()
#include <cstring>	// strncmp()
#include <iostream>
#include <regex>
#include <limits>	// std::numeric_limits
#include <algorithm>

Config::Config(const int &argc, const char *argv[])
:	_providers_ssl({ Streamcloud, Vivo, Shared, YouTube, OpenLoad, OpenLoadHD }),
	_providers_nossl({ PowerWatch, CloudTime, AuroraVid, Vidto, VoDLocker }),
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
		{Vidto, providerpair("Vidto", "vidto.me")},
		{VoDLocker, providerpair("VoDLocker", "vodlocker.com")}
	}),
	_yt_dl_path("youtube-dl"),
	_useragent(""),
	_episode_range{{0, std::numeric_limits<uint16_t>::max()}},
	_season_range{{0, 0}},
	_use_current_episode(0b00),
	_playlist(PL_RAW),
	_direct_url(false),
	_url(""),
	_resolve(false),
	_delay(0),
	_use_tor(false),
	_tor_address("127.0.0.1:9050"),
	_tor_controlport(9051),
	_tor_password("")
{
	Global::debug("config.cpp");
	ConfigFile configfile("seriespl");
	if (configfile.read())
	{
		Global::debug("Reading config file...");
		try
		{
			if (configfile.get_value("streamproviders") != "")	// Before 2.0.0
				populate_providers(configfile.get_value("streamproviders"));
			if (configfile.get_value("hostingproviders") != "")
				populate_providers(configfile.get_value("hostingproviders"));
			if (configfile.get_value("youtube-dl") != "")
				_yt_dl_path = configfile.get_value("youtube-dl");
			if (configfile.get_value("user-agent") != "")
				_useragent = configfile.get_value("user-agent");
			if (configfile.get_value("resolve_delay") != "")	// Before 2.3.2
			{
				_delay = std::stoi(configfile.get_value("resolve_delay"));
				if (_delay != std::stoi(configfile.get_value("resolve_delay")))
				{
					throw std::out_of_range("Delay out of range");
				}
			}
			if (configfile.get_value("delay") != "")
			{
				_delay = std::stoi(configfile.get_value("delay"));
				if (_delay != std::stoi(configfile.get_value("delay")))
				{
					throw std::out_of_range("Delay out of range");
				}
			}
			if (configfile.get_value("resolve_tries") != "")
			{
				_resolve_tries = std::stoi(configfile.get_value("resolve_tries"));
				if (_resolve_tries != std::stoi(configfile.get_value("resolve_tries")))
				{
					throw std::out_of_range("resolve_tries out of range");
				}
			}
			if (configfile.get_value("tor_address") != "")
				_tor_address = configfile.get_value("tor_address");
			if (configfile.get_value("tor_controlport") != "")
				_tor_controlport = std::stoi(configfile.get_value("tor_controlport"));
			if (configfile.get_value("tor_password") != "")
				_tor_password = configfile.get_value("tor_password");
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error in config file: " << e.what() << '\n'
				<< "Consult the manpage for more information.\n";
			exit(1);
		}
	}

	handle_args(argc, argv);
}

const Config::Websites &Config::get_website() const
{
	return _website;
}

const std::vector<Config::HostingProviders> &Config::get_providers_ssl() const
{
	return _providers_ssl;
}

const std::vector<Config::HostingProviders> &Config::get_providers_nossl() const
{
	return _providers_nossl;
}

const std::vector<Config::HostingProviders> &Config::get_providers() const
{
	return _providers;
}

const std::map<const Config::HostingProviders, const Config::providerpair>
	&Config::get_providermap() const
{
	return _providermap;
}

const std::string &Config::get_yt_dl_path() const
{
	return _yt_dl_path;
}

const std::string &Config::get_useragent() const
{
	return _useragent;
}

const std::array<uint16_t, 2> &Config::get_episode_range() const
{
	return _episode_range;
}

const std::array<uint16_t, 2> &Config::get_season_range() const
{
	return _season_range;
}

const uint8_t &Config::get_use_current_episode() const
{
	return _use_current_episode;
}

const Config::PlaylistFormat &Config::get_playlist() const
{
	return _playlist;
}

const bool &Config::get_direct_url() const
{
	return _direct_url;
}

const std::string &Config::get_url() const
{
	return _url;
}

const bool &Config::get_resolve() const
{
	return _resolve;
}

const uint8_t &Config::get_resolve_tries() const
{
	return _resolve_tries;
}

const uint16_t &Config::get_delay() const
{
	return _delay;
}

const bool &Config::get_use_tor() const
{
	return _use_tor;
}
const std::string &Config::get_tor_address() const
{
	return _tor_address;
}

const uint16_t &Config::get_tor_controlport() const
{
	return _tor_controlport;
}

const std::string &Config::get_tor_password() const
{
	return _tor_password;
}

void Config::handle_args(const int &argc, const char *argv[])
{
	int opt;
	std::string usage = std::string("usage: ") + argv[0] +
		" [-h]|[-V] [-i]|[-p list] [-e episodes] [-s seasons] [-y] URL";

	Global::debug("Parsing arguments...");
	while ((opt = getopt(argc, (char **)argv, "hp:ie:s:f:yVa:rtd:")) != -1)
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
				"  -d                   Delay in seconds between connection attempts\n"
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
				std::cout << "seriespl " << Global::version << "\n"
						  << "Copyright © 2016-2017 tastytea <tastytea@tastytea.de>.\n"
						  << "License GPLv2: GNU GPL version 2 <https://www.gnu.org/licenses/gpl-2.0.html>.\n"
						  << "This is free software: you are free to change and redistribute it.\n"
						  << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
				exit(0);
				break;
			case 'a':	// User-Agent
				_useragent = optarg;
				break;
			case 'r':	// Resolve
				_resolve = true;
				break;
			case 't':	// Tor
				std::cerr << "WARNING: Inbuilt Tor support is deprecated and will be removed in the future. "
					"Please use the socks_proxy environment variable instead.\n";
				_use_tor = true;
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
					std::cerr << "Invalid argument: delay" << '\n';
					exit(1);
				}
				catch (const std::out_of_range &e)
				{
					std::cerr << "Out of range: " << e.what() << '\n';
					std::cerr << "Seconds have to be between 0 and 65535.\n";
					exit(1);
				}
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
			_website = Config::Websites::BurningSeries;
		}
		else if (_url.find("geektv.ma") != std::string::npos)
		{
			_website = Config::Websites::GeekTV;
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
{ // providerlist is a comma separated list
	Global::debug("Compiling list of hosting providers: <" + providerlist + ">...");

	std::regex reConfig("([[:alnum:]]+)");
	std::sregex_iterator it_re(providerlist.begin(), providerlist.end(), reConfig);
	std::sregex_iterator it_re_end;

	_providers.clear();
	while (it_re != it_re_end)
	{ // Slice providerlist into providers, iterate through them
		std::map<const HostingProviders, const providerpair>::const_iterator it;
		for (it = _providermap.begin(); it != _providermap.end(); ++it)
		{ // Lookup suggested provider in list of supported providers, add to _providers if found
			std::string supported = it->second.first;	// Name of supported provider
			std::string suggested = (*it_re)[1];		// Name of suggested provider
			// Transform providers to lowercase, for comparing
			// FIXME: Works only with ASCII (I think)
			std::transform(supported.begin(), supported.end(), supported.begin(), ::tolower);
			std::transform(suggested.begin(), suggested.end(), suggested.begin(), ::tolower);
			if (supported == suggested)
				_providers.push_back(it->first);
		}
		++it_re;
	}
	if (_providers.empty())
	{
		std::cerr << "Error: List of hosting providers is empty." << std::endl;
		exit(1);
	}
}
