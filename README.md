**seriespl** extracts stream-URLs for entire seasons of tv series from bs.to (Burning-Series). These URLs can then be further processed by e.g. [youtube-dl](https://rg3.github.com/youtube-dl/) or [mpv](https://mpv.io/).

# Install
## Dependencies
 * Linux (May work on other OS, not tested)
 * Newer than ancient clang or gcc (must support -std=c++11)
 * glibc (another libc should work, must support getopt)
 * make
 * [poco](http://pocoproject.org/) (works with 1.4.6_p4)
 * [Doxygen](http://doxygen.org/) (for the man page)

Does not work with Debian Jessie due to old libpoco-dev.

## Get sourcecode
### Current release
[Download the current release](https://github.com/tastytea/seriespl/releases).

### Development version
    git clone https://github.com/tastytea/seriespl.git

## Compile
    make
    make install

# Usage
Working websites:
 * https://bs.to/

Working streaming providers:
 * Streamcloud
 * Vivo
 * Shared
 * YouTube
 * PowerWatch (no ssl)
 * CloudTime (no ssl)
 * AuroraVid (no ssl)

## Overview
    usage: seriespl [-h] [-i]|[-p list] [-e episodes] [-s seasons] [-f format] URL

      -h                   Show this help
      -p stream providers  Comma delimited list. Available:
                           Streamcloud,Vivo,Shared,YouTube,PowerWatch,CloudTime,AuroraVid
      -i                   Use stream providers without SSL support too
      -e                   Episode range, e.g. 2-5 or 7 or 9- or c (for current)
      -s                   Season range, e.g. 1-2 or 4
      -f                   Playlist format. Available: raw, m3u, pls

## Examples
Download all episodes of South Park Season 1-3:

    seriespl -s 1-3 https://bs.to/serie/South-Park | youtube-dl -a -

Watch all episodes starting from 6 of South Park Season 1:

    seriespl -e 6- https://bs.to/serie/South-Park/1 | mpv --playlist=-

Create an M3U playlist of Southpark Season 2 using only Streamcloud and Shared:

    seriespl -s 2 -f m3u -p Streamcloud,Shared https://bs.to/serie/South-Park > playlist.m3u

Watch only current Episode:

    seriespl -e c https://bs.to/serie/South-Park/1/1-Cartman-und-die-Analsonde | mpv --playlist=-

# Legal aspects
Please inform yourself if using this program is legal under your jurisdiction. I am not responsible for your actions.


# Todo
## Until 1.0
 * ~~Better error handling & reporting~~
 * ~~Season selection~~
 * ~~Extended M3U playlist support~~
 * ~~PLS playlist support~~
 * ~~Support for all streaming providers available on bs.to and compatible with youtube-dl~~

## After 1.0
 * Support for more Websites

# Bugs & feature requests
[Bugtracker](https://github.com/tastytea/seriespl/issues) on GitHub or via [E-Mail](mailto:bugs Ⓐ tastytea.de)
