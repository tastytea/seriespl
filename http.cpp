/******************************************************************************
 * "THE HUG-WARE LICENSE" (Revision 2): As long as you retain this notice you *
 * can do whatever you want with this stuff. If we meet some day, and you     *
 * think this stuff is worth it, you can give me/us a hug.                    *
 ******************************************************************************/
//Author: tastytea <tastytea@tastytea.de>


#include <curl/curl.h>
#include <vector>
#include <iostream>

size_t curl_write_data(void *contents, size_t size, size_t nmemb, void *user)
{
	auto chunk = reinterpret_cast<char*>(contents);
	auto buffer = reinterpret_cast<std::vector<char>*>(user);

	size_t priorSize = buffer->size();
	size_t sizeIncrease = size * nmemb;

	buffer->resize(priorSize + sizeIncrease);
	std::copy(chunk, chunk + sizeIncrease, buffer->data() + priorSize);

	return sizeIncrease;
}

std::string getpage(const std::string &url)
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
