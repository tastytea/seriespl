#!/bin/sh

if [ "$(basename ${PWD})" != "packages" ]; then
	echo "ERROR: Wrong directory" >&2
fi

if [ -z ${1} ]; then
	version="$(grep "version\[\] =" ../src/global.hpp | cut -d\" -f2)"
else
	version="${1}"
fi

wget https://github.com/tastytea/seriespl/archive/${version}.tar.gz -O seriespl_${version}.orig.tar.gz
tar -xf seriespl_${version}.orig.tar.gz
cd seriespl-${version}
cp -r packages/debian ./
dpkg-buildpackage -us -uc
