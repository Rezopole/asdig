#!/bin/sh

	autoall
	cp -a debian-proto debian
        DEBEMAIL="sysadm@rezopole.net" DEBFULLNAME="Jean-Daniel Pauget" dh_make -y --single --createorig
        pandoc -f markdown -t plain --wrap=none README.md -o debian/README
        cd debian && debuild -us -uc

