# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Id$

EAPI=6
inherit git-r3
DESCRIPTION="Extracts stream-URLs for entire seasons of tv series from bs.to"
HOMEPAGE="https://github.com/tastytea/seriespl"
EGIT_REPO_URI="git://git.tastytea.de/repositories/seriespl.git"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86 ~arm"
DEPEND=">=net-misc/curl-7.50.3
		>=dev-libs/libconfig-1.5[cxx]"
RDEPEND="${DEPEND}"

src_unpack() {
	git-r3_src_unpack
}

src_install() {
	emake DESTDIR="${D}" PREFIX="${EPREFIX}/usr" install
}
