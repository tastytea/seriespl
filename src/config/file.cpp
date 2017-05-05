// Parse config file, declared in config.hpp
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

#include <iostream>
#include <string>

#include <libconfig.h++>

#include "config.hpp"
#include "../global.hpp"

std::uint8_t Config::read_configfile()
{
	std::string filepath;
	libconfig::Config cfg;

	ttdebug << "Reading config file...\n";
	// Build file path
	if (getenv("XDG_CONFIG_HOME") != NULL)
	{
		filepath = std::string(getenv("XDG_CONFIG_HOME"));
	}
	else
	{
		filepath = std::string(getenv("HOME")) + "/.config";
	}
	filepath += "/seriespl.cfg";

	try
	{
		cfg.readFile(filepath.c_str());
	}
	catch (const libconfig::FileIOException &e)
	{
		tterror << "I/O error while reading config file\n";
		return global::Error::IOError;
	}
	catch (const libconfig::ParseException &e)
	{
		tterror << "Parse error at " << e.getFile() << ":" << e.getLine()
			<< " - " << e.getError() << '\n';
		return global::Error::ParseError;
	}

	const libconfig::Setting &settings = cfg.getRoot();

	if (settings.exists("streamproviders"))					// Before 2.0.0
	{
		std::cerr << "WARNING: config file: "
			<< "streamproviders is deprecated, use hostingproviders instead\n";
		populate_providers(settings["streamproviders"].c_str());
	}
	if (settings.exists("hostingproviders"))
		populate_providers(settings["hostingproviders"].c_str());
	if (settings.exists("youtube-dl"))
		_yt_dl_path = settings["youtube-dl"].c_str();
	if (settings.exists("user-agent"))
		_useragent = settings["user-agent"].c_str();
	if (settings.exists("resolve_delay"))					// Bfore 2.3.2
	{
		std::cerr << "WARNING: config file: "
			<< "resolve_delay is deprecated, use delay instead\n";
		_delay = std::stoi(settings["resolve_delay"].c_str());
	}
	if (settings.exists("delay"))
		_delay = std::stoi(settings["delay"].c_str());

	return 0;
}
