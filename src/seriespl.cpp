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

#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <vector>
#include <unistd.h>	// sleep()
#include <map>
#include <utility>
#include <cstdio>

#include "seriespl.hpp"
#include "config.hpp"

Seriespl::Seriespl()
	// Set default list of active streaming providers, SSL only
:	Providers_ssl({ Streamcloud, Vivo, Shared, YouTube, OpenLoad }),
	Providers_nossl({ PowerWatch, CloudTime, AuroraVid, Vidto }),
	Providers(Providers_ssl),
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
	}),
	config("seriespl")
{
	// read config and set streaming providers, if specified
	config.verbose = true;
	if (config.read())
	{
		if (config.get_value("streamproviders") != "")
			populate_providers(config.get_value("streamproviders"));
		if (config.get_value("youtube-dl") != "")
			yt_dl_path = config.get_value("youtube-dl");
	}
}

Seriespl::~Seriespl() {}

int Seriespl::run(int argc, char const *argv[])
{
	int ret = handle_args(argc, argv);

	if (ret > 0)
	{
		return ret;
	}
	else if (ret < 0)
	{
		return 0;
	}

	set_service();

	switch (service)
	{
		case BurningSeries:
			return burningseries();
			break;
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
