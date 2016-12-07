# Makefile for seriespl
ifndef CXX
	CXX = c++
endif
LDLIBS   = -lcurl -lconfig++
ifndef CXXFLAGS
	CXXFLAGS = -O2 -pipe -Wall -mtune=native
endif
# Make sure that c++11 is selected
override CXXFLAGS += -std=c++14
NAME = seriespl
ifndef PREFIX
	PREFIX = /usr/local
endif
# arm-g++ doesn't seem to support -march=native, despite saying so
ifneq (,$(findstring arm,$(CXX)))
	CXXFLAGS := $(filter-out -mtune=native,$(CXXFLAGS))
endif
VERSION=$(shell grep "version\[\] =" src/global.hpp | cut -d\" -f2)


.PHONY: all
all: dirs $(NAME) man

.PHONY: dirs
dirs: bin obj

.PHONY: $(NAME)
$(NAME): bin/$(NAME)

.PHONY: man
man: $(NAME).1

bin obj:
	mkdir -p bin
	mkdir -p obj

# $@ = left side of :, $< = first element right of : (%.cpp)
obj/%.o: src/%.cpp $(wildcard src/*.hpp)
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -c -o $@ $<

# $^ = right side of :
bin/$(NAME): $(patsubst %.cpp, obj/%.o, $(notdir $(wildcard src/*.cpp)))
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o bin/$(NAME) $^ $(LDFLAGS) $(EXTRA_LDFLAGS) $(LDLIBS)

$(NAME).1: manpage
	sed "s/\[DATE\]/$(shell date --iso-8601)/" manpage > $(NAME).1
	sed -i "s/\[VERSION\]/$(VERSION)/" $(NAME).1

.PHONY: debug
debug: CXXFLAGS += -DDEBUG -g -ggdb
debug: $(NAME)

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 0755 bin/$(NAME) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 0644 $(NAME).1 $(DESTDIR)$(PREFIX)/share/man/man1

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/share/man/man1
	rmdir --parents --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/bin

.PHONY: clean
clean:
	rm -f obj/*.o
	rm -f bin/$(NAME)
	rm -f $(NAME).1

.PHONY: test
test:
	./bin/seriespl -V && echo "\"Test\" passed."
