/******************************************************************************
 * "THE HUG-WARE LICENSE" (Revision 2): As long as you retain this notice you *
 * can do whatever you want with this stuff. If we meet some day, and you     *
 * think this stuff is worth it, you can give me/us a hug.                    *
 ******************************************************************************/
//Author: tastytea <tastytea@tastytea.de>

#ifndef HTTP_HPP
#define HTTP_HPP

#include <curl/curl.h>
#include <vector>

size_t curl_write_data(void *contents, size_t size, size_t nmemb, void *user);

std::string getpage(const std::string &url); // Fetch URL, return content

#endif
