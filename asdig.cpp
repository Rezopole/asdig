
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
#include <map>
#include <iomanip>

#include "level3addr.h"
#include "config.h"

using namespace std;

const char* nstype_to_s (int ns_type) {
  switch (ns_type) {
    case T_A:          return "T_A";
    case T_NS:         return "T_NS";
    case T_MD:         return "T_MD";
    case T_MF:         return "T_MF";
    case T_CNAME:      return "T_CNAME";
    case T_SOA:        return "T_SOA";
    case T_MB:         return "T_MB";
    case T_MG:         return "T_MG";
    case T_MR:         return "T_MR";
    case T_NULL:       return "T_NULL";
    case T_WKS:        return "T_WKS";
    case T_PTR:        return "T_PTR";
    case T_HINFO:      return "T_HINFO";
    case T_MINFO:      return "T_MINFO";
    case T_MX:         return "T_MX";
    case T_TXT:        return "T_TXT";
    case T_RP:         return "T_RP";
    case T_AFSDB:      return "T_AFSDB";
    case T_X25:        return "T_X25";
    case T_ISDN:       return "T_ISDN";
    case T_RT:         return "T_RT";
    case T_NSAP:       return "T_NSAP";
    case T_NSAP_PTR:   return "T_NSAP_PTR";
    case T_SIG:        return "T_SIG";
    case T_KEY:        return "T_KEY";
    case T_PX:         return "T_PX";
    case T_GPOS:       return "T_GPOS";
    case T_AAAA:       return "T_AAAA";
    case T_LOC:        return "T_LOC";
    case T_NXT:        return "T_NXT";
    case T_EID:        return "T_EID";
    case T_NIMLOC:     return "T_NIMLOC";
    case T_SRV:        return "T_SRV";
    case T_ATMA:       return "T_ATMA";
    case T_NAPTR:      return "T_NAPTR";
    case T_KX:         return "T_KX";
    case T_CERT:       return "T_CERT";
    case T_A6:         return "T_A6";
    case T_DNAME:      return "T_DNAME";
    case T_SINK:       return "T_SINK";
    case T_OPT:        return "T_OPT";
    case T_APL:        return "T_APL";
    case T_DS:         return "T_DS";
    case T_SSHFP:      return "T_SSHFP";
    case T_IPSECKEY:   return "T_IPSECKEY";
    case T_RRSIG:      return "T_RRSIG";
    case T_NSEC:       return "T_NSEC";
    case T_DNSKEY:     return "T_DNSKEY";
    case T_DHCID:      return "T_DHCID";
    case T_NSEC3:      return "T_NSEC3";
    case T_NSEC3PARAM: return "T_NSEC3PARAM";
    case T_TLSA:       return "T_TLSA";
    case T_SMIMEA:     return "T_SMIMEA";
    case T_HIP:        return "T_HIP";
    case T_NINFO:      return "T_NINFO";
    case T_RKEY:       return "T_RKEY";
    case T_TALINK:     return "T_TALINK";
    case T_CDS:        return "T_CDS";
    case T_CDNSKEY:    return "T_CDNSKEY";
    case T_OPENPGPKEY: return "T_OPENPGPKEY";
    case T_CSYNC:      return "T_CSYNC";
    case T_SPF:        return "T_SPF";
    case T_UINFO:      return "T_UINFO";
    case T_UID:        return "T_UID";
    case T_GID:        return "T_GID";
    case T_UNSPEC:     return "T_UNSPEC";
    case T_NID:        return "T_NID";
    case T_L32:        return "T_L32";
    case T_L64:        return "T_L64";
    case T_LP:         return "T_LP";
    case T_EUI48:      return "T_EUI48";
    case T_EUI64:      return "T_EUI64";
    case T_TKEY:       return "T_TKEY";
    case T_TSIG:       return "T_TSIG";
    case T_IXFR:       return "T_IXFR";
    case T_AXFR:       return "T_AXFR";
    case T_MAILB:      return "T_MAILB";
    case T_MAILA:      return "T_MAILA";
    case T_ANY:        return "T_ANY";
    case T_URI:        return "T_URI";
    case T_CAA:        return "T_CAA";
    case T_AVC:        return "T_AVC";
    case T_TA:         return "T_TA";
    case T_DLV:        return "T_DLV";
    default:	       return "T_UNKONWN";
  }
  return "T_UNKONWN";
}



// the multipmap is used for sorting by prefix size

int resp_query (const char * query, multimap<int,string> &lout, const char * nameserver=NULL) {

    unsigned char answer [NS_PACKETSZ+10], *comp_dn;
    char uncompressed[256];


    if ((_res.options & RES_INIT) == 0) {
	if (res_init() < 0) {
	    lout.insert (pair<int,string>(0, "DNS_ERROR"));
	    cerr << "res_init: failed" << endl;
	    return 1;
	}
	// _res.options |= RES_USEVC;   someday would use tcp rather than udp ?
    }
    
    if(nameserver != NULL) {
	_res.nscount = 1;
	_res.nsaddr_list[0].sin_family = AF_INET;
	_res.nsaddr_list[0].sin_port = htons (53);
	inet_aton (nameserver, &(_res.nsaddr_list[0].sin_addr));
    }

    memset(answer, 0, PACKETSZ+10);
    int r = res_query (query, ns_c_in, ns_t_txt, (unsigned char *)answer, NS_PACKETSZ);
    // int r = res_query (query, ns_c_in, ns_t_txt, (unsigned char *)answer, 30000);

    if (r < 0) {
	switch (h_errno) {
	    case HOST_NOT_FOUND:   /* Authoritative Answer Host not found */
		lout.insert (pair<int,string>(0, "HOST_NOT_FOUND"));
		return 0;
	    case TRY_AGAIN:        /* Non-Authoritative Host not found, or SERVERFAIL */
		lout.insert (pair<int,string>(0, "DNS_ERROR"));
		cerr << "res_query: " << query << " TRY_AGAIN" << endl;
		return 1;
	    case NO_RECOVERY:      /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
		lout.insert (pair<int,string>(0, "DNS_ERROR"));
		cerr << "res_query: " << query << "NO_RECOVERY:" << endl;
		return 1;
	    case NO_DATA:          /* Valid name, no data record of requested type */
		lout.insert (pair<int,string>(0, "NO_DATA"));
		return 0;
	    default:
		lout.insert (pair<int,string>(0, "DNS_ERROR"));
		cerr << "res_query: " << query << " error no:" << h_errno << endl;
		return 1;

	}
	int e = h_errno;
	cerr << "res_query error : " << e << " " << strerror(e) << endl;
	return -1;
    }

    HEADER *ns_header = (HEADER *) answer;

    comp_dn = answer + sizeof(HEADER);	// from /usr/include/arpa/nameser_compat.h
    int processed_answers = 0,
	nb_answers = ntohs(ns_header->ancount);


// this part is for hopping above the querry, repeated in the answer ...
    int uncompressed_len = dn_expand(answer, answer + r, comp_dn, uncompressed, sizeof(uncompressed));
    if (uncompressed_len < 0) {
	lout.insert (pair<int,string>(0, "DNS_ERROR"));
	cerr << "dn_expand: " << query << " failed" << endl;
	return 1;
    }

    comp_dn += uncompressed_len;

    int ns_type;
    GETSHORT(ns_type, comp_dn);	    // from /usr/include/arpa/nameser_compat.h
    if (ns_type != T_TXT) {
	lout.insert (pair<int,string>(0, "DNS_ERROR"));
	cerr << "dn_expand+GETSHORT: answer is not T_TXT, got " << nstype_to_s (ns_type) << "(" << ns_type << ")" << endl;
	return 1;
    }

    comp_dn += INT16SZ;	// let's move forward ...


    while ((processed_answers < nb_answers) && (comp_dn < answer + r)) {

	if (DEBUG_DNS_BUFFER) {
	    unsigned char *p;
	    int nn;
	    for (p = comp_dn, nn=0 ; (nn <300) && (p < answer + r) ; p++, nn++) {
		cerr << setw(2) << setbase(16) << (unsigned int)(*p) << " ";
	    }
	    cerr << endl;
	    for (p = comp_dn, nn=0 ; (nn <300) && (p < answer + r) ; p++, nn++) {
		cerr << " " << ((*p >= 32 ) ? (char)*p : '.') << " ";
	    }
	    cerr << endl;
	    cerr << endl;
	    cerr << endl;
	}

	uncompressed_len = dn_expand(answer, answer + r, comp_dn, uncompressed, sizeof(uncompressed));
	if (uncompressed_len < 0) {
	    lout.insert (pair<int,string>(0, "DNS_ERROR"));
	    cerr << "dn_expand_2: " << query << " failed" << endl;
	    return 1;
	}

	comp_dn += uncompressed_len;
	GETSHORT(ns_type, comp_dn);     // from /usr/include/arpa/nameser_compat.h
	if (ns_type != T_TXT) {
	    lout.insert (pair<int,string>(0, "DNS_ERROR"));
	    cerr << "dn_expand+GETSHORT_2 : answer is not T_TXT, got " << nstype_to_s (ns_type) << "(" << ns_type << ")" << endl;
	    return 1;
	}
	
	comp_dn += INT16SZ; // let's move forward : ns_type
	comp_dn += INT32SZ; // let's move forward : ttl
	size_t ns_size;
	GETSHORT(ns_size, comp_dn);
	size_t slen = (size_t)(*comp_dn);
	if (slen+1 != ns_size) {
	    lout.insert (pair<int,string>(0, "DNS_ERROR"));
	    cerr << "dns answer size inconsistency : " << slen << "+1 != " << ns_size << endl;
	    return 1;
	}
	if (ns_size == 0) {
	    lout.insert (pair<int,string>(0, "DNS_ERROR"));
	    cerr << "dns answer size inconsistency_2 : " << slen << "+1 != " << ns_size << endl;
	    return 1;
	}

	comp_dn ++;

	string s;

	for (size_t i=0 ; (i<slen) && (*comp_dn != 0) ; i++)
	    s += *comp_dn++;

    // typical answer is :
    // 199422 | 77.95.64.0/23 | FR | ripe | 2013-01-24 | REZOPOLE
	int prefsize = 0;
	{	size_t p,q;
	    p = s.find('|');
	    if (p != string::npos) {
		p = s.find('/', p+1);
		if (p != string::npos) {
		    p++;
		    q = s.find('|', p);
		    if (q != string::npos) {
			prefsize = 129 - atoi(s.substr(p,q-p).c_str());
		    } else {
			prefsize = 129 - atoi(s.substr(p).c_str());
		    }
		}
	    }
	}

	lout.insert (pair<int,string>(prefsize, s));
	processed_answers ++;
    }

    return 0;
}

using namespace rzpnet;

int main (int nb, char ** cmde) {

    int i;
    bool therewassomeerror = false;
    bool debug_query = false;
    const char 
	*rzpas = "asn.rezopole.net",
	*rzpv4 = "origin.asn.rezopole.net.",
	*rzpv6 = "origin6.asn.rezopole.net.",
	*cymruas = NULL,
	*cymruv4 = "origin.asn.cymru.com.",
	*cymruv6 = "origin6.asn.cymru.com.",
	*suffixas = rzpas,
	*suffixv4 = rzpv4,
	*suffixv6 = rzpv6,
	*nameserver = NULL;

    // first we look for nameserver
    bool dnsserver_multiple_change = false;
    for (i=1 ; i<nb ; i++) {
	if (cmde[i][0] == '@') {
	    if (nameserver != NULL) dnsserver_multiple_change = true;
	    nameserver = cmde[i] + 1;
	    continue;
	}
    }
    if (dnsserver_multiple_change)
	cerr << "WARNING: multiple dns servers given, will only use " << nameserver << " for dns-lookups" << endl;

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
		cout << "asdig [-rezopole] [-cymru] [-suffixv4=my.ipv4.suffix.com] [@IP.my.ser.ver]" << endl
		     << "      [-suffixv6=my.ipv6.suffix.net] IP|AS#### [ ... IP|AS### ... ] [-d|--debug]" << endl;
		return 0;
	    } else if (	    (strcmp (cmde[i], "-version") == 0)
			||  (strcmp (cmde[i], "--version") == 0)
			||  (strcmp (cmde[i], "-v") == 0)
		){
		cout << "asdig version " << ASDIG_VERSION << endl;
		return 0;
	    } else if (	(strcmp (cmde[i], "-d") == 0)
			|| (strcmp (cmde[i], "--debug") == 0)
		) {
		debug_query = true;
		continue;
	    } else {
		cerr << "unknown option \"" << cmde[i] << "\"" << endl;
		continue;
	    }
	} else if (cmde[i][0] == '@') {
	    continue;
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

	if (debug_query) {
	    cerr << "dig TXT " << q.str().c_str();
	    if (nameserver != NULL)
		cerr << " @" << nameserver;
	    cerr << endl;
	}

	multimap<int,string> l;
	if (  resp_query (q.str().c_str(), l, nameserver)  )
	    therewassomeerror = true;
	multimap<int, string>::iterator li;
	for (li=l.begin() ; li!=l.end() ; li++) {
	    cout << li->second << endl;
	}
    }

    if (therewassomeerror)
	return 1;

    return 0;
}

