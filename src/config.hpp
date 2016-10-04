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


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <libconfig.h++>

typedef std::map<std::string, std::string> cfgmap;
typedef std::pair<std::string, std::string> cfgpair;

bool readconfig(cfgmap &data);

#endif
