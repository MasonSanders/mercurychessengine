CXX ?= g++
MUSL_CXX ?= musl-g++
STRIP ?= strip
TAR ?= tar
SHA256SUM ?= sha256sum

TARGET ?= mercury
VERSION ?= dev
BUILD ?= native
OBJDIR ?= bin/$(BUILD)
DISTDIR ?= dist

SRCS := src/main.cpp src/Board.cpp src/Engine.cpp src/TranspositionTable.cpp
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

HEADERS := include/Board.h include/Piece.h include/Move.h include/MoveType.h include/BitboardUtils.h include/Engine.h include/TranspositionTable.h

THREAD_FLAGS ?= -pthread
PORTABLE_X86_FLAGS ?= -march=x86-64 -mtune=generic
CXXFLAGS ?= -std=c++23 -Wall -Wextra -Wpedantic -O3 -DNDEBUG $(THREAD_FLAGS) $(PORTABLE_X86_FLAGS)
LDFLAGS ?= $(THREAD_FLAGS) -static-libstdc++ -static-libgcc
LDLIBS ?=

GLIBC_PACKAGE := mercury-$(VERSION)-linux-x86_64-glibc
MUSL_PACKAGE := mercury-$(VERSION)-linux-x86_64-musl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

release: $(TARGET)
	$(STRIP) --strip-unneeded $(TARGET)

glibc:
	$(MAKE) BUILD=glibc TARGET=mercury CXX="$(CXX)" LDFLAGS="$(THREAD_FLAGS) -static-libstdc++ -static-libgcc" release

musl: check-musl-cxx
	$(MAKE) BUILD=musl TARGET=mercury-musl CXX="$(MUSL_CXX)" LDFLAGS="-static $(THREAD_FLAGS)" release

package: package-glibc

package-all: package-glibc package-musl

package-glibc:
	$(MAKE) clean
	$(MAKE) BUILD=glibc TARGET=mercury CXX="$(CXX)" LDFLAGS="$(THREAD_FLAGS) -static-libstdc++ -static-libgcc" release
	rm -rf $(DISTDIR)/$(GLIBC_PACKAGE)
	mkdir -p $(DISTDIR)/$(GLIBC_PACKAGE)
	cp mercury $(DISTDIR)/$(GLIBC_PACKAGE)/
	chmod 755 $(DISTDIR)/$(GLIBC_PACKAGE)/mercury
	$(TAR) -C $(DISTDIR) -czf $(DISTDIR)/$(GLIBC_PACKAGE).tar.gz $(GLIBC_PACKAGE)
	$(SHA256SUM) $(DISTDIR)/$(GLIBC_PACKAGE).tar.gz > $(DISTDIR)/$(GLIBC_PACKAGE).tar.gz.sha256

package-musl: check-musl-cxx
	$(MAKE) clean
	$(MAKE) BUILD=musl TARGET=mercury CXX="$(MUSL_CXX)" LDFLAGS="-static $(THREAD_FLAGS)" release
	rm -rf $(DISTDIR)/$(MUSL_PACKAGE)
	mkdir -p $(DISTDIR)/$(MUSL_PACKAGE)
	cp mercury $(DISTDIR)/$(MUSL_PACKAGE)/
	chmod 755 $(DISTDIR)/$(MUSL_PACKAGE)/mercury
	$(TAR) -C $(DISTDIR) -czf $(DISTDIR)/$(MUSL_PACKAGE).tar.gz $(MUSL_PACKAGE)
	$(SHA256SUM) $(DISTDIR)/$(MUSL_PACKAGE).tar.gz > $(DISTDIR)/$(MUSL_PACKAGE).tar.gz.sha256

check-musl-cxx:
	@command -v "$(MUSL_CXX)" >/dev/null 2>&1 || { \
		echo "error: $(MUSL_CXX) not found. Install a musl C++ toolchain or run 'make MUSL_CXX=/path/to/musl-g++ package-musl'."; \
		exit 1; \
	}

clean:
	rm -rf bin mercury mercury-musl

distclean: clean
	rm -rf $(DISTDIR)

.PHONY: all run release glibc musl package package-all package-glibc package-musl check-musl-cxx clean distclean
