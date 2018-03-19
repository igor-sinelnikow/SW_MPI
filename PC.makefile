CC := gcc
MPICC := mpicc

# OPT := -O3
OPT := -g
OPT += -Og
FLAGS := -std=c99 $(OPT) -pedantic -Wall
CFLAGS := $(FLAGS)
MPIFLAGS := $(FLAGS)
OMP := -fopenmp

OBJECTS := $(addprefix obj/, wrapper.o sw.o main.o)

TARGET := simmtx
ALIGN := align
GEN := generate
PREP := prepare

LEN1 := 8192
LEN2 := 4096
NPROC := 4
NTHREADS := 1

all: obj $(PREP) $(TARGET) # $(GEN) $(ALIGN)

$(TARGET): $(OBJECTS)
	$(MPICC) $(OPT) $(OMP) $^ -o $@

obj:
	mkdir -p $@

obj/wrapper.o: src/wrapper.c src/wrapper.h
	$(MPICC) $(MPIFLAGS) -c $< -o $@

obj/sw.o: src/sw.c src/wrapper.h src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -c $< -o $@

obj/main.o: src/main.c src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -c $< -o $@

$(PREP): src/prepare.c
	$(CC) $(CFLAGS) -Wno-unused-result $< -o $@

$(GEN): src/generate.c
	$(CC) $(CFLAGS) $< -o $@

$(ALIGN): src/align.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: all run clean cleanup archive

run: $(PREP) $(TARGET) # $(ALIGN)
	./prep2 $(LEN1) $(LEN2)
	mpirun -np $(NPROC) ./$(TARGET) ../data/$(LEN1).target $(LEN1) ../data/$(LEN2).query $(LEN2) 2 -1 -2 $(NTHREADS)
# ../data/sim.mtx
# ./$(ALIGN) ../data/sim.mtx ../data/seq.target 102400 ../data/seq.query 10240 2 -1 -2 > out.txt

clean:
	rm -f $(PREP) $(TARGET) $(GEN) $(ALIGN)
	rm -rf obj

cleanup: clean
	rm -f ../data/*.target ../data/*.query

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
