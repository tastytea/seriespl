# Use clang if available, else gcc
ifneq ($(shell which clang++ 2> /dev/null),)
	CXX = clang++
else
	CXX = g++
endif

LDLIBS   = -lcurl -lconfig++
CXXFLAGS = -O2 -pipe -Wall -std=c++11 -mtune=native
NAME = seriespl
PREFIX = /usr/local

# arm-g++ doesn't seem to support -march=native, despite saying so
ifneq (,$(findstring arm,$(CXX)))
	CXXFLAGS := $(filter-out -mtune=native,$(CXXFLAGS))
endif

.PHONY: all
all: dirs $(NAME) man

.PHONY: dirs
dirs: bin obj

$(NAME): bin/$(NAME)

.PHONY: man
man: man/man1/$(NAME).1

bin obj:
	mkdir -p bin
	mkdir -p obj

# $@ = left side of :, $< = first element right of : (%.cpp)
obj/%.o: src/%.cpp $(wildcard src/*.hpp)
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c -o $@ $<

# $^ = right side of :
bin/$(NAME): $(patsubst %.cpp, obj/%.o, $(notdir $(wildcard src/*.cpp)))
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) $(LDLIBS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o bin/$(NAME) $^
	strip --strip-all bin/$(NAME)

man/man1/$(NAME).1: src/main.cpp
	doxygen > /dev/null

.PHONY: install
install: all
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	install -m 0755 bin/$(NAME) $(DESTDIR)/$(PREFIX)/bin
	mkdir -p $(DESTDIR)/$(PREFIX)/share/man/man1
	install -m 0644 man/man1/$(NAME).1 $(DESTDIR)/$(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)/$(PREFIX)/bin/$(NAME)
	rm $(DESTDIR)/$(PREFIX)/share/man/man1/$(NAME).1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)/$(PREFIX)/share/man/man1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)/$(PREFIX)/bin

.PHONY: clean
clean:
	rm -f obj/*.o
	rm -f bin/$(NAME)
	rm -rf man
