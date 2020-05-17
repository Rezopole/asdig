
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



#ifndef RZPNETLEVEL3ADDR
#define RZPNETLEVEL3ADDR

#include <ostream>
#include <sstream>

#include <arpa/inet.h>	// inet_pton
#include <stdlib.h>	// atoi

#include "ethertype.h"

namespace rzpnet {

    using namespace std;
    using namespace rzpnet;

// --------- Level3Addr ------------------------------------------------------------------------------------------------------------------------

class Level3Addr;
ostream &operator<< (ostream &out, const Level3Addr &a);
Level3Addr l3mask (int nb);

class Level3Addr {
  private:
    static Level3Addr mask[128];
    static bool mask_not_initialized;

  public:
    TEthertype t;
    unsigned char b[16];

    bool valid (void) const {
	return ((t == TETHER_IPV4) || (t == TETHER_IPV6));
    }

    Level3Addr (void)  : t(TETHER_UNKNOWN) {
	size_t i;
	for (i=0 ; i<16 ; i++) b[i] = 0;
    }

    void applymask (Level3Addr const &mask) {
	size_t i;
	for (i=0 ; i<16 ; i++) b[i] &= mask.b[i];
    }

    inline void init_mask (void) {
	if (mask_not_initialized) {
	    size_t i;
	    for (i=0 ; i<128 ; i++) {
		mask[i] = l3mask(i);
	    }
	    mask_not_initialized = false;
	}
    }

    void applymask (int masklen) {
	init_mask();
	if (masklen < 0) masklen = 0;
	if (masklen >=128) return;
	applymask (mask[masklen]);
    }

    Level3Addr (TEthertype proposed_type, const u_char* p) : t(TETHER_UNKNOWN) {
	size_t i;
	switch (proposed_type) {
	  case TETHER_IPV4:
	    for (i=0 ; i<4 ; i++) b[i] = *p++;
	    for ( ; i<16 ; i++) b[i] = 0;
	    t = TETHER_IPV4;
	    break;
	  case TETHER_IPV6:
	    for (i=0 ; i<16 ; i++) b[i] = *p++;
	    t = TETHER_IPV6;
	    break;
	  default:
	    for (i=0 ; i<16 ; i++) b[i] = 0;
	    t = TETHER_UNKNOWN;
	}
    }

    Level3Addr (TEthertype proposed_type, string const &s) : t(TETHER_UNKNOWN) {
	size_t p, i;
	switch (proposed_type) {
	  case TETHER_IPV4:
	    b[0] = atoi (s.c_str());
	    p = s.find('.');
	    if (p!=string::npos) {
		b[1] = atoi (s.substr(p+1).c_str());
		p = s.find('.', p+1);
		if (p!=string::npos) {
		    b[2] = atoi (s.substr(p+1).c_str());
		    p = s.find('.', p+1);
		    if (p!=string::npos) {
			b[3] = atoi (s.substr(p+1).c_str());
			t = TETHER_IPV4;
		    }
		}
	    }
	    for (i=4 ; i<16 ; i++) b[i] = 0;	// we pad with zeros for comparisons
	    break;

	  case TETHER_IPV6:
	    p = s.find_first_not_of ("0123456789abcdefABCDEF:");
	    if (p == string::npos) {
		if (inet_pton(AF_INET6, s.c_str(), &(b[0])) == 1)
		    t = TETHER_IPV6;
	    } else {
		if (inet_pton(AF_INET6, s.substr(0,p).c_str(), &(b[0])) == 1)
		    t = TETHER_IPV6;
	    }
	    break;

	  default:
	    for (i=0 ; i<16 ; i++) b[i] = 0;
	    t = TETHER_UNKNOWN;
	}
    }

    Level3Addr (Level3Addr const &r) : t(r.t) {
	size_t i;
	for (i=0 ; i<16 ; i++) b[i] = r.b[i];
    }
    bool operator< (const Level3Addr &a) const {
	if (t < a.t) {
	    return true;
	} else if (a.t < t) {
	    return false;
	} else {	// we have equal types
	    size_t i;
	    for (i=0 ; i<16 ; i++) {
		if (b[i] < a.b[i]) {
		    return true;
		} else if (a.b[i] < b[i]) {
		    return false;
		}
	    }
	}
	return false;	// we should get there only if a == *this
    }

    bool operator== (const Level3Addr &a) const {
	if (t != a.t) {
	    return false;
	}
	// we have equal types
	size_t i;
	for (i=0 ; i<16 ; i++) {
	    if (b[i] != a.b[i]) {
		return false;
	    }
	}
	return true;	// we should get there only if a == *this
    }

    int rev_arpa_radix (ostream & cout) {
	int i;
	const char *h = "0123456789abcdef";
	switch (t) {
	    case TETHER_IPV4:
		for (i=3 ; i>=0 ; i--) {
		    cout << (int) b[i];
		    if (i>0)
		    cout << '.';
		}
		break;
	    case TETHER_IPV6:
		for (i=15 ; i>=0 ; i--) {
		    cout << h[ b[i] & 0xf ] << '.' << h[ (b[i] & 0xf0)>>4 ];
		    if (i>0)
		    cout << '.';
		}
		break;
	    default:
		return -1;
	}
	return 0;
    }
};

bool Level3Addr::mask_not_initialized = true;
Level3Addr Level3Addr::mask[128];

ostream &operator<< (ostream &out, const Level3Addr &a) {
    stringstream s;
    switch (a.t) {
      case TETHER_IPV4:
	s << (unsigned int)a.b[0] << '.' << (unsigned int)a.b[1] << '.' << (unsigned int)a.b[2] << '.' << (unsigned int)a.b[3];
	return out << s.str();
	break;
      case TETHER_IPV6:
	char str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &(a.b[0]), str, INET6_ADDRSTRLEN);
	return out << str;
	break;
      default:
	break;
    }
    return out << "invalidL3addr";
}

Level3Addr l3mask (int nb) {
    Level3Addr mask;
    mask.t = TETHER_IPV6;

    int bit = 0;

    for (bit=0 ; bit<128 ; bit++) {
	size_t i = bit / 8;
	size_t n = 7-(bit % 8);

	if (bit < nb) {	// we set one bit to 1
	    mask.b[i] |= (1 << n);
	} else {	// we set one bit to 0
	    mask.b[i] &= ~(1 << n);
	}
    }
    return mask;
}


} // namespace rzp_net

#endif // RZPNETLEVEL3ADDR

