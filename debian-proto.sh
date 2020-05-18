#!/bin/bash

    OURPACKAGENAME=$( grep '^AC_INIT' ./configure.ac | cut -d'[' -f2 | cut -d']' -f1 )
    OURPACKAGEVERSION=$( grep '^AC_INIT' ./configure.ac | cut -d'[' -f3 | cut -d']' -f1 )
    RELEASE=$( lsb_release -c -s )

    OURBUILDDIR=$( mktemp -d /tmp/"${OURPACKAGENAME}"_build_XXXXX  )
    FINALDEST=$( pwd )

    DEBHELPERVERSION=$( dpkg -s debhelper | grep '^Version:' | sed 's/^.*sion: //' | cut -d. -f1 )

    if [ ! -d "${OURBUILDDIR}" ]
    then
	echo 'mktemp -d /tmp/'"${OURPACKAGENAME}"'_build_XXXXX failed !!'
	exit 1
    else
	echo "will build  ${OURPACKAGENAME}-${OURPACKAGEVERSION}  in  ${OURBUILDDIR}  ..."
	echo '-----------------------------------------------------------------'
    fi

    time ( 

	mkdir "${OURBUILDDIR}/${OURPACKAGENAME}-${OURPACKAGEVERSION}"
	tar -cpf - . | (cd "${OURBUILDDIR}/${OURPACKAGENAME}-${OURPACKAGEVERSION}" && tar -xpvf -)

	(   cd "${OURBUILDDIR}/${OURPACKAGENAME}-${OURPACKAGEVERSION}" || exit 1
	    autoall
	    if [ "$DEBHELPERVERSION" -le 10 ]
	    then
	        cp -a debian-proto-9 debian
	    else
		cp -a debian-proto debian
	    fi

	    sed -i 's/RELEASE-TARGET/'"${RELEASE}"'/g' debian/changelog

	    DEBEMAIL="sysadm@rezopole.net" DEBFULLNAME="Jean-Daniel Pauget" dh_make -y --single --createorig
	    pandoc -f markdown -t plain --wrap=none README.md -o debian/README
	    cd debian && debuild -us -uc
	)
    )
    cp "${OURBUILDDIR}"/*deb "${FINALDEST}"
    echo '-----------------------------------------------------------------'
    ls *deb | while read FNAME
    do
	if echo "${FNAME}" | grep -- "-${RELEASE}\.deb$" 2>&1 > /dev/null
	then
	    ls -l "${FNAME}"
	else
	    FTARGET=$( echo "${FNAME}" | sed 's/\.deb$/'"-${RELEASE}.deb"'/' )
	    mv "${FNAME}" "${FTARGET}"
	    ls -l "${FTARGET}"
	fi
    done

