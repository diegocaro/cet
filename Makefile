DEPEN_LIBCDS_INC=../libcds/includes/
DEPEN_LIBCDS=../libcds/lib/libcds.a

DEPEN_WT64=../wtkd/
DEPEN_WT64_OBJS=wt_coder.o wt_node.o wt_node_leaf.o wavelet_tree.o wt_coder_binary.o wt_node_internal.o wt_coder_morton.o wt_coder_huff_morton.o symbols.o

CC=g++
DEBUG = -g -DDEBUG
CFLAGS=-c -Wall -I$(DEPEN_LIBCDS_INC) -I$(DEPEN_WT64) -O9
LDFLAGS=-Wall $(DEPEN_LIBCDS) -lm

EXT=

SOURCES=tgl.cpp
OBJECTS=$(SOURCES:.cpp=.o)

DEPENOBJS+=$(addprefix $(DEPEN_WT64), $(DEPEN_WT64_OBJS))

MAINSRC=create.cpp use.cpp test.cpp
MAINOBJ=$(MAINSRC:.cpp=.o)

EXECUTABLE=create use test

#all: CFLAGS += -O9
all: $(SOURCES) $(MAINSRC) $(MAINOBJ) $(OBJECTS) $(EXECUTABLE) 

debug: CFLAGS += $(DEBUG)
debug: clean all

#$(EXECUTABLE): $(MAINOBJ) $(OBJECTS)
#	$(CC) $(LDFLAGS) $(OBJECTS) $(DEPENOBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: cleanobj cleanexe

cleanobj:
	rm -f $(OBJECTS) $(MAINOBJ)

cleanexe:
	rm -f $(EXECUTABLE)

create: create.o
	$(CC) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

use: use.o
	$(CC) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

benchmark: benchmark.o
	$(CC) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

test: test.o
	$(CC) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)