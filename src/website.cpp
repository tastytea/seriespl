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

#include "website.hpp"
#include <sys/types.h>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <iostream>

Website::Website(const Config &cfg)
:	_cfg(cfg)
{}

size_t Website::curl_write_data(char *data, size_t size, size_t nmemb, std::string *stream)
{
	if (stream != NULL)
	{
		stream->append(data, size * nmemb);
		return size * nmemb;
	}
	else
	{
		return 0;
	}
}
std::string Website::getpage(const std::string &url)
{
	CURL *curl;
	CURLcode res;
	std::string data;
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	if (_cfg.get_useragent() != "")
	{
		curl_easy_setopt(curl, CURLOPT_USERAGENT, _cfg.get_useragent().c_str());
	}

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return data;
}
