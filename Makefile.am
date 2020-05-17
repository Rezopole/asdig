## {
##
##  asdig, retreives Autonomous System (AS) infos from repositories via DNS rev-queries 
##  Copyright (C) 2016 Jean-Daniel Pauget <jdpauget@rezopole.net>
##  
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
##  
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##  
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
##
## }




ACLOCAL_AMFLAGS = -I m4

vimtest: asdig
	./asdig					134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -cumry				134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -cymru				134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -rezopole			134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -suffixv4=tagada.tsointsoin.fr	134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -suffixv6=tagada.tsointsoin.fr	134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008



bin_PROGRAMS = asdig
man1_MANS = asdig.1

asdig_SOURCES = asdig.cpp  ethertype.h  level3addr.h config.h
asdig_CXXFLAGS = @CXXFLAGS@ @RESOLV_CXXFLAGS@
asdig_LDFLAGS = @RESOLV_LDFLAGS@
		   
clean-local:
	:

distclean-local:
	:
	rm -f m4/*.m4
	rm -f config.h.in~
	rm -rf debian
	rm -f Makefile
	rm -f asdig*.deb

