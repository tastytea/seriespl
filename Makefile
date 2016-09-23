# Use clang if available, else gcc
ifneq ($(shell which clang++ 2> /dev/null),)
	CC = clang++
else
	CC = g++
endif

LDLIBS   = -lcurl -lconfig++
CXXFLAGS = -O2 -pipe -Wall -std=c++11 -mtune=native
NAME = seriespl
PREFIX = /usr/local

# If on Raspberry Pi
ifeq ($(shell uname -m),armv6l)
	CXXFLAGS = -Os -pipe -Wall -std=c++11
endif


all: dirs $(NAME) man

dirs: bin obj

$(NAME): bin/$(NAME)

man: man/man1/$(NAME).1

bin obj:
	mkdir -p bin
	mkdir -p obj

# $@ = left side of :, $< = first element right of : (%.cpp)
obj/%.o: src/%.cpp $(wildcard src/*.hpp)
	$(CC) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c -o $@ $<

# $^ = right side of :
bin/$(NAME): $(patsubst %.cpp, obj/%.o, $(notdir $(wildcard src/*.cpp)))
	$(CC) $(CXXFLAGS) $(EXTRA_CXXFLAGS) $(LDLIBS) $(EXTRA_LDFLAGS) -o bin/$(NAME) $^
	strip --strip-all bin/$(NAME)

man/man1/$(NAME).1: src/main.cpp
	doxygen > /dev/null

.PHONY: install
install: all
	mkdir -p $(PREFIX)/bin
	install -m 0755 bin/$(NAME) $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/man/man1
	install -m 0644 man/man1/$(NAME).1 $(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall:
	rm $(PREFIX)/bin/$(NAME)
	rm $(PREFIX)/share/man/man1/$(NAME).1
	rmdir --parents --ignore-fail-on-non-empty $(PREFIX)/share/man/man1
	rmdir --parents --ignore-fail-on-non-empty $(PREFIX)/bin

.PHONY: clean
clean:
	rm -f obj/*.o
	rm -f bin/$(NAME)
	rm -rf man
