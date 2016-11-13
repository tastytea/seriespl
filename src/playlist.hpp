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

#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "config.hpp"
#include <string>
#include <vector>
#include <sys/types.h>
#include <utility>		// std::pair

class Playlist
{
public:
	// URL, title
	typedef std::pair<std::string, const std::string> episodepair;

	/*! \param &cfg Config object */
	explicit Playlist(const Config &cfg);
	const uint8_t print(const std::vector<episodepair> &episodes) const;

private:
	Config _cfg;

	const std::string replace_chars(const std::string &title) const;
};

#endif
