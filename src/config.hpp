/******************************************************************************
 * "THE HUG-WARE LICENSE" (Revision 2): As long as you retain this notice you *
 * can do whatever you want with this stuff. If we meet some day, and you     *
 * think this stuff is worth it, you can give me/us a hug.                    *
 ******************************************************************************/
//Author: tastytea <tastytea@tastytea.de>


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <libconfig.h++>

typedef std::map<std::string, std::string> cfgmap;
typedef std::pair<std::string, std::string> cfgpair;

bool readconfig(cfgmap &data);

#endif
