######   
#Makefile of all white-box implementations on Armv8 
######
CXX=aarch64-linux-gnu-g++
libs=/usr/lib/gcc-cross/aarch64-linux-gnu/7/libgivaro.a,/usr/lib/gcc-cross/aarch64-linux-gnu/7/libgmp.a,/usr/lib/gcc-cross/aarch64-linux-gnu/7/libgmpxx.a

all: SPNBOX16_WBI WEM16_WBI WARX16_WBI

SPNBOX16_WBI:SPNBOX16_WBI.C
	$(CXX) -O2 -Wall -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 -std=gnu++11 -o SPNBOX16_WBI SPNBOX16_WBI.C -Wl,$(libs) -lgivaro -lgmp -lgmpxx -static


WEM16_WBI:WEM16_WBI.C
	$(CXX) -O2 -Wall -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 -std=gnu++11 -o WEM16_WBI WEM16_WBI.C -Wl,$(libs) -lgivaro -lgmp -lgmpxx -static


WARX16_WBI:WARX16_WBI.C
	$(CXX) -O2 -Wall -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 -std=gnu++11 -o WARX16_WBI WARX16_WBI.C -Wl,$(libs) -lgivaro -lgmp -lgmpxx -static





clean:
	\rm SPNBOX16_WBI WEM16_WBI WARX16_WBI 
