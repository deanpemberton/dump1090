#
# When building a package or installing otherwise in the system, make
# sure that the variable PREFIX is defined, e.g. make PREFIX=/usr/local
#
PROGNAME=dump1090

ifdef PREFIX
BINDIR=$(PREFIX)/bin
SHAREDIR=$(PREFIX)/share/$(PROGNAME)
EXTRACFLAGS=-DHTMLPATH=\"$(SHAREDIR)\"
endif

#CFLAGS=-O2 -g -Wall -W `pkg-config --cflags librtlsdr`
CFLAGS=-O2 -g -Wall -W `pkg-config --cflags librtlsdr` `mysql_config --cflags --libs`
#LIBS=`pkg-config --libs librtlsdr` -lpthread -lm
LIBS=`pkg-config --libs librtlsdr` `mysql_config --libs` -lpthread -lm
CC=gcc


all: dump1090

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRACFLAGS) -c $<

dump1090: dump1090.o anet.o interactive.o mode_ac.o mode_s.o net_io.o mysql.o
	$(CC) -g -o dump1090 dump1090.o anet.o interactive.o mode_ac.o mode_s.o net_io.o mysql.o $(LIBS)

clean:
	rm -f *.o dump1090
