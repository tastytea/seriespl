# Use clang if available, else gcc
ifneq ($(shell which clang++ 2> /dev/null),)
	CC = clang++
else
	CC = g++
endif

LIBS   = -lPocoNet -lPocoNetSSL -lPocoFoundation -lpthread
CFLAGS = -O2 -pipe -Wall -std=c++11 -mtune=native
SRC    = main.cpp
TARGET = seriespl
MAN    = man/man1/seriespl.1
PREFIX = /usr/local

# If on Raspberry Pi
ifeq ($(shell uname -m),armv6l)
	CFLAGS = -Os -pipe -Wall -std=c++11
endif

all: $(TARGET) $(MAN)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(SRC)
	strip --strip-all $(TARGET)

$(MAN): $(SRC)
	doxygen > /dev/null

.PHONY: install
install: $(TARGET)
	install -m 0755 $(TARGET) $(PREFIX)/bin
	install -m 0644 $(MAN) $(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall: $(TARGET)
	rm $(PREFIX)/bin/$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -rf man
