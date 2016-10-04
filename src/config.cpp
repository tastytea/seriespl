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


#include "config.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>

bool Config::read(cfgmap &data)
{
	libconfig::Config cfg;
	std::string filename;
	
	if (getenv("XDG_CONFIG_HOME") != NULL)
	{
		filename = std::string(getenv("XDG_CONFIG_HOME"));
	}
	else
	{
		filename = std::string(getenv("HOME")) + "/.config";
	}
	filename += "/seriespl.cfg";

	//Read config file. On error report and return false
	try
	{
		cfg.readFile(filename.c_str());

		try
		{
			std::string value = "";

			cfg.lookupValue("streamproviders", value);
			data.insert(cfgpair("streamproviders", value));
			value = "";
			cfg.lookupValue("youtube-dl", value);
			data.insert(cfgpair("youtube-dl", value));
		}
		catch(const libconfig::SettingNotFoundException &e)
		{
			//std::cerr <<"Setting not found in configuration file." << std::endl;
		}
	}
	catch(const libconfig::FileIOException &e)
	{
		//std::cerr << "I/O error while reading configuration file." << std::endl;
		return false;
	}
	catch(const libconfig::ParseException &e)
	{
		std::cerr << "Parse error at " << e.getFile() << ":" << e.getLine()
			<< " - " << e.getError() << std::endl;
		return false;
	}

	return true;
}
