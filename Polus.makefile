CC := xlc
MPICC := mpixlc

# OPT := -Og
OPT := -O3 -qhot -qarch=pwr8 -qtune=pwr8:balanced
CFLAGS := $(OPT)
MPIFLAGS := $(OPT)
# -g -pedantic -Wall
OMP := -qsmp=omp

OBJECTS := $(addprefix obj/, wrapper.o sw.o main.o)

TARGET := simmtx
ALIGN := align
GEN := generate
PREP := prepare

len1 := 20
len2 := 20
mpi_tasks := 5
num_threads_per_task := 8
smt_mode := 8
cpus_per_core := 8
task_dist := pack
# queue := -q "normal"

all: obj $(PREP) $(TARGET) $(ALIGN) # $(GEN)

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

.PHONY: all run clean cleanup archive

run: $(TARGET) $(PREP) $(ALIGN)
	./prep2.sh $(len1) $(len2)
# 	valgrind --leak-check=full
	mpirun -np $(mpi_tasks) ./$< ../data/$(len1).target $(len1) ../data/$(len2).query $(len2) 2 -1 -2 ../data/sim.mtx
	./$(ALIGN) ../data/sim.mtx ../data/$(len1).target $(len1) ../data/$(len2).query $(len2) 2 -1 -2
# 	bsub -n $(mpi_tasks) -a "p8aff($(num_threads_per_task),$(smt_mode),$(cpus_per_core),$(task_dist))" -R "select[maxmem==256G] same[nthreads]" -env "all, OMP_DISPLAY_ENV=VERBOSE, OMP_DYNAMIC=FALSE, OMP_SCHEDULE=STATIC" $(queue) -o $<.%J.out -e $<.%J.err -J "SW_MPI" mpiexec ./$< ../data/$(len1).target $(len1) ../data/$(len2).query $(len2) 2 -1 -2
# 	mpisubmit.pl -p $(mpi_tasks) -t $(num_threads_per_task) -d ./$< -- ../data/$(len1).target $(len1) ../data/$(len2).query $(len2) 2 -1 -2 > SW_MPI.lsf

clean:
	rm -f $(TARGET) $(PREP) $(GEN) $(ALIGN)
	rm -rf obj lsf

cleanup: clean
	rm -f ../data/*.target ../data/*.query

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
