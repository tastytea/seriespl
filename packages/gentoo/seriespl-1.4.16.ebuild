# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id$

EAPI=6
DESCRIPTION="Extracts stream-URLs for entire seasons of tv series from bs.to"
HOMEPAGE="https://github.com/tastytea/seriespl"
SRC_URI="${HOMEPAGE}/archive/${PV}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86 ~arm"
DEPEND=">=net-misc/curl-7.50.3
		>=dev-libs/libconfig-1.5[cxx]
		>=app-doc/doxygen-1.8.11-r2"
RDEPEND=">=net-misc/curl-7.50.3
		 >=dev-libs/libconfig-1.5[cxx]
		 >=net-misc/youtube-dl-2016.09.19"

src_install() {
	emake DESTDIR="${D}" PREFIX="${EPREFIX}/usr" install
}
