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
#include <map>
#include <utility>


const std::string version = "0.6";
enum Services
{ // Services who provide links to entire seasons
	BurningSeries
};
Services service;

enum StreamProviders
{ // Available stream providers
	Streamcloud,
	Vivo,
	Shared,
	YouTube,
	PowerWatch,
	CloudTime,
	AuroraVid
};
std::vector<StreamProviders> Providers;	// List of active stream providers

typedef const std::pair <std::string, std::string> providerpair; // Name and domain
const std::map<StreamProviders, providerpair> providermap =
{ // Map stream providers to string and URL
	{Streamcloud, providerpair("Streamcloud", "streamcloud.eu")},
	{Vivo, providerpair("Vivo", "vivo.sx")},
	{Shared, providerpair("Shared", "shared.sx")},
	{YouTube, providerpair("YouTube", "www.youtube.com")},
	{PowerWatch, providerpair("PowerWatch", "powerwatch.pw")},
	{CloudTime, providerpair("CloudTime", "www.cloudtime.to")},
	{AuroraVid, providerpair("AuroraVid", "auroravid.to")}
};

enum PlaylistFormat
{
	PL_RAW,
	PL_M3U,
	PL_PLS
};

void init_poco()
{
	Poco::Net::HTTPStreamFactory::registerFactory();
	Poco::Net::HTTPSStreamFactory::registerFactory();

	Poco::Net::initializeSSL();
	Poco::Net::SSLManager::InvalidCertificateHandlerPtr ptrHandler(
		new Poco::Net::AcceptCertificateHandler(false));
	Poco::Net::Context::Ptr ptrContext(
		new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, ""));
	Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);
}

std::string getpage(const std::string &url)
{ // Fetch URL, return content
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

std::string getlink(const std::string &url, const StreamProviders &provider, std::string &title)
{ // Takes URL of episode-page and streaming provider, returns URL of stream-page or "" on error
	std::string content = getpage(url);
	std::string streamurl = "";

	if (service == BurningSeries)
	{
		std::regex reStreamPage("<a href=\"(https?://" +
			providermap.at(provider).second + "/.*)\" target=");
		std::regex reTitle(
			std::string("<h2 id=\"titleGerman\">(.*)") +
				"[[:space:]]+<small id=\"titleEnglish\" lang=\"en\">(.*)</small>");
		std::smatch match;

		if (std::regex_search(content, match, reStreamPage))
		{
			streamurl = match[1].str();
			
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
			if (match[1].str() != "")
				title = match[1].str();
			else if (match[2].str() != "")
				title = match[2].str();
		}

		// size_t pos = url.find('/', 20) + 1; // Skip https://bs.to/serie/
		// title = url.substr(pos, url.find('/', url.find('/', pos) + 1) - pos);
	} // service-if

	return streamurl;
}

std::string getlink(const std::string &url, const StreamProviders &provider)
{
	std::string title;
	return getlink(url, provider, title);
}

void print_playlist(const PlaylistFormat &playlist, const std::string &url,
					const std::string &title)
{
	static unsigned short counter = 1;

	switch (playlist)
	{
		case PL_RAW:
			std::cout << url << std::endl;
			break;
		case PL_M3U:
			if (counter == 1) // Write header
			{
				std::cout << "#EXTM3U" << std::endl;
			}
			std::cout << "#EXTINF:-1," << title << std::endl;
			std::cout << url << std::endl;
			break;
		case PL_PLS:
			if (counter == 1) // Write header
			{
				std::cout << "[playlist]" << std::endl;
				std::cout << "Version=2" << std::endl;
			}
			if (url == "NUMBER_OF_EPISODES")
			{
				std::cout << "NumberOfEntries=" << counter - 1 << std::endl;
			}
			else
			{
				std::cout << "File" << counter << "=" << url << std::endl;
				std::cout << "Title" << counter << "=" << title << std::endl;
				std::cout << "Length" << counter << "=-1" << std::endl;
			}
			break;
	}
	++counter;
}

void print_playlist(const PlaylistFormat &playlist, const std::string &url)
{
	return print_playlist(playlist, url, "");
}

int main(int argc, char const *argv[])
{
	std::string directoryurl = "";	// URL for the overview-page of a series,
									// e.g. https://bs.to/serie/Die-Simpsons/1
	Providers =
	{ // Set default list of active streaming providers, SSL only
		Streamcloud,
		Vivo,
		Shared,
		YouTube
	};
	unsigned short startEpisode = 0, endEpisode = std::numeric_limits<unsigned short>::max();
	short startSeason = -1, endSeason = -1;
	std::string content;
	PlaylistFormat playlist = PL_RAW;

	int opt;
	std::string usage = std::string("usage: ") + argv[0] +
		" [-h] [-i]|[-p stream providers] [-e episode range] [-s season range] [-f format] URL";
	
	while ((opt = getopt(argc, (char **)argv, "hp:ie:s:f:")) != -1)
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
				std::cout <<
					"  -f                   Playlist format. Available: raw, m3u or pls" << std::endl;
				return 0;
				break;
			case 'p':	// Provider
				Providers.clear();
				ss.str(optarg);
				while (std::getline(ss, item, ','))
				{
					std::map<StreamProviders, providerpair>::const_iterator it = providermap.begin();
					for (; it != providermap.end(); ++it)
					{
						if (it->second.first == item)
							Providers.push_back(it->first);
					}
				}
				break;
			case 'i':	// Insecure
					Providers =
					{
						Streamcloud,
						Vivo,
						Shared,
						YouTube,
						PowerWatch,
						CloudTime,
						AuroraVid
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
			case 'f':	// Format
				if (strncmp(optarg, "raw", 3) == 0)
					playlist = PL_RAW;
				else if (strncmp(optarg, "m3u", 3) == 0)
					playlist = PL_M3U;
				else if (strncmp(optarg, "pls", 3) == 0)
					playlist = PL_PLS;
				else
					std::cerr << "Playlist format not recognized, defaulting to raw." << std::endl;
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

	init_poco();

	if (service == BurningSeries)
	{
		std::string provider_re = "(";

		for (std::vector<StreamProviders>::iterator it = Providers.begin(); it != Providers.end(); ++it)
		{ // Build regular expression for all supported streaming providers
			if (it != Providers.begin())
			{ // Add | unless it is the first match
				provider_re += "|";
			}
			provider_re += providermap.at(*it).first;
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
					std::map<StreamProviders, providerpair>::const_iterator it = providermap.begin();
					for (; it != providermap.end(); ++it)
					{
						if (it->second.first == (*it_re)[3])
						{
							std::string streamtitle;
							std::string streamurl = getlink(episodelink, it->first, streamtitle);
							print_playlist(playlist, streamurl, streamtitle);
						}
					}
					episode = std::stoi((*it_re)[2]);
				}
				++it_re;
			} // Iterating through matches
			if (playlist == PL_PLS)
			{ // Print NumberOfEntries
				print_playlist(PL_PLS, "NUMBER_OF_EPISODES");
			}
		}
	}

	return 0;
}
