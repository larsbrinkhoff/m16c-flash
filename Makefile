CFLAGS+=-g -Wall -pedantic
LFLAGS+=

all: m16c-flash

clean:
	rm ccomport.o
	rm cpage.o
	rm cmotfile.o
	rm cm16flash.o
	rm m16c-flash

ccomport.o: ccomport.cxx ccomport.h
	g++ $(CFLAGS) ccomport.cxx -c -o ccomport.o 

cpage.o: cpage.cxx cpage.h
	g++ $(CFLAGS) cpage.cxx -c -o cpage.o

cmotfile.o: cmotfile.cxx cmotfile.h
	g++ $(CFLAGS) cmotfile.cxx -c -o cmotfile.o

cm16flash.o: cm16flash.cxx cm16flash.h
	g++ $(CFLAGS) cm16flash.cxx -c -o cm16flash.o

m16c-flash: flashm16.cxx cm16flash.o cmotfile.o cpage.o ccomport.o
	g++ $(CFLAGS) flashm16.cxx cm16flash.o cmotfile.o cpage.o ccomport.o -o m16c-flash

install:
	cp m16c-flash /usr/bin

