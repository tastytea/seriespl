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

int main(int argc, char const *argv[])
{ // TODO: options for season(s), playlist format, streaming provider
	std::string directoryurl = "";
	enum Services
	{
		BurningSeries
	};
	Services service;

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
		std::regex reEpisodePage("<a href=\"serie/.*/[0-9]+/.*\">");
		std::sregex_iterator it(content.begin(), content.end(), reEpisodePage);
		std::sregex_iterator it_end;

		while (it != it_end)
		{
			std::string episodelink = "https://bs.to/" +	// Remove HTML, add domain
				it->str().substr(9, it->str().length() - 9 - 2);
			std::cout << episodelink << std::endl;
			++it;
		}
	}

	return 0;
}
