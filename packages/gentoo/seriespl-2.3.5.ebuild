# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id$

EAPI=6
DESCRIPTION="Extracts stream-URLs for entire seasons of tv series from bs.to and others"
HOMEPAGE="https://github.com/tastytea/seriespl"
SRC_URI="${HOMEPAGE}/archive/${PV}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86 ~arm"
IUSE="debug"
DEPEND=">=net-misc/curl-7.50.3
		>=dev-libs/libconfig-1.5[cxx]"
RDEPEND="${DEPEND}"

src_compile() {
	if use debug; then
		emake debug
	else
		emake
	fi
}

src_install() {
	emake DESTDIR="${D}" PREFIX="${EPREFIX}/usr" install
}
