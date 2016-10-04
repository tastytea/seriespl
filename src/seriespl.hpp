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

#include <utility>	// std::pair
#include <string>
#include <vector>
#include <map>
#include <limits>	// std::numeric_limits
#include "config.hpp"

class Seriespl
{
public:
	const std::string version = "1.4.7";

	Seriespl(int argc, char const *argv[]);
	~Seriespl();
	int run();

private:
	typedef const std::pair <std::string, std::string> providerpair; // Name and domain
	enum Services
	{ // Services who provide links to entire seasons
		BurningSeries
	};
	Services service;
	enum StreamProviders
	{ // Available stream providers
		Streamcloud,
		Vivo,
		Shared,
		YouTube,
		OpenLoad,
		PowerWatch,
		CloudTime,
		AuroraVid,
		Vidto
	};
	std::vector<StreamProviders> Providers;	// List of active stream providers
	// Map stream providers to string and URL
	const std::map<StreamProviders, providerpair> providermap;
	enum PlaylistFormat
	{
		PL_RAW,
		PL_M3U,
		PL_PLS
	};
	std::string yt_dl_path = "youtube-dl";	// Path to youtube-dl
	std::string directoryurl = "";	// URL for the overview-page of a series,
									// e.g. https://bs.to/serie/Die-Simpsons/1
	Config::cfgmap config;
	unsigned short startEpisode = 0, endEpisode = std::numeric_limits<unsigned short>::max();
	short startSeason = -1, endSeason = -1;
	uint8_t current_episode = 0; // 0 = no, 1 = c-, 2 = -c, 3 = c
	bool direct_url = false;
	PlaylistFormat playlist = PL_RAW;

	int handle_args(int argc, char const *argv[]);
	void set_service();
	void populate_providers(const std::string &providerlist);
	std::string getlink(const std::string &url, const StreamProviders &provider,
						std::string &title);
	std::string getlink(const std::string &url, const StreamProviders &provider);
	void print_playlist(const PlaylistFormat &playlist, const std::string &url,
						const std::string &title);
	void print_playlist(const PlaylistFormat &playlist, const std::string &url);
	std::string get_direct_url(std::string &providerurl);
};
