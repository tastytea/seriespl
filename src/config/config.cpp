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
#include <cstdlib>		// exit()
#include <cstdint>
#include <regex>
#include <limits>		// std::numeric_limits
#include <algorithm>	// std::transform

#include "config.hpp"
#include "../global.hpp"

Config::Config()
: _website(Websites::BurningSeries)
, _providers_ssl({
	HostingProviders::Streamcloud,
	HostingProviders::Vivo,
	HostingProviders::Shared,
	HostingProviders::YouTube,
	HostingProviders::OpenLoad,
	HostingProviders::OpenLoadHD
})
, _providers_nossl({
	HostingProviders::PowerWatch,
	HostingProviders::CloudTime,
	HostingProviders::AuroraVid,
	HostingProviders::Vidto,
	HostingProviders::VoDLocker
})
, _providers(_providers_ssl)
, _providermap
({
	{HostingProviders::Streamcloud, providerpair("Streamcloud", "streamcloud.eu")},
	{HostingProviders::Vivo, providerpair("Vivo", "vivo.sx")},
	{HostingProviders::Shared, providerpair("Shared", "shared.sx")},
	{HostingProviders::YouTube, providerpair("YouTube", "youtube.com")},
	{HostingProviders::OpenLoad, providerpair("OpenLoad", "openload.co")},
	{HostingProviders::OpenLoadHD, providerpair("OpenLoadHD", "openload.co")},
	{HostingProviders::PowerWatch, providerpair("PowerWatch", "powerwatch.pw")},
	{HostingProviders::CloudTime, providerpair("CloudTime", "cloudtime.to")},
	{HostingProviders::AuroraVid, providerpair("AuroraVid", "auroravid.to")},
	{HostingProviders::Vidto, providerpair("Vidto", "vidto.me")},
	{HostingProviders::VoDLocker, providerpair("VoDLocker", "vodlocker.com")}
})
, _yt_dl_path("youtube-dl")
, _useragent("")
, _episode_range{{0, std::numeric_limits<std::uint16_t>::max()}}
, _season_range{{0, 0}}
, _use_current_episode(0b00)
, _playlist(PlaylistFormat::PL_RAW)
, _direct_url(false)
, _url("")
, _resolve(false)
, _delay(0)
, _errors_fatal(false)
{
	std::uint8_t ret;

	ret = read_configfile();

	if (ret != 0)
	{
		tterror << "Reading config file failed\n";
		if (_errors_fatal)
			exit(global::Error::ConfigError);
	}
}

void Config::populate_providers(const std::string &providerlist)
{ // providerlist is a comma separated list
	ttdebug << "Compiling list of hosting providers: <" + providerlist + ">...\n";

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
			// Transform provider names to lowercase, for comparing
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
		exit(global::Error::ConfigError);
	}
}

Config global::config;		// Declared in global.hpp
