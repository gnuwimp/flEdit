OS := $(shell uname -s)

ifeq ($(findstring MINGW64, $(OS)), MINGW64)
	OS = Windows
else ifeq ($(findstring inux, $(OS)), inux)
	OS = Linux
else ifeq ($(findstring arwin, $(OS)), arwin)
	OS = macOS
else
$(error Unknow operating system $(OS))
endif

CXXFLAGS  = -O2
CXXFLAGS += $(shell /usr/local/bin/fltk-config --cxxflags)
CXXFLAGS += -std=c++17
CXXFLAGS += -DNDEBUG
CXXFLAGS += -DGNU_USE_PCRE
CXXFLAGS += -DGNU_USE_SQLITE3

CFLAGS    = -O2
CFLAGS   += -DNDEBUG
CFLAGS   += -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
CFLAGS   += -DSQLITE_MAX_EXPR_DEPTH=0
CFLAGS   += -DSQLITE_OMIT_DECLTYPE
CFLAGS   += -DSQLITE_OMIT_DEPRECATED
CFLAGS   += -DSQLITE_OMIT_PROGRESS_CALLBACK
CFLAGS   += -DSQLITE_OMIT_SHARED_CACHE

LDFLAGS   = $(shell /usr/local/bin/fltk-config --ldflags)

ifeq ($(OS), macOS)
	CXX       = clang++
	CC        = clang
	EXE       = fledit
else ifeq ($(OS), Windows)
	LDFLAGS  += -static
	CXXFLAGS += -D__USE_MINGW1_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800
	EXE       = fledit.exe
	RES       = fledit.res
	CXX       = g++
	CC        = gcc
else
	EXE       = fledit
	LDFLAGS  += -flto
	CXX       = g++
	CC        = gcc
endif

ifeq ($(build), pgo1)
	CXXFLAGS += -fprofile-generate -fprofile-update=atomic
	CFLAGS   += -fprofile-generate -fprofile-update=atomic
	LDFLAGS  += -fprofile-generate
else ifeq ($(build), pgo2)
	CXXFLAGS += -fprofile-use
	CFLAGS   += -fprofile-use
	LDFLAGS  += -fprofile-use
endif

all: $(EXE)

$(RES): res/fledit.rc
	windres res/fledit.rc -O coff -o $(RES)

pcre8.o: src/pcre8.c src/pcre8.h
	$(CC) $(CFLAGS) -c $< -o $@

sqlite3.o: src/sqlite3.c src/sqlite3.h
	$(CC) $(CFLAGS) -c $< -o $@

fledit.o: src/fledit.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): fledit.o pcre8.o sqlite3.o $(RES)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXE) $(RES) *.o *.gcda

pgo:
	make clean
	make -j 3 build=pgo1
	./$(EXE) src/*
	rm -f $(EXE) *.o
	make -j 3 build=pgo2
