// Very limited, but easy to use, wrapper for libconfig++

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
#include <libconfig.h++>


class Config
{
public:
	explicit Config(const std::string &name);
	~Config();
	bool read();
	std::string get_value(const std::string &key);

	bool verbose;

private:
	const std::string _name = "";
	std::string _filepath = "";
	libconfig::Config _cfg;
};

#endif
