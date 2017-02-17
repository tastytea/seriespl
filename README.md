**seriespl** extracts stream-URLs for entire seasons of tv series from bs.to (Burning-Series) and geektv.ma. These URLs can then be further processed by e.g. [youtube-dl](https://rg3.github.com/youtube-dl/) or [mpv](https://mpv.io/).

# Install
## Dependencies
 * Tested OS: Linux, macOS
 * C++ compiler (tested: gcc 4.9 and clang 3.5.0)
 * make
 * [curl](https://curl.haxx.se/) (libcurl in some package managers)
 * [libconfig](http://www.hyperrealm.com/libconfig/libconfig.html) (libconfig++ in some package managers)

## Get sourcecode
### Current release
[Download the current release](https://github.com/tastytea/seriespl/releases).

### Development version
    git clone https://github.com/tastytea/seriespl.git

[![Build Status](https://travis-ci.org/tastytea/seriespl.svg?branch=master)](https://travis-ci.org/tastytea/seriespl)

## Compile
    make
    make install

If you have installed dependencies in non standard paths (homebrew for example installs into /usr/local), you can use `EXTRA_CXXFLAGS` and `EXTRA_LDFLAGS` to point to these paths. For example:

    make EXTRA_CXXFLAGS="-I/usr/local/include" EXTRA_LDFLAGS="-L/usr/local/lib"

## Packages
You find meta files for packages and instructions to build them for **Gentoo**, **Void** and **Debian** in [packages/](https://github.com/tastytea/seriespl/tree/master/packages/).

# Usage
Working websites:

 * https://bs.to/
 * https://geektv.ma/

Working hosting providers:

 * Streamcloud
 * Vivo
 * Shared
 * YouTube
 * OpenLoad
 * PowerWatch (no ssl)
 * CloudTime (no ssl)
 * AuroraVid (no ssl)
 * Vidto (no ssl)
 * VoDLocker (no ssl)

## Overview
    usage: bin/seriespl [-h]|[-V] [-i]|[-p list] [-e episodes] [-s seasons] [-y] URL
    
      -h                   Show this help and exit
      -p hosting providers Comma delimited list. Available:
                           Streamcloud,Vivo,Shared,YouTube,OpenLoad,OpenLoadHD,PowerWatch,CloudTime,AuroraVid,Vidto,VoDLocker
      -i                   Use hosting providers without SSL support too
      -e episodes          Episode range, e.g. 2-5 or 7 or 9-, use c for current
      -s episodes          Season range, e.g. 1-2 or 4
      -f format            Playlist format. Available: raw, m3u, pls
      -y                   Use youtube-dl to print the direct URL of the video file
      -a user-agent        Set User-Agent
      -r                   Resolve redirections
      -d                   Delay in seconds between connection attempts
      -t                   Use Tor
      -V                   Output version and copyright information and exit

## Examples
Download all episodes of South Park Season 1-3, resolve redirections first with a delay of 120 seconds:

    seriespl -s 1-3 -r -d 120 https://bs.to/serie/South-Park | youtube-dl -a -

Watch all episodes starting from 6 of The Simpsons Season 1:

    seriespl -e 6- https://geektv.ma/season-1/the-simpsons.html | mpv --playlist=-

Create an M3U playlist of Southpark Season 2 using only Streamcloud and Shared:

    seriespl -s 2 -f m3u -p Streamcloud,Shared https://bs.to/serie/South-Park > playlist.m3u

Watch only current Episode in vlc, with the title from the website:

    seriespl -i -e c -y -f m3u https://bs.to/serie/South-Park/1/1-Cartman-und-die-Analsonde | vlc -

# Config
Place the config file in `${XDG_CONFIG_HOME}/seriespl.cfg` or `${HOME}/.config/seriespl.cfg`. Example:

    hostingproviders = "Streamcloud,Vivo,Shared,YouTube,OpenLoad"
    youtube-dl = "/usr/bin/youtube-dl"
    user-agent = "Mozilla/5.0 (Windows NT 6.1; rv:45.0) Gecko/20100101 Firefox/45.0"
    tor_address = "127.0.0.1:9050"
    tor_controlport = "9051"
    tor_password = "secure"
    delay = "120"

hostingproviders is separated by comma. The quotes are mandatory. For more info, look at the manpage.

# Copyright
    Copyright © 2016-2017 tastytea <tastytea@tastytea.de>.
    License GPLv2: GNU GPL version 2 <http://www.gnu.org/licenses/gpl-2.0.html>.
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.

# Bugs & feature requests
[Bugtracker](https://github.com/tastytea/seriespl/issues) on GitHub or via [E-Mail](mailto:bugs Ⓐ tastytea.de)
