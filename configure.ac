dnl {
dnl
dnl  asdig, retreives Autonomous System (AS) infos from repositories via DNS rev-queries 
dnl  Copyright (C) 2016 Jean-Daniel Pauget <jdpauget@rezopole.net>
dnl  
dnl  This program is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU General Public License
dnl  as published by the Free Software Foundation; either version 2
dnl  of the License, or (at your option) any later version.
dnl  
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl  
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
dnl
dnl }

dnl Process this file with autoconf to produce a configure script.
AC_INIT([asdig], [0.0.97], [sysadm@rezopole.net])
AC_CONFIG_SRCDIR([./asdig.cpp])


dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET


dnl quieten some complains from missing files
AM_INIT_AUTOMAKE([foreign])

dnl the magic of additionnal local m4 ?
AC_CONFIG_MACRO_DIR([m4])

dnl autoheader
AM_CONFIG_HEADER(config.h)
AC_DEFINE_UNQUOTED([ASDIG_VERSION], ["$VERSION"], [the release-version of asdig])
AC_SUBST(ASDIG_VERSION,[$VERSION])

dnl Check for tools

AC_PROG_CC
AC_PROG_CXX
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

dnl Check for compiler environment

AC_C_CONST

dnl JD's standard minimum CFLAG
CFLAGS="$CFLAGS -O2 -Wall -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes"
CXXFLAGS="$CXXFLAGS -O2 -Wall -Woverloaded-virtual -Wcast-qual -Wwrite-strings -Wredundant-decls"

dnl how to dns resolv in regard with target-system's flavor
dnl DISCLAIMER : this part hasn't been thoroughly tested, please feel free to contribute !
case "$target" in
    *-freebsd* | *-netbsd* | *-openbsd*)
	RESOLV_CXXFLAGS="-I/usr/local/include/bind"
	RESOLV_LDFLAGS="-L/usr/local/lib/libbind -lbind"
	;;
    *)
	RESOLV_CXXFLAGS=""
	RESOLV_LDFLAGS="-lresolv"
	;;
esac
AC_SUBST(RESOLV_CXXFLAGS)
AC_SUBST(RESOLV_LDFLAGS)


dnl check for mac-osX gcc stl variant
case "$target" in
    *-apple-darwin*)
	dnl some older darwin wanted that .... CXXFLAGS="$CXXFLAGS -stdlib=libstdc++"
	CXXFLAGS="$CXXFLAGS-std=libc++"
	AC_MSG_RESULT([stdlib variant : -stdlib=libstdc++])
	;;
    *)
	:
	;;
esac


dnl  dnl concatenation of include dirs
dnl  CFLAGS="$INCPATH $CFLAGS"
dnl  CXXFLAGS="$INCPATH $CXXFLAGS"

dnl Finally create all the generated files
AC_OUTPUT([
	Makefile
	asdig.1
])


