CC = gcc
CFLAGS = -O -g -Iinclude $(LDFLAGS) $(CPPFLAGS) -lcrypto
# CFLAGS = -O -g -Iinclude -lcrypto -lssl
LINKS = WTFclient.o


all: WTF
WTF: src/wtf.c WTFserver WTFclient
  $(CC) $(CFLAGS) -o WTF src/wtf.c WTFclient.o helpers.o -pthread

WTFserver: src/WTFserver.c helpers
  $(CC) $(CFLAGS) -o WTFserver src/WTFserver.c helpers.o -pthread

WTFclient: src/WTFclient.c include/WTFclient.h
  $(CC) $(CFLAGS) -c src/WTFclient.c

helpers: src/helpers.c include/helpers.h
  $(CC) $(CFLAGS) -c src/helpers.c

clean:
  rm -f WTF
  rm -f WTFserver
  rm -f *.o
  # rm -f .configure
    rm -rf *.dSYM
