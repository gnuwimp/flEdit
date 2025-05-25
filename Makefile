include makeinclude

CXXFLAGS += $(FLTK_CXXFLAGS)
CXXFLAGS += -DGNU_USE_PCRE
CXXFLAGS += -DGNU_USE_SQLITE3
CFLAGS   += -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
CFLAGS   += -DSQLITE_MAX_EXPR_DEPTH=0
CFLAGS   += -DSQLITE_OMIT_DECLTYPE
CFLAGS   += -DSQLITE_OMIT_DEPRECATED
CFLAGS   += -DSQLITE_OMIT_PROGRESS_CALLBACK
CFLAGS   += -DSQLITE_OMIT_SHARED_CACHE
LDFLAGS  += $(FLTK_LDFLAGS)

ifeq ($(OS), Windows)
	CFLAGS     += -Wno-dangling-pointer
	CFLAGS     += -Wno-unused-but-set-variable
	CFLAGS     += -Wno-implicit-fallthrough
	CFLAGS     += -Wno-unused-parameter
	RESOURCE    = obj/fledit.res
	EXECUTABLE  = fledit.exe
else ifeq ($(OS), Linux)
	CFLAGS     += -Wno-implicit-fallthrough
	CFLAGS     += -Wno-unused-parameter
	EXECUTABLE  = fledit
else ifeq ($(OS), macOS)
	EXECUTABLE = fledit
else
	EXECUTABLE = fledit
endif

all: obj $(EXECUTABLE)

obj:
	mkdir obj

OBJ = obj/fledit.o obj/pcre8.o obj/sqlite3.o obj/rainbow.o

$(RESOURCE): res/fledit.rc
	windres res/fledit.rc -O coff -o $(RESOURCE)

obj/pcre8.o: src/pcre8.c src/pcre8.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/sqlite3.o: src/sqlite3.c src/sqlite3.h
	$(CC) $(CFLAGS) -c $< -o $@ -Wno-unused-parameter

obj/rainbow.o: src/rainbow.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/fledit.o: src/fledit.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJ) $(RESOURCE)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE) $(RESOURCE)
	rm -f -R obj/*

pgo:
	make clean
	make -j 3 build=pgo1
	./$(EXECUTABLE) src/*
	rm -f $(EXECUTABLE) obj/*.o
	make -j 3 build=pgo2
