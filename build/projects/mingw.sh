#!/bin/bash

stop_on_error=1
init=1
dont_install=0
sed_soft="sed"
user=`whoami`

if [ "$user" != "root" ]
then
	echo "You must be root to run this script"
	exit 1
fi

download()
{
	if [ -e $2 ]
	then
		return 0
	else
		wget $1$2
		return $?
	fi
}

mkdir mingw_tmp 2> /dev/null

if [ -d "./mingw_tmp" ]; then
	echo "Start"
else
	echo "Could not create directory mingw_tmp"
	exit 1;
fi

cd mingw_tmp

if [ "$init" -eq "1" ]
then
	# a mirror is also available at mattn.ninex.info and ufo.myexp.de
	# e.g. wget http://ufo.myexp.de/any2deb/any2deb_1.0-2_all.deb
	aptitude install fakeroot alien wget
	download http://www.profv.de/any2deb/ any2deb_1.0-2_all.deb
	dpkg -i any2deb_1.0-2_all.deb

	aptitude install mingw32
	#aptitude install upx-ucl
	aptitude install nsis
fi

echo "========================================"
echo " ZLIB"
echo "========================================"
version="1.2.3"
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ zlib-$version-lib.zip
any2deb mingw32-zlib1-dev $version zlib-$version-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-zlib1-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (zlib)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBPNG"
echo "========================================"
version="1.2.8"
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ libpng-$version-lib.zip
any2deb mingw32-libpng12-dev $version libpng-$version-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libpng12-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libpng)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBJPEG"
echo "========================================"
#beware the -4
#needed for dpkg version string - otherwise the package name would be wrong
version="6b"
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ jpeg-$version-4-lib.zip
any2deb mingw32-libjpeg-dev $version jpeg-$version-4-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libjpeg-dev_6b-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libjpeg)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBICONV"
echo "========================================"
version="1.9.2"
#beware the -1
#needed for dpkg version string - otherwise the package name would be wrong
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ libiconv-$version-1-lib.zip
any2deb mingw32-libiconv-dev $version libiconv-$version-1-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libiconv-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libiconv)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBINTL"
echo "========================================"
version="0.14.4"
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ libintl-$version-lib.zip
any2deb mingw32-libintl-dev $version libintl-$version-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libintl-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libintl)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " FREETYPE"
echo "========================================"
version="2.1.8"
download http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/ freetype-$version-lib.zip
any2deb mingw32-freetype-dev $version freetype-$version-lib.zip /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-freetype-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (freetype)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " SDL"
echo "========================================"
version="1.2.11"
download http://www.libsdl.org/release/ SDL-devel-$version-mingw32.tar.gz
tar -xz -f SDL-devel-$version-mingw32.tar.gz
mkdir -p build-sdl/usr/i586-mingw32msvc
make -C SDL-$version prefix="`pwd`/build-sdl/usr/i586-mingw32msvc/" install >> /dev/null

echo "set prefix to /usr/i586-mingw32msvc"
#TODO: should be i586-mingw32msvc-sdl-config not i386-mingw32msvc-sdl-config
cat build-sdl/usr/i586-mingw32msvc/bin/i386-mingw32msvc-sdl-config | $sed_soft -e 's/^\(prefix\=\).*/\1\/usr\/i586-mingw32msvc/' > build-sdl/usr/i586-mingw32msvc/bin/i386-mingw32msvc-sdl-config

tar -cz --owner=root --group=root -C build-sdl -f mingw32-libsdl1.2-dev-$version.tgz .
fakeroot alien mingw32-libsdl1.2-dev-$version.tgz >> /dev/null

if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libsdl1.2-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (SDL)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " SDLTTF"
echo "========================================"
version="2.0.8"
#beware the -1
#needed for dpkg version string - otherwise the package name would be wrong
download http://cefiro.homelinux.org/resources/files/SDL_ttf/ SDL_ttf-$version-1-i386-mingw32.tar.gz
any2deb mingw32-libsdl-ttf2.0-dev $version SDL_ttf-$version-1-i386-mingw32.tar.gz /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libsdl-ttf2.0-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libsdl-ttf)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBOGG"
echo "========================================"
version="1.1.3"
download http://oss.netfarm.it/mplayer/pkgs/common/ libogg-mingw32-$version.tar.gz
any2deb mingw32-libogg-dev $version libogg-mingw32-$version.tar.gz /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libogg-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libogg)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

echo "========================================"
echo " LIBVORBIS"
echo "========================================"
version="1.1.2"
download http://oss.netfarm.it/mplayer/pkgs/common/ libvorbis-mingw32-$version.tar.gz
any2deb mingw32-libvorbis-dev $version libvorbis-mingw32-$version.tar.gz /usr/i586-mingw32msvc >> /dev/null
if [ "$dont_install" -ne "1" ]
then
	dpkg -i mingw32-libvorbis-dev_$version-2_all.deb
	if [ $? -ne 0 ]
	then
		echo "Fatal error with dpkg (libvorbis)"
		if [ "$stop_on_error" -eq "1" ]
		then
			exit 1
		fi
	fi
fi

if [ "$dont_install" -eq "1" ]
then
	echo "NOTE: No packages were installed - deactivate dont_install if you want to install them automatically"
fi

