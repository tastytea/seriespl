**seriespl** extracts stream-URLs for entire seasons of tv series from bs.to (Burning-Series). These URLs can then be further processed by e.g. [youtube-dl](https://rg3.github.com/youtube-dl/) or [mpv](https://mpv.io/).

# Install
## Dependencies
 * Linux (May work on other OS, not tested)
 * Newer than ancient clang or gcc (must support -std=c++11)
 * make
 * getopt
 * [poco](http://pocoproject.org/)

## Compile
    make

# Usage
Working streaming providers:
 * Streamcloud
 * Vivo
 * PowerWatch (no ssl)
 * CloudTime (no ssl)

The only output format is raw at the moment.

## Overview
    usage: seriespl [-h] [-i]|[-p stream providers] URL
    
      -h                   Show this help
      -p stream providers  Comma delimited list. Available:
                           Streamcloud,Vivo,PowerWatch,CloudTime
      -i                   Use stream providers without SSL support too

## Examples
Download all episodes of South Park Season 1:

    seriespl https://bs.to/serie/South-Park/1 | youtube-dl -a -

Watch all episodes of South Park Season 1:

    seriespl https://bs.to/serie/South-Park/1 | mpv --playlist=-

# Legal aspects
Please inform yourself if using this program is legal under your jurisdiction. I am not responsible for your actions.

# Bugs
[Bugtracker](https://github.com/tastytea/seriespl/issues) on GitHub or via [E-Mail](mailto:bugs Ⓐ tastytea.de)
