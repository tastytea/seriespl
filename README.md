**seriespl** fetches stream-URLs from bs.to. These URLs can then be further processed by e.g. [youtube-dl](https://rg3.github.com/youtube-dl/) or [mpv](https://mpv.io/).

# Install
## Dependencies
 * Linux (May work on other OS, not tested)
 * Newer than ancient clang or gcc (must support -std=c++11)
 * [poco](http://pocoproject.org/)
 * [make](https://www.gnu.org/software/make/make.html)

## Compile
    make

# Usage
The only working streaming provider is streamcloud at the moment. The only output format is raw.

Download all episodes of South Park Season 1:
    seriespl http://bs.to/serie/South-Park/1 | youtube-dl -a -

Watch all episodes of South Park Season 1:
    seriespl http://bs.to/serie/South-Park/1 | mpv --playlist=-

# Legal aspects
Please inform yourself if using this program is legal under your jurisdiction. I am not responsible for your actions.

# Bugs
[Bugtracker](https://github.com/tastytea/seriespl/issues) on GitHub or via [E-Mail](mailto:bugs Ⓐ tastytea.de)
