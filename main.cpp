/******************************************************************************
 * "THE HUG-WARE LICENSE":                                                    *
 * tastytea <tastytea@tastytea.de> wrote this file. As long as you retain     *
 * this notice you can do whatever you want with this stuff. If we meet       *
 * some day, and you think this stuff is worth it, you can give me a hug.     *
 ******************************************************************************/

#include "Poco/Exception.h"
#include "Poco/StreamCopier.h"

#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPSStreamFactory.h"

#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/SSLManager.h"

#include <memory>
#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <vector>

//TODO: cleanup, prettify
//TODO: Error handling & reporting

const std::string version = "0.1";
enum Services
{ // Services who provide links to entire seasons
	BurningSeries
};
Services service;

std::string getpage(const std::string &url)
{
	std::string content;
	try
	{
		auto &opener = Poco::URIStreamOpener::defaultOpener();
		auto uri = Poco::URI { url };
		auto is = std::shared_ptr<std::istream> { opener.open(uri) };
		Poco::StreamCopier::copyToString(*(is.get()), content);
	}
	catch (Poco::Exception &e)
	{
		std::cerr << e.displayText() << std::endl;
	}

	return content;
}

std::string getlink(const std::string &url, const std::string &provider)
{ // Takes URL of episode-page, returns URL of stream-page
	std::string content = getpage(url);

	if (service == BurningSeries)
	{
		if (provider == "Streamcloud")
		{
			std::regex reStreamPage("<a href=\"(https?://streamcloud\\.eu/.*)\" target=");
			std::smatch match;
			
			if (std::regex_search(content, match, reStreamPage))
			{
				if (match[1].str().find("https") != std::string::npos)
				{
					return match[1].str();
				}
				else
				{
					return "https" + match[1].str().substr(4, std::string::npos);
				}
			}
		}
		else if (provider == "Vivo")
		{
			std::regex reStreamPage("<a href=\"(https?://vivo\\.sx/.*)\" target=");
			std::smatch match;
			
			if (std::regex_search(content, match, reStreamPage))
			{
				if (match[1].str().find("https") != std::string::npos)
				{
					return match[1].str();
				}
				else
				{
					return "https" + match[1].str().substr(4, std::string::npos);
				}
			}
		}
		else if (provider == "PowerWatch")
		{
			std::regex reStreamPage("<a href=\"(https?://powerwatch\\.pw/.*)\" target=");
			std::smatch match;
			
			if (std::regex_search(content, match, reStreamPage))
			{
				return match[1].str();
			}
		}
	}

	return "";
}

int main(int argc, char const *argv[])
{ // TODO: options for season(s), playlist format, preferred streaming providers, episode range
	std::string directoryurl = "";	// URL for the overview-page of a series,
									// e.g. https://bs.to/serie/Die-Simpsons/1
	std::vector<std::string> streamprovider =
	{
		"Streamcloud",	// List of streaming providers,
		"Vivo",			// name must match hyperlinks
		"PowerWatch"
	};
	if (argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " URL" << std::endl;
		return 1;
	}
	else
	{ // Set URL of tv series
		directoryurl = argv[1];
	}

	service = BurningSeries;

	Poco::Net::HTTPStreamFactory::registerFactory();
	Poco::Net::HTTPSStreamFactory::registerFactory();

	Poco::Net::initializeSSL();
	Poco::Net::SSLManager::InvalidCertificateHandlerPtr ptrHandler(
		new Poco::Net::AcceptCertificateHandler(false));
	Poco::Net::Context::Ptr ptrContext(
		new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, ""));
	Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);

	const auto url = directoryurl;
	const auto content = getpage(url);

	if (service == BurningSeries)
	{
		std::vector<std::string>::iterator it;
		std::string provider_re = "(";

		for (it = streamprovider.begin(); it != streamprovider.end(); ++it)
		{ // Build regular expression for all supported streaming providers
			if (it != streamprovider.begin())
			{ // Add | unless it is the first match
				provider_re += "|";
			}
			provider_re += *it;
		}
		provider_re += ")";

		std::regex reEpisodePage("href=\"(serie/.*/[[:digit:]]+/([[:digit:]]+)-.*/(" + provider_re + ")-[0-9])\">");
		std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_re_end;

		while (it_re != it_re_end)
		{
			static short episode;
			if (std::stoi((*it_re)[2]) != episode)
			{ // 1 == link, 2 == episode, 3 == provider
				std::string episodelink = "https://bs.to/" + (*it_re)[1].str();
				std::cout << getlink(episodelink, (*it_re)[3]) << std::endl;
				episode = std::stoi((*it_re)[2]);
			}
			++it_re;
		}
	}

	return 0;
}
