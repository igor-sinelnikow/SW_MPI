CC := gcc
# CC := icc
MPICC := mpicc

FLAGS := -std=c99 -O3 -pedantic -Wall
CFLAGS := $(FLAGS) -Wno-unused-result
MPIFLAGS := $(FLAGS)

OBJ_DIR := obj
OBJECTS := $(addprefix $(OBJ_DIR)/, wrapper.o sw.o main.o)

TARGET := simmtx
ALIGN := align
GEN := generate
PREP := prepare

LEN1 := 8192
LEN2 := 4096
NPROC := 2

all: obj_dir $(PREP) $(TARGET) # $(GEN) $(ALIGN)

$(TARGET): $(OBJECTS)
	$(MPICC) -O3 $^ -o $@

obj_dir:
	mkdir -p $(OBJ_DIR)

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

.PHONY: all obj_dir run clean cleanup archive

run: $(PREP) $(TARGET) # $(ALIGN)
	./prep2 $(LEN1) $(LEN2)
	mpirun -np $(NPROC) ./$(TARGET) ../data/$(LEN1).target $(LEN1) ../data/$(LEN2).query $(LEN2) 2 -1 -2
# ../data/sim.mtx
# ./$(ALIGN) ../data/sim.mtx ../data/seq.target 102400 ../data/seq.query 10240 2 -1 -2 > out.txt

clean:
	rm -f $(PREP) $(TARGET)
# $(GEN) $(ALIGN)
	rm -rf $(OBJ_DIR)

cleanup: clean
	rm -f ../data/*.target ../data/*.query

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
