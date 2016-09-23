# Use clang if available, else gcc
ifneq ($(shell which clang++ 2> /dev/null),)
	CC = clang++
else
	CC = g++
endif

LIBS   = -lcurl
CXXFLAGS = -O2 -pipe -Wall -std=c++11 -mtune=native $(LIBS)
SRC    = main.cpp http.cpp
TARGET = seriespl
MAN    = man/man1/seriespl.1
PREFIX = /usr/local

# If on Raspberry Pi
ifeq ($(shell uname -m),armv6l)
	CXXFLAGS = -Os -pipe -Wall -std=c++11
endif

all: $(TARGET) $(MAN)

$(TARGET): $(SRC)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(SRC)
	strip --strip-all $(TARGET)

$(MAN): $(SRC)
	doxygen > /dev/null

.PHONY: install
install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	install -m 0755 $(TARGET) $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/man/man1
	install -m 0644 $(MAN) $(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall: $(TARGET)
	rm $(PREFIX)/bin/$(TARGET)
	rm $(PREFIX)/share/$(MAN)
	rmdir --parents --ignore-fail-on-non-empty $(PREFIX)/share/man/man1
	rmdir --parents --ignore-fail-on-non-empty $(PREFIX)/bin

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -rf man
