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
#include <cstdint>
#include <array>
#include <vector>
#include <map>
#include <utility>		// std::pair

class Config
{
public:
	enum class PlaylistFormat
	{
		PL_RAW,
		PL_M3U,
		PL_PLS
	};
	enum class Websites
	{ // Indexing services
		BurningSeries,
		GeekTV
	};
	enum class HostingProviders
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

	Config();
	void handle_args(const int &argc, const char *argv[]);

private:
	Websites _website;
	const std::vector<HostingProviders> _providers_ssl;
	const std::vector<HostingProviders> _providers_nossl;
	std::vector<HostingProviders> _providers;
	// Map hosting providers to string and URL
	const std::map<const HostingProviders, const providerpair> _providermap;
	std::string _yt_dl_path;		// Path to youtube-dl
	std::string _useragent;
	std::array<std::uint16_t, 2> _episode_range;	// from, to
	std::array<std::uint16_t, 2> _season_range;		// from, to
	std::uint8_t _use_current_episode;				// 0b00 = no, 0b01 = c-, 0b10 = -c, 0b11 = c
	PlaylistFormat _playlist;						// Format of playlist
	bool _direct_url;								// filter through youtube-dl
	std::string _url;								// User supplied URL
	bool _resolve;									// Resolve redirections
	std::uint16_t _delay;							// How much seconds to wait between connections
	bool _errors_fatal;

	void populate_providers(const std::string &providerlist);
};

#endif
