/******************************************************************************
 * "THE HUG-WARE LICENSE" (Revision 2): As long as you retain this notice you *
 * can do whatever you want with this stuff. If we meet some day, and you     *
 * think this stuff is worth it, you can give me/us a hug.                    *
 ******************************************************************************/
//Author: tastytea <tastytea@tastytea.de>


#include "config.hpp"
#include <string>
#include <iostream>
#include <libconfig.h++>

bool readconfig(cfgmap &data)
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
			std::string value;

			cfg.lookupValue("streamproviders", value);
			data.insert(cfgpair("streamproviders", value));
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
