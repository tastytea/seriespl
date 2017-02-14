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

#include "website.hpp"
#include "config.hpp"
#include <sys/types.h>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <iostream>
#include <map>

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Website::Website(const Config &cfg)
:	_cfg(cfg)
{
	curl_global_init(CURL_GLOBAL_ALL);
}

Website::~Website()
{
	curl_global_cleanup();
}

const size_t Website::curl_write_data(char *data, size_t size, size_t nmemb, std::string *stream)
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
const std::string Website::getpage(const std::string &url)
{
	CURL *curl;
	CURLcode res;
	std::string data;

	curl = curl_easy_init();
	if (_cfg.get_use_tor())
	{
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4A); 
		curl_easy_setopt(curl, CURLOPT_PROXY, _cfg.get_tor_address().c_str());
	}
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
		return "";
	}

	curl_easy_cleanup(curl);

	return data;
}

const uint8_t Website::resolve_redirect(std::string &url)
{
	CURL *curl;
	CURLcode res;
	char *location;
	uint8_t ret = 0;

	if (_cfg.get_use_tor())
	{
		static uint8_t counter = 0;

		if (counter == 4)
		{	// Get new IP after 4 URLs
			if (tor_newip() != 0)
			{
				std::cerr << "Error: Could not renew IP\n";
			}
			counter = 1;
		}
		else
		{
			++counter;
		}
	}

	curl = curl_easy_init();
	if(curl)
	{
		if (_cfg.get_use_tor())
		{
			curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4A); 
			curl_easy_setopt(curl, CURLOPT_PROXY, _cfg.get_tor_address().c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);	// Do not output body
		if (_cfg.get_useragent() != "")
		{
			curl_easy_setopt(curl, CURLOPT_USERAGENT, _cfg.get_useragent().c_str());
		}
		res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &location);
			if((res == CURLE_OK) && location)
			{
				url = location;
			}
		}
		else
		{
			ret = 2;
		}
	}
	else
	{
		ret = 1;
	}

	curl_easy_cleanup(curl);

	return ret;
}

const uint8_t Website::tor_newip()
{
	const std::string host = _cfg.get_tor_address().substr(0, _cfg.get_tor_address().find(':'));
	const uint16_t port = _cfg.get_tor_controlport();
	const std::string password = _cfg.get_tor_password();
	int sockfd;
	struct sockaddr_in remote;
	const uint8_t bufsize = 255;
	char buffer[bufsize];

	// Clear out needed memory
	memset(buffer, 0, bufsize);
	memset(&remote, 0, sizeof(remote));
	// Fill in required details in the socket structure
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(host.c_str());
	const std::array<const std::string, 2> commands =
		{{ "authenticate \"" + password + "\"\n", "SIGNAL NEWNYM\n" }};

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return 1;
	}
	// Connect to remote host
	if(connect(sockfd, (struct sockaddr *) &remote, sizeof(remote)) < 0)
	{
		perror("connect");
		return 1;
	}

	// Send commands
	for (const std::string &cmd : commands)
	{
		ssize_t ret;

		// Send command
		strncpy(buffer, cmd.c_str(), cmd.length());
		ret = write(sockfd, buffer, cmd.length());
		if (ret < 0)
		{
			perror("write");
		}

		// Receive and check status
		if((ret = read(sockfd, &buffer, bufsize - 1)) > 0)
		{
			buffer[ret] = '\0'; // null terminate the string
			if (strncmp(buffer, "250", 3) != 0)
			{	// Print response on error
				std::cerr << buffer;
				return 2;
			}
		}
		if(ret < 0) {
			perror("read");
			return 1;
		}
	}

	return 0;
}

const std::string Website::to_https(const std::string &url)
{
	// Look up if provider supports SSL
	std::map<const Config::HostingProviders, const Config::providerpair>::const_iterator it;
	for (it = _cfg.get_providermap().begin(); it != _cfg.get_providermap().end(); ++it)
	{
		if (url.find("//" + (*it).second.second) != std::string::npos)
		{
			for (const Config::HostingProviders &provider : _cfg.get_providers_nossl())
			{
				if ((*it).first == provider)
				{
					return url;
				}
			}
		}
	}

	if (url[4] != 's')
	{
		return "https" + url.substr(4, std::string::npos);
	}
	else
	{
		return url;
	}
}
