// Base class for all websites
/*
 *  Copyright © 2016 tastytea <tastytea@tastytea.de>
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

#ifndef WEBSITE_HPP
#define WEBSITE_HPP

#include "config.hpp"
#include "global.hpp"
#include <string>
#include <vector>
#include <sys/types.h>

class Website
{
public:
	/*! \param &cfg Config object */
	explicit Website(const Config &cfg);
	~Website();
	/*!	Derived classes must implement this.
		\param &episodes Vector of std::pair<std::string, std::string>
		to hold URLs and titles for episodes
		\return 0 on success */
	virtual const uint8_t getlinks(std::vector<Global::episodepair> &episodes) = 0;

protected:
	const Config &_cfg;

	/*! \param &url URL to fetch
		\return Webpage */
	const std::string getpage(const std::string &url);

protected:
	/*! \param &url URL to resolve, will be overwritten
		\return 0 on success */
	const uint8_t resolve_redirect(std::string &url);
	/*!	\param &url URL
		\return url starting with https */
	const std::string to_https(const std::string &url);

private:
	static const size_t curl_write_data(char *data, size_t size, size_t nmemb, std::string *stream);
	const uint8_t tor_newip();
};

#endif
