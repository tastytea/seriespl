// Fetch pages, extract URLs
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


#include <curl/curl.h>
#include <vector>
#include <iostream>
#include <string>
#include <regex>
#include "seriespl.hpp"

size_t Seriespl::curl_write_data(void *contents, size_t size, size_t nmemb, void *user)
{
	auto chunk = reinterpret_cast<char*>(contents);
	auto buffer = reinterpret_cast<std::vector<char>*>(user);

	size_t priorSize = buffer->size();
	size_t sizeIncrease = size * nmemb;

	buffer->resize(priorSize + sizeIncrease);
	std::copy(chunk, chunk + sizeIncrease, buffer->data() + priorSize);

	return sizeIncrease;
}

std::string Seriespl::getpage(const std::string &url)
{ // Fetch URL, return content
	CURL *curl;
	CURLcode res;
	std::vector<char> data;
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	//curl_easy_setopt(curl, CURLOPT_USERAGENT, "seriespl/" + version);

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return std::string(data.begin(), data.end());
}

std::string Seriespl::getlink(const std::string &url, const StreamProviders &provider,
							  std::string &title)
{ // Takes URL of episode-page and streaming provider, returns URL of stream-page or "" on error
	std::string content = getpage(url);
	std::string streamurl = "";

	if (service == BurningSeries)
	{
		std::regex reStreamPage("(<a href| src)=[\"\'](https?://" +
			providermap.at(provider).second + "/.*)[\"\']( target=|></iframe>)");
		std::regex reTitle(
			std::string("<h2 id=\"titleGerman\">(.*)") +
				"[[:space:]]+<small id=\"titleEnglish\" lang=\"en\">(.*)</small>");
		std::smatch match;

		if (std::regex_search(content, match, reStreamPage))
		{
			streamurl = match[2].str();
			
			if (provider == Streamcloud ||
				provider == Vivo ||
				provider == Shared ||
				provider == YouTube)
			{ // Make sure we use SSL where supported
				if (streamurl.find("https") == std::string::npos)
				{
					streamurl = "https" + streamurl.substr(4, std::string::npos);
				}
			}
		}
		else
		{
			std::cerr << "Error extracting stream" << std::endl;
		}

		if (std::regex_search(content, match, reTitle))
		{
			if (match[1].str() != "")		// German
				title = match[1].str();
			else if (match[2].str() != "")	// English
				title = match[2].str();
		}
	} // service-if

	return streamurl;
}

std::string Seriespl::getlink(const std::string &url, const StreamProviders &provider)
{
	std::string title;
	return getlink(url, provider, title);
}

std::string Seriespl::get_direct_url(const std::string &providerurl)
{ // Use youtube-dl to print the direct URL of the video file
	FILE *ytdl;
	char buffer[256];
	std::string result;

	if(!(ytdl = popen(( yt_dl_path + " --get-url " + providerurl).c_str(), "r")))
	{
		std::cerr << "Error: Can not spawn process for youtube-dl" << std::endl;
		return "";
	}

	while(fgets(buffer, sizeof(buffer), ytdl) != NULL)
	{
		result += buffer;
	}
	if (pclose(ytdl) != 0)
	{
		std::cerr << "Error: youtube-dl returned non-zero exit code" << std::endl;
		return "";
	}

	return result.substr(0, result.find_last_not_of("\r\n") + 1);
}
