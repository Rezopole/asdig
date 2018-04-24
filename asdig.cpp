
// {
//
//  asdig, retreives Autonomous System (AS) infos from repositories via DNS rev-queries 
//  Copyright (C) 2016 Jean-Daniel Pauget <jdpauget@rezopole.net>
//  
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// }

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>  // h_errno ???

#include <errno.h>  // errno ...
#include <string.h> //	strerror


#include <iostream>
#include <sstream>
#include <iomanip>

#include "level3addr.h"

using namespace std;

int resp_query (const char * query) {

    unsigned char answer [NS_PACKETSZ+10];


    if ((_res.options & RES_INIT) == 0) res_init();
    // _res.options |= RES_USEVC;   someday would use tcp rather than udp ?


    int r = res_query (query, ns_c_in, ns_t_txt, (unsigned char *)answer, NS_PACKETSZ);
    // int r = res_query (query, ns_c_in, ns_t_txt, (unsigned char *)answer, 30000);

    if (r < 0) {
	switch (h_errno) {
	    case HOST_NOT_FOUND:   /* Authoritative Answer Host not found */
		cout << "HOST_NOT_FOUND" << endl;
		return 0;
	    case TRY_AGAIN:        /* Non-Authoritative Host not found, or SERVERFAIL */
		cout << "DNS_ERROR" << endl;
		cerr << "res_query: " << query << " TRY_AGAIN" << endl;
		return 1;
	    case NO_RECOVERY:      /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
		cout << "DNS_ERROR" << endl;
		cerr << "res_query: " << query << "NO_RECOVERY:" << endl;
		return 1;
	    case NO_DATA:          /* Valid name, no data record of requested type */
		cout << "NO_DATA" << endl;
		return 0;
	    default:
		cout << "DNS_ERROR" << endl;
		cerr << "res_query: " << query << " error no:" << h_errno << endl;
		return 1;

	}
	int e = h_errno;
	cerr << "res_query error : " << e << " " << strerror(e) << endl;
	return -1;
    }

    ns_msg handle;  /* handle for response message */
    if (ns_initparse(answer, r, &handle) < 0) {
	int e = errno;
	cerr << "ns_initparse " << strerror(e) << endl;
        return -1;
    }

    ns_rr rr;   /* expanded resource record */
    for (int rrnum=0 ; rrnum < ns_msg_count(handle, ns_s_an) ; rrnum++) {
        if (ns_parserr(&handle, ns_s_an, rrnum, &rr) != 0) {
	    int e = errno;
	    cerr << "ns_parserr error : " << strerror (e) << endl;
        }
        if (ns_rr_type(rr) == ns_t_txt) {

	    int l = (int)(*ns_rr_rdata(rr)); // the length of the TXT field (? JD)
	    const unsigned char *pmax = ns_msg_base(handle) + ns_msg_size(handle);
	    const unsigned char *p = ns_rr_rdata(rr) + 1;
	    for (int i=0 ; (i<l) && (*p!=0) && (p<pmax) ; i++, p++) {
		cout << (char)(*p);
	    }

	    cout << endl;
	}
    }

    return 0;
}

using namespace rzpnet;

int main (int nb, char ** cmde) {

    int i;
    bool therewassomeerror = false;
    const char 
	*rzpas = "asn.rezopole.net",
	*rzpv4 = "origin.asn.rezopole.net.",
	*rzpv6 = "origin6.asn.rezopole.net.",
	*cymruas = NULL,
	*cymruv4 = "origin.asn.cymru.com.",
	*cymruv6 = "origin6.asn.cymru.com.",
	*suffixas = rzpas,
	*suffixv4 = rzpv4,
	*suffixv6 = rzpv6;

    for (i=1 ; i<nb ; i++) {
	if (cmde[i][0] == '-')  {
	    if (strncmp (cmde[i], "-suffixv4=", 10) == 0) {
		suffixv4 = cmde[i] + 10;
		continue;
	    } else if (strncmp (cmde[i], "-suffixv6=", 10) == 0) {
		suffixv6 = cmde[i] + 10;
		continue;
	    } else if (strcmp (cmde[i], "-rezopole") == 0) {
		suffixas = rzpas;
		suffixv4 = rzpv4;
		suffixv6 = rzpv6;
		continue;
	    } else if (strcmp (cmde[i], "-cymru") == 0) {
		suffixas = cymruas;
		suffixv4 = cymruv4;
		suffixv6 = cymruv6;
		continue;
	    } else if (	    (strcmp (cmde[i], "-help") == 0)
			||  (strcmp (cmde[i], "--help") == 0)
			||  (strcmp (cmde[i], "-h") == 0)
		) {
		cout << "asdig [-rezopole] [-cymru] [-suffixv4=my.ipv4.suffix.com]" << endl
		     << "      [-suffixv6=my.ipv6.suffix.net] IP|AS#### [ ... IP|AS### ... ]" << endl;
		return 0;
	    } else if (strcmp (cmde[i], "-version") == 0) {
		cout << "asdig version 0.9" << endl;
		return 0;
	    } else {
		cerr << "unkown option \"" << cmde[i] << "\"" << endl;
		continue;
	    }
	}

	stringstream q;
	Level3Addr adr;
	if ((strncmp ("as", cmde[i], 2) == 0) || (strncmp ("AS", cmde[i], 2) == 0)) {
	    q << "as" << (cmde[i]+2);
	    if (suffixas == NULL) {
		cerr << "falling back on rezopole for as-lookup" << endl;
		suffixas = rzpas;
	    }
	    q << '.' << suffixas;
	} else {
	    if (strchr(cmde[i], ':') != NULL) { // we have an IPv6
		adr = Level3Addr (TETHER_IPV6, string(cmde[i]));
	    } else if (strchr(cmde[i], '.') != NULL) { // we now assume IPv4
		adr = Level3Addr (TETHER_IPV4, string(cmde[i]));
	    }

	    adr.rev_arpa_radix (q);
	    switch (adr.t) {
		case TETHER_IPV4:
		    q << '.' << suffixv4;
		    break;
		case TETHER_IPV6:
		    q << '.' << suffixv6;
		    break;
		default:
		    break;
	    }
	}

	if (resp_query (q.str().c_str()))
	    therewassomeerror = true;
    }

    if (therewassomeerror)
	return 1;

    return 0;
}

