CC=gcc
CXX=g++

GLOBALS=-D __LINUX__ -D _AMD64_
OPTIMIZATION=-O3
#OPTIMIZATION=-g -Og

CFLAGS=$(OPTIMIZATION) -Wall -Wextra -maes $(GLOBALS) -shared -fPIC
CXXFLAGS=$(OPTIMIZATION) -Wall -Wextra $(GLOBALS) -fopenmp -std=c++11 -fPIC

OBJDIRBASE=./obj
OBJDIR=$(OBJDIRBASE)/c
OBJDIRXX=$(OBJDIRBASE)/cpp
SRCDIR=./src
BINDIR=./bin

_OBJ= \
dependencies/aes/aes.o dependencies/aes/aes_ni.o \
prng/xof.o \
utils/fix_overflow.o \
utils/cycles.o \
utils/buftools.o \
utils/memavail.o
OBJ=$(patsubst %,$(OBJDIR)/%,$(_OBJ))

_OBJXX= \
prng/lcg.o prng/aes.o \
types/triples.o \
types/state.o \
memory/ram.o \
gen/points.o \
types/points.o \
gen/instance.o \
gen/random_function.o \
sync_strategies.o \
benchmarking.o \
vow.o \
swig_helpers.o \
utils/bintree.o \
main.o
OBJXX=$(patsubst %,$(OBJDIRXX)/%,$(_OBJXX))

_BINXX=main
BINXX=$(patsubst %,$(BINDIR)/%,$(_BINXX))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p `dirname $@`
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIRXX)/%.o: $(SRCDIR)/%.cpp
	mkdir -p `dirname $@`
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BINDIR)/%: $(BIN) $(OBJ) $(OBJXX)
	mkdir -p `dirname $@`
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

all: $(BIN) $(OBJ) $(OBJXX) $(BINXX)

swig: $(OBJ) $(OBJXX)
	mkdir -p pyvow
	mkdir -p $(OBJDIRXX)
	swig -Wall -Wextra -c++ -python -threads -outdir pyvow swig/vow.i
	$(CXX) $(CXXFLAGS) -c swig/vow_wrap.cxx -I/usr/include/python2.7 -o $(OBJDIRXX)/vow_wrap.o -fPIC
	rm swig/vow_wrap.cxx
	$(CXX) $(CXXFLAGS) -I/usr/include/python2.7 $^ $(OBJDIRXX)/vow_wrap.o -o pyvow/_vow.so -shared
	touch pyvow/__init__.py
	rm -f python/pyvow
	ln -rs pyvow python/pyvow

clean:
	rm -rf $(OBJDIRBASE)
	rm -rf $(BINDIR)
	rm -rf swig/vow_wrap.c swig/vow_wrap.cxx pyvow python/pyvow


