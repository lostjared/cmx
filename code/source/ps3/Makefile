CXXFLAGS=-g -O2 -Wall 
OBJECTS=source/*.cc
HEADER_PATH=../cmx/include
all:
	ppu-g++ $(OBJECTS) -c $(CXXFLAGS) -I$(HEADER_PATH) -I$(PSL1GHT)/target/include
	ppu-ar cru build/libcmx.a *.o
	ppu-ranlib build/libcmx.a
install: all
	cp -v build/libcmx.a $(PSL1GHT)/target/lib
	cp -rfv ../cmx/include/* $(PSL1GHT)/target/include	
clean:
	rm -rf *.o build/libcmx.a	
	make -C test clean
	make -C mp clean
	make -C debugscr clean
	make -C filebrowser clean
	make -C maptest clean
build_test: install
	make -C test
	make -C mp
	make -C filebrowser
	make -C debugscr
	make -C maptest

