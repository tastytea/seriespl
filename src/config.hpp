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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <sys/types.h>	// uint8_t
#include <array>
#include <vector>
#include <map>

class Config
{
public:
	static constexpr char version[] = "2.0.0";
	enum PlaylistFormat
	{
		PL_RAW,
		PL_M3U,
		PL_PLS
	};
	enum Websites
	{ // Indexing services
		BurningSeries
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
		Vidto
	};

	Config(const int &argc, const char *argv[]);

private:
	// Name, URL-regex
	typedef const std::pair <const std::string, const std::string> providerpair;
	Websites _website;
	const std::vector<HostingProviders> _providers_ssl;
	const std::vector<HostingProviders> _providers_nossl;
	std::vector<HostingProviders> _providers;
	// Map stream providers to string and URL
	const std::map<const HostingProviders, const providerpair> _providermap;
	std::string _yt_dl_path;		// Path to youtube-dl
	std::string _useragent;
	uint16_t _episode_range[2];		// from, to
	uint16_t _season_range[2];		// from, to
	uint8_t _use_current_episode;	// 0 = no, 1 = c-, 2 = -c, 3 = c
	PlaylistFormat _playlist;		// Format of playlist
	bool _direct_url;				// filter through youtube-dl
	std::string _url;				// User supplied URL

	void handle_args(const int &argc, const char *argv[]);
	void populate_providers(const std::string &providerlist);
};

#endif
