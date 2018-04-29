
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




#ifndef RZPNETETHERTYPE
#define RZPNETETHERTYPE

#include <iostream>
#include <string>

#if defined(__OpenBSD__)
#include <net/ethertypes.h>
#else
#include <net/ethernet.h>
#endif

// --------- Ethertype -------------------------------------------------------------------------------------------------------------------------

namespace rzpnet {

    using namespace std;

typedef enum {
    TETHER_IPV4,
    TETHER_IPV6,
    TETHER_ARP,
    TETHER_8023,
    TETHER_8021Q,
    TETHER_MOPRC,
    TETHER_LLC,
    TETHER_AOE,
    TETHER_UNKNOWN,
    TETHER_OTHER
} TEthertype;

class Ethertype {
  public:
    TEthertype ethertype;
    Ethertype () : ethertype (TETHER_UNKNOWN) {};
    Ethertype (const u_short *p) {
	switch (ntohs(*p)) {
	    case ETHERTYPE_IP:		ethertype = TETHER_IPV4;	    break;
	    case ETHERTYPE_ARP:		ethertype = TETHER_ARP;		    break;
	    case ETHERTYPE_VLAN:	ethertype = TETHER_8021Q;	    break;
	    case ETHERTYPE_IPV6:	ethertype = TETHER_IPV6;	    break;
	    default:			ethertype = TETHER_OTHER;	    break;
	}
    }
    Ethertype (const string &s) {
//cerr << "===================" << s.substr (0,20) << endl;
	     if (s.find ("ethertype IPv4") == 0)		ethertype = TETHER_IPV4;
	else if (s.find ("ethertype IPv6") == 0)		ethertype = TETHER_IPV6;
	else if (s.find ("ethertype ARP") == 0)			ethertype = TETHER_ARP;
	else if (s.find ("802.3") == 0)				ethertype = TETHER_8023;
	else if (s.find ("ethertype 802.1Q") == 0)		ethertype = TETHER_8021Q;
	else if (s.find ("LLC") == 0)				ethertype = TETHER_LLC;
	else if (s.find ("ethertype MOP RC") == 0)		ethertype = TETHER_MOPRC;
	else if (s.find ("ethertype AoE") == 0)			ethertype = TETHER_AOE;
	else if (s.find ("ethertype Unknown ") == 0)		ethertype = TETHER_UNKNOWN;
	else							ethertype = TETHER_OTHER;
    }
    Ethertype (const Ethertype &o) : ethertype(o.ethertype) {}
    bool operator< (const Ethertype &a) const {
	return ethertype < a.ethertype;
    }
};
ostream &operator<< (ostream &out, const Ethertype &p) {
    switch (p.ethertype) {
	case TETHER_IPV4:    return out << "IPv4";
	case TETHER_IPV6:    return out << "IPv6";
	case TETHER_ARP:     return out << "ARP";
	case TETHER_8023:    return out << "803.3";
	case TETHER_MOPRC:   return out << "MOP RC";
	case TETHER_AOE:     return out << "AoE";
	case TETHER_8021Q:   return out << "802.1q";
	case TETHER_UNKNOWN: return out << "Unknown";
	case TETHER_OTHER:   return out << "Other";
	default:             return out << "Other";
    }
}


} // namespace rzp_net

#endif //  RZPNETETHERTYPE
