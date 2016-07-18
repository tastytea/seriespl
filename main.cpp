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
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <limits>

//TODO: Better error handling & reporting
//TODO: Other Services

const std::string version = "0.5";
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
{ // Takes URL of episode-page, returns URL of stream-page or "" on error
	std::string content = getpage(url);
	std::string streamurl = "";

	if (service == BurningSeries)
	{
		std::regex reStreamPage;
		std::smatch match;

		if (provider == "Streamcloud")
		{
			reStreamPage.assign("<a href=\"(https?://streamcloud\\.eu/.*)\" target=");
		}
		else if (provider == "Vivo")
		{
			reStreamPage.assign("<a href=\"(https?://vivo\\.sx/.*)\" target=");
		}
		else if (provider == "PowerWatch")
		{
			reStreamPage.assign("<a href=\"(https?://powerwatch\\.pw/.*)\" target=");
		}
		else if (provider == "CloudTime")
		{
			reStreamPage.assign("<a href=\"(https?://www.cloudtime\\.to/.*)\" target=");
		}
		else if (provider == "AuroraVid")
		{
			reStreamPage.assign("<a href=\"(https?://auroravid\\.to/.*)\" target=");
		}
		else if (provider == "Shared")
		{
			reStreamPage.assign("<a href=\"(https?://shared\\.sx/.*)\" target=");
		}
		else if (provider == "YouTube")
		{
			reStreamPage.assign("<a href=\"(https?://www\\.youtube\\.com/.*)\" target=");
		}
		else
		{
			std::cerr << "Error: Stream provider unknown" << std::endl;
		}

		if (std::regex_search(content, match, reStreamPage))
		{
			streamurl = match[1].str();
		}
		else
		{
			std::cerr << "Error extracting stream" << std::endl;
		}

		if (provider == "Streamcloud" ||
			provider == "Vivo" ||
			provider == "Shared" ||
			provider == "YouTube")
		{ // Make sure we use SSL where supported
			if (std::regex_search(content, match, reStreamPage))
			{
					if (streamurl.find("https") == std::string::npos)
					{
						streamurl = "https" + streamurl.substr(4, std::string::npos);
					}
			}
		}
	} // service-if

	return streamurl;
}

int main(int argc, char const *argv[])
{ // TODO: options for season(s), playlist format
	std::string directoryurl = "";	// URL for the overview-page of a series,
									// e.g. https://bs.to/serie/Die-Simpsons/1
	std::vector<std::string> streamprovider =
	{ // FIXME: Better solution for streamprovider list
		"Streamcloud",	// List of active streaming providers,
		"Vivo",			// name must match hyperlinks
		"Shared",
		"YouTube"
	};
	unsigned short startEpisode = 0, endEpisode = std::numeric_limits<unsigned short>::max();
	 short startSeason = -1, endSeason = -1;
	int opt;
	std::string usage = std::string("usage: ") + argv[0] +
		" [-h] [-i]|[-p stream providers] [-e episode range] [-s season range] URL";
	
	while ((opt = getopt(argc, (char **)argv, "hp:ie:s:")) != -1)
	{
		std::istringstream ss;
		std::string item;
		std::string episodes, seasons;
		size_t pos;
		switch (opt)
		{
			case 'h':	// Help
				std::cout << usage << std::endl << std::endl;
				std::cout <<
					"  -h                   Show this help" << std::endl;
				std::cout <<
					"  -p stream providers  Comma delimited list. Available:" << std::endl;
				std::cout <<
					"                       Streamcloud,Vivo,Shared,YouTube,PowerWatch,CloudTime,AuroraVid" << std::endl;
				std::cout <<
					"  -i                   Use stream providers without SSL support too" << std::endl;
				std::cout <<
					"  -e                   Episode range, e.g. 2-5 or 7 or 9-" << std::endl;
				std::cout <<
					"  -s                   Season range, e.g. 1-2 or 4" << std::endl;
				return 0;
				break;
			case 'p':	// Provider
				streamprovider.clear();
				ss.str(optarg);
				while (std::getline(ss, item, ','))
				{
					streamprovider.push_back(item);
				}
				break;
			case 'i':	// Insecure
					streamprovider =
					{
						"Streamcloud",
						"Vivo",
						"Shared",
						"YouTube",
						"PowerWatch",
						"CloudTime",
						"AuroraVid"
					};
				break;
			case 'e':	// Episodes
				episodes = optarg;
				pos = episodes.find('-');
				if (pos != std::string::npos)
				{
					try
					{
						startEpisode = std::stoi( episodes.substr(0, pos) );
						if (episodes.length() > pos + 1)
						{ // If episodes = 5-, output all episodes, beginning with 5
							endEpisode = std::stoi( episodes.substr(pos + 1) );
						}
					}
					catch (std::exception &e)
					{ // There is a '-' but no numbers around it
						std::cerr << "Error: Can not decipher episode range, " <<
							"defaulting to all." << std::endl;
						sleep(2);
					}
				}
				else
				{
					try
					{ // Is episodes a single number?
						startEpisode = std::stoi(episodes);
						endEpisode = std::stoi(episodes);
					}
					catch (std::exception &e)
					{
						std::cerr << "Error: Can not decipher episode range, " <<
							"defaulting to all." << std::endl;
						sleep(2);
					}
				}
				break;
			case 's':	// Seasons
				seasons = optarg;
				pos = seasons.find('-');
				if (pos != std::string::npos)
				{
					try
					{
						startSeason = std::stoi( seasons.substr(0, pos) );
						endSeason = std::stoi( seasons.substr(pos + 1) );
					}
					catch (std::exception &e)
					{ // There is a '-' but no numbers around it
						std::cerr << "Error: Can not decipher season range, " <<
							"defaulting to selected." << std::endl;
						startSeason = -1;
						endSeason = -1;
						sleep(2);
					}
				}
				else
				{
					try
					{ // Is seasons a single number?
						startSeason = std::stoi(seasons);
						endSeason = std::stoi(seasons);
					}
					catch (std::exception &e)
					{
						std::cerr << "Error: Can not decipher season range, " <<
							"defaulting to selected." << std::endl;
						sleep(2);
					}
				}
				break;
			default:
				std::cerr << usage << std::endl;
				return 1;
				break;
		}
	}
	if (optind >= argc)
	{
		std::cerr << usage << std::endl;
		return 1;
	}
	else
	{ // Set URL of tv series
		directoryurl = argv[optind];
	}

	if (directoryurl.find("bs.to") != std::string::npos)
	{
		service = BurningSeries;
	}
	else
	{
		std::cerr << "Error: Could not determine which website you specified, " <<
			"defaulting to Burning-Series." << std::endl;
		sleep(2);
		service = BurningSeries;
	}

	Poco::Net::HTTPStreamFactory::registerFactory();
	Poco::Net::HTTPSStreamFactory::registerFactory();

	Poco::Net::initializeSSL();
	Poco::Net::SSLManager::InvalidCertificateHandlerPtr ptrHandler(
		new Poco::Net::AcceptCertificateHandler(false));
	Poco::Net::Context::Ptr ptrContext(
		new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, ""));
	Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);

	std::string content;

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

		for (short season = startSeason; season <= endSeason; ++season)
		{
			if (season != -1)
			{ // A season range was selected
				//FIXME: If season is higher than available seasons, season 1 is returned
				std::regex reSeries("(https://bs.to/serie/[^/]*/).*");
				std::smatch match;
			
				// Generate URL for each season
				if (std::regex_search(directoryurl, match, reSeries))
				{
					directoryurl = match[1].str() + std::to_string(season);
				}
				else
				{
					directoryurl = directoryurl + "/" + std::to_string(season);
				}
				content = getpage(directoryurl);
			}
			else
			{ // If no season range was selected, use supplied URL
				content = getpage(directoryurl);
			}

			std::regex reEpisodePage("href=\"(serie/.*/[[:digit:]]+/([[:digit:]]+)-.*/(" + provider_re + ")-[0-9])\">");
			std::sregex_iterator it_re(content.begin(), content.end(), reEpisodePage);
			std::sregex_iterator it_re_end;

			while (it_re != it_re_end)
			{ // 1 == link, 2 == episode, 3 == provider
				static short episode = 0;
				if (episode == 0 && startEpisode > 0)
				{ // Set episode to the right starting value if episode range is specified
					episode = startEpisode - 1;
				}
				if (std::stoi((*it_re)[2]) >= startEpisode &&
					std::stoi((*it_re)[2]) <= endEpisode &&
					std::stoi((*it_re)[2]) != episode)
				{
					if ( stoi((*it_re)[2]) > (episode + 1) )
					{ // If current episode is > last episode + 1, we are missing an episode
						std::cerr << "Error: Could not get URL for episode " << (episode + 1);
						if ((episode + 2) != stoi((*it_re)[2]))
						{ // If more than 1 episode is missing
							std::cerr << "-" << (stoi((*it_re)[2]) - 1);
						}
						std::cerr << "." << std::endl;
					}
					std::string episodelink = "https://bs.to/" + (*it_re)[1].str();
					std::cout << getlink(episodelink, (*it_re)[3]) << std::endl;
					episode = std::stoi((*it_re)[2]);
				}
				++it_re;
			}
		}
	}

	return 0;
}
