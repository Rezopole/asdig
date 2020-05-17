# asdig
simple unix util for retrieving AS (autonomous system) match from IP(v6) published
by services like Rezopole GoASMap (http://github.com/rezopole/goasmap) or cymru.

## Usage
how to retrieve the AS that publish a prefix for 134.214.100.6 ?

```bash
$ asdig 134.214.100.6
2060 | 134.214.0.0/16 | EU | ripe | 1993-09-01 | FR-RENATER
 ```

AS-2060 (RENATER) does via the 134.214.0.0/16 subnet route.

who publish some IPv6 route for www.nasa.gov ?

```bash
$ host www.nasa.gov
www.nasa.gov is an alias for www.nasawestprime.com.
www.nasawestprime.com is an alias for iznasa.hs.llnwd.net.
iznasa.hs.llnwd.net has address 95.140.230.145
iznasa.hs.llnwd.net has IPv6 address 2a02:3d0:623:a000::8008

$ asdig 2a02:3d0:623:a000::8008
22822 | 2a02:3d0::/32 | US | arin | 2001-11-28 | LLNW
 ```

AS-22822 (LLWN) does via the 2a02:3d0::/32 subnet announce.

who is AS-199422 ?

```bash
$ asdig as199422
199422 | FR | ripe | 2013-01-24 | REZOPOLE
 ```

AS-199422 is registered from RIPE to some french organisation (since 2013-01-24) that named it "REZOPOLE".

## Compilation / Installation
the following should bring you to a traditional build tree :
```bash
   libtoolize --force --copy && aclocal && autoheader && automake --add-missing --copy && autoconf ```
```
then the traditional build sequence :
```bash
    ./configure
    make
    make install
```

## Prerequisites
an autoconf / automake build-chain, in order to get a compilable tree.

a C++ compiler and the resolv library with it's headers.

## Authors
The Rezopole team (https://www.rezopole.net/en/about-rezopole/team)

