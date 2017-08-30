
asdig: asdig.cpp
	g++ -Wall -o asdig asdig.cpp -lresolv

clean:
	rm -f asdig

.PHONY test: vimtest

.PHONY vimtest: asdig
	./asdig					134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -cumry				134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -cymru				134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -rezopole			134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -suffixv4=tagada.tsointsoin.fr	134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008
	./asdig -suffixv6=tagada.tsointsoin.fr	134.214.100.6  10.10.12.64  2a02:3d0:623:a000::8008

