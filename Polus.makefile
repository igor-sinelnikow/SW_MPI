CC := gcc
MPICC := mpicc

OPT := -O3
# OPT := -g
FLAGS := $(OPT) -pedantic
CFLAGS := -std=c99 $(FLAGS) -Wall
MPIFLAGS := $(FLAGS)
OMP := -fopenmp

OBJECTS := $(addprefix obj/, wrapper.o sw.o main.o)

TARGET := simmtx
ALIGN := align
GEN := generate
PREP := prepare

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
	$(CC) $(CFLAGS) $< -o $@

$(GEN): src/generate.c
	$(CC) $(CFLAGS) $< -o $@

$(ALIGN): src/align.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: all clean cleanup archive

clean:
	rm -f $(PREP) $(TARGET) $(GEN) $(ALIGN)
	rm -rf obj lsf

cleanup: clean
	rm -f ../data/*.target ../data/*.query

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
