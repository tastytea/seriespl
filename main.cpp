/******************************************************************************
 * "THE HUG-WARE LICENSE":                                                    *
 * tastytea <tastytea@tastytea.de> wrote these files. As long as you retain   *
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

std::string getlink(const std::string &url)
{ // Takes URL of episode-page, returns URL of stream-page
	std::string content = getpage(url);

	if (service == BurningSeries)
	{
		std::regex reStreamPage("<a href=\"(http://streamcloud.*)\" target=");
		std::smatch match;
		
		if (std::regex_search(content, match, reStreamPage))
		{
			return match[1].str();
		}
	}

	return "";
}

int main(int argc, char const *argv[])
{ // TODO: options for season(s), playlist format, streaming provider
	std::string directoryurl = "";	// URL for the overview-page of a series,
									// e.g. https://bs.to/serie/Die-Simpsons/1
	std::vector<std::string> streamprovider = { "Streamcloud"};	// List of streaming providers,
																// name must match hyperlinks
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
			{
				provider_re += "|";
			}
			provider_re += *it;
		}
		provider_re += ")";

		// FIXME: This will result in multiple links per episode if streamprovider > 1
		std::regex reEpisodePage("href=\"serie/.*/[0-9]+/.*/" + provider_re + "-[0-9]\">");
		std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_re_end;

		while (it_re != it_re_end)
		{ // TODO: redo with std::smatch for lesser obscurity
			std::string episodelink = "https://bs.to/" +	// Remove HTML, add domain
				it_re->str().substr(6, it_re->str().length() - 6 - 2);
			std::cout << getlink(episodelink) << std::endl;
			++it_re;
		}
	}

	return 0;
}
