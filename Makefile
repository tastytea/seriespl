# Makefile for seriespl
NAME     = seriespl
VERSION  = $(shell grep "version\[\] =" src/global.hpp | cut -d\" -f2)
LDLIBS   = -lconfig++ #-lcurl

ifndef CXX
	CXX = c++
endif
ifndef CXXFLAGS
	# -Wno-unused-value is only active for release builds (ttdebug in global.hpp)
	CXXFLAGS = -O2 -pipe -Wall -mtune=native -Wno-unused-value
endif
# Make sure that c++14 is selected
override CXXFLAGS += -std=c++14
# arm-g++ doesn't support -mtune=native
ifneq (,$(findstring arm,$(CXX)))
	CXXFLAGS := $(filter-out -mtune=native,$(CXXFLAGS))
endif
ifndef PREFIX
	PREFIX = /usr/local
endif


.PHONY: all
all: $(NAME) man

.PHONY: $(NAME)
$(NAME): dirs bin/$(NAME)

.PHONY: man
man: man/$(NAME).1

.PHONY: debug
debug: CXXFLAGS += -DDEBUG -g -ggdb
debug: CXXFLAGS := $(filter-out -Wno-unused-value,$(CXXFLAGS))
debug: $(NAME) man

.PHONY: dirs
dirs: obj bin

obj:
	mkdir -p obj

bin:
	mkdir -p bin

# write git commit into global.hpp
src/global.hpp: .git/refs/heads
	sed -i "s/\(git_commit\[\] = \)\"[0-9a-f]*\";/\1\"$(shell git rev-parse HEAD)\";/" src/global.hpp

# $@ = left side of :, $< = first element right of : (%.cpp)
obj/%.o: src/%.cpp $(wildcard src/*.hpp) src/global.hpp
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c -o $@ $<

obj/%.o: src/*/%.cpp $(wildcard src/*/*.hpp) src/global.hpp
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c -o $@ $<

# $^ = right side of :
bin/$(NAME): $(patsubst %.cpp, obj/%.o, $(notdir $(wildcard src/*.cpp))) \
			 $(patsubst %.cpp, obj/%.o, $(notdir $(wildcard src/*/*.cpp)))
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o bin/$(NAME) $^ $(LDFLAGS) $(EXTRA_LDFLAGS) $(LDLIBS)

man/$(NAME).1: src/$(NAME).groff
	mkdir -p man
	sed "s/\[DATE\]/$(shell date --iso-8601)/" src/$(NAME).groff > man/$(NAME).1
	sed -i "s/\[VERSION\]/$(VERSION)/" man/$(NAME).1

.PHONY: test
test:
	./bin/seriespl -V && echo "\"Test\" passed."

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 0755 bin/$(NAME) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 0644 man/$(NAME).1 $(DESTDIR)$(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/share/man/man1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/bin

.PHONY: clean
clean:
	rm -rf obj/
	rm -rf bin/
	rm -rf man/
