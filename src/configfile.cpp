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

#include "configfile.hpp"
#include "global.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <libconfig.h++>

ConfigFile::ConfigFile(const std::string &name)
:	verbose(false),
	_name(name),
	_filepath("")
{
	Global::debug("configfile.cpp");
}

ConfigFile::~ConfigFile() {}

bool ConfigFile::read()
{
	// Build file path
	if (getenv("XDG_CONFIG_HOME") != NULL)
	{
		_filepath = std::string(getenv("XDG_CONFIG_HOME"));
	}
	else
	{
		_filepath = std::string(getenv("HOME")) + "/.config";
	}
	_filepath += "/" + _name + ".cfg";

	//Read config file. On error report and return false
	try
	{
		_cfg.readFile(_filepath.c_str());
	}
	catch(const libconfig::FileIOException &e)
	{
		if (verbose)
			std::cerr << "ERROR: ConfigFile: I/O error while reading \"" << _filepath
				<< "\"." << std::endl;
		return false;
	}
	catch(const libconfig::ParseException &e)
	{
		std::cerr << "ERROR: ConfigFile: Parse error at " << e.getFile() << ":" << e.getLine()
			<< " - " << e.getError() << std::endl;
		return false;
	}

	return true;
}

std::string ConfigFile::get_value(const std::string &key)
{
	std::string value = "";

	try
	{
		_cfg.lookupValue(key, value);
		return value;
	}
	catch(const libconfig::SettingNotFoundException &e)
	{ //FIXME: Never gets triggered
		if (verbose)
			std::cerr << "ERROR: ConfigFile: \"" << key << "\" not found." << std::endl;
		return "";
	}
}
