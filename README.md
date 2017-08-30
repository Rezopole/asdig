# asdig
simple unix util for retrieving AS (autonomous system) match from IP(v6) published
by services like Rezopole GoASMap (http://github.com/rezopole/goasmap) or cymru.

## Installation
`> make && chmod 755 ./asdig && cp ./asdig /usr/local/bin`

## Usage
how to retrieve the AS that publish a prefix for 134.214.100.6 ?

```bash
$ asdig 134.214.100.6`
`2060 | 134.214.0.0/16 |`
 ```

AS no 2060 does via the 134.214.0.0/16 subnet route.

who publish some IPv6 route for www.nasa.gov ?

```bash
$ host www.nasa.gov
www.nasa.gov is an alias for www.nasawestprime.com.
www.nasawestprime.com is an alias for iznasa.hs.llnwd.net.
iznasa.hs.llnwd.net has address 95.140.230.145
iznasa.hs.llnwd.net has IPv6 address 2a02:3d0:623:a000::8008

$ asdig 2a02:3d0:623:a000::8008
22822 | 2a02:3d0::/32 |
 ```

AS 22822 does via the 2a02:3d0::/32 subnet announce.


## Prerequisites
a C++ compiler and the resolv library with it's headers.

## Authors
The Rezopole team (https://www.rezopole.net/en/about-rezopole/team)

