CC := icc
MPICC := mpicc

FLAGS := -std=c99 -O3 -pedantic -Wall
CFLAGS := $(FLAGS)
MPIFLAGS := $(FLAGS)

OBJ_DIR := obj
OBJECTS := $(addprefix $(OBJ_DIR)/, wrapper.o sw.o main.o)

TARGET := simmtx
ALIGN := align
GEN := generate
PREP := prepare

LEN1 := 266270
LEN2 := 8064
NPROC := 64
K := 1
OUTPUT := ./results/$(LEN1)x$(LEN2)_$(NPROC)_$(K).txt

all: make_dirs $(PREP) $(TARGET) # $(GEN) $(ALIGN)

$(TARGET): $(OBJECTS)
	$(MPICC) -O3 $^ -o $@

make_dirs:
	mkdir -p $(OBJ_DIR) results

$(OBJ_DIR)/wrapper.o: src/wrapper.c src/wrapper.h # Makefile
	$(MPICC) $(MPIFLAGS) -c $< -o $@

$(OBJ_DIR)/sw.o: src/sw.c src/wrapper.h src/sw.h # Makefile
	$(MPICC) $(MPIFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: src/main.c src/sw.h # Makefile
	$(MPICC) $(MPIFLAGS) -c $< -o $@

$(PREP): src/prepare.c
	$(CC) $(CFLAGS) $< -o $@

$(GEN): src/generate.c
	$(CC) $(CFLAGS) $< -o $@

$(ALIGN): src/align.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: all make_dirs run clean cleanup archive

run: $(PREP) $(TARGET) # $(ALIGN)
	./prep2.sh $(LEN1) $(LEN2)
	export OMP_NUM_THREADS=$(NTHREADS)
	sbatch -p test -n $(NPROC) -o $(OUTPUT) ompi ./$(TARGET) ../data/$(LEN1).target $(LEN1) ../data/$(LEN2).query $(LEN2) 2 -1 -2
# ../data/sim.mtx
# ./$(ALIGN) ../data/sim.mtx ../data/seq.target 102400 ../data/seq.query 10240 2 -1 -2 > out.txt

clean:
	rm -f $(PREP) $(TARGET)
# $(GEN) $(ALIGN)
	rm -rf $(OBJ_DIR)

cleanup: clean
	rm -f ../data/*.target ../data/*.query
	rm -rf results

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
