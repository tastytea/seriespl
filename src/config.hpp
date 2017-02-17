// Read configuration file and parse command-line arguments
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <sys/types.h>	// uint8_t
#include <array>
#include <vector>
#include <map>
#include <utility>		// std::pair

class Config
{
public:
	enum PlaylistFormat
	{
		PL_RAW,
		PL_M3U,
		PL_PLS
	};
	enum Websites
	{ // Indexing services
		BurningSeries,
		GeekTV
	};
	enum HostingProviders
	{ // Available hosting providers
		Streamcloud,
		Vivo,
		Shared,
		YouTube,
		OpenLoad,
		OpenLoadHD,
		PowerWatch,
		CloudTime,
		AuroraVid,
		Vidto,
		VoDLocker
	};
	// Name, URL-regex
	typedef const std::pair <const std::string, const std::string> providerpair;

	Config(const int &argc, const char *argv[]);
	const Config::Websites &get_website() const;
	const std::vector<Config::HostingProviders> &get_providers_ssl() const;
	const std::vector<Config::HostingProviders> &get_providers_nossl() const;
	const std::vector<Config::HostingProviders> &get_providers() const;
	const std::map<const Config::HostingProviders, const Config::providerpair>
		&get_providermap() const;
	const std::string &get_yt_dl_path() const;
	const std::string &get_useragent() const;
	const std::array<uint16_t, 2> &get_episode_range() const;
	const std::array<uint16_t, 2> &get_season_range() const;
	const uint8_t &get_use_current_episode() const;
	const Config::PlaylistFormat &get_playlist() const;
	const bool &get_direct_url() const;
	const std::string &get_url() const;
	const bool &get_resolve() const;
	const uint8_t &get_resolve_tries() const;
	const uint16_t &get_delay() const;
	const bool &get_use_tor() const;
	const std::string &get_tor_address() const;
	const uint16_t &get_tor_controlport() const;
	const std::string &get_tor_password() const;

private:
	Websites _website;
	const std::vector<HostingProviders> _providers_ssl;
	const std::vector<HostingProviders> _providers_nossl;
	std::vector<HostingProviders> _providers;
	// Map stream providers to string and URL
	const std::map<const HostingProviders, const providerpair> _providermap;
	std::string _yt_dl_path;		// Path to youtube-dl
	std::string _useragent;
	std::array<uint16_t, 2> _episode_range;	// from, to
	std::array<uint16_t, 2> _season_range;	// from, to
	uint8_t _use_current_episode;			// 0b00 = no, 0b01 = c-, 0b10 = -c, 0b11 = c
	PlaylistFormat _playlist;				// Format of playlist
	bool _direct_url;						// filter through youtube-dl
	std::string _url;						// User supplied URL
	bool _resolve;							// Resolve redirections
	uint8_t _resolve_tries;					// How often to try resolving
	uint16_t _delay;						// How much seconds to wait between connection attempts
	bool _use_tor;							// Use Tor
	std::string _tor_address;				// Tor host:port
	uint16_t _tor_controlport;				// Tor ControlPort
	std::string _tor_password;				// Tor password

	void handle_args(const int &argc, const char *argv[]);
	void populate_providers(const std::string &providerlist);
};

#endif
