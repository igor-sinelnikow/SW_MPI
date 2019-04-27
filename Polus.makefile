CC := xlc
MPICC := mpixlc

OPT := -O3 -qhot -qarch=pwr8 -qtune=pwr8:balanced
CFLAGS := $(OPT)
MPIFLAGS := $(OPT)
# MPIFLAGS := -g -pedantic -Wall $(OPT)
OMP := -qsmp=omp

OBJ_MPI := $(addprefix obj/, wrapper.o sw_mpi.o main_mpi.o)
OBJ_OMP := $(addprefix obj/, wrapper.o sw_omp.o main_omp.o)
OBJ_GPU := $(addprefix obj/, wrapper.o sw_gpu.o main_gpu.o)

ALIGN := align
GEN := generate
PREP := prepare

len1 := 64113
len2 := 62000
# len1 := 67106
# len2 := 64000
TARGET := ../data/$(len1).target
QUERY := ../data/$(len2).query

mpi_tasks := 4
id := $(len1)x$(len2)-p$(mpi_tasks)

num_threads_per_task := 8
smt_mode := 8
cpus_per_core := 8
task_dist := pack
esub := -a "p8aff($(num_threads_per_task),$(smt_mode),$(cpus_per_core),$(task_dist))"

dev := 2
gpu := -gpu "num=$(dev):mode=exclusive_process"

# queue := -q "normal"

all: obj simmtx_gpu simmtx_omp simmtx_mpi $(PREP) $(ALIGN) # $(GEN)

simmtx_mpi: $(OBJ_MPI)
	$(MPICC) $(OPT) $^ -o $@

simmtx_omp: $(OBJ_OMP)
	$(MPICC) $(OPT) $(OMP) $^ -o $@

simmtx_gpu: $(OBJ_GPU)
	$(MPICC) $(OPT) $(OMP) -qoffload $^ -o $@

obj:
	mkdir -p $@

obj/wrapper.o: src/wrapper.c src/wrapper.h
	$(MPICC) $(MPIFLAGS) -c $< -o $@

obj/sw_mpi.o: src/sw.c src/wrapper.h src/sw.h
	$(MPICC) $(MPIFLAGS) -c $< -o $@

obj/sw_omp.o: src/sw.c src/wrapper.h src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -c $< -o $@

obj/sw_gpu.o: src/sw.c src/wrapper.h src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -qoffload -c $< -o $@

obj/main_mpi.o: src/main.c src/sw.h
	$(MPICC) $(MPIFLAGS) -c $< -o $@

obj/main_omp.o: src/main.c src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -c $< -o $@

obj/main_gpu.o: src/main.c src/sw.h
	$(MPICC) $(MPIFLAGS) $(OMP) -qoffload -c $< -o $@

$(PREP): src/prepare.c
	$(CC) $(CFLAGS) $< -o $@

$(GEN): src/generate.c
	$(CC) $(CFLAGS) $< -o $@

$(ALIGN): src/align.c
	$(CC) $(CFLAGS) $< -o $@

$(TARGET): $(PREP)
	./$< ../data/M.mycoides.fasta $@ $(len1)

$(QUERY): $(PREP)
	./$< ../data/M.capricolum.fasta $@ $(len2)

.PHONY: all mpi run offload clean cleanup archive

offload: simmtx_gpu $(TARGET) $(QUERY) # $(ALIGN)
	bsub -n $(mpi_tasks) $(gpu) -R "select[(maxmem==256G) && (type==any)]" $(queue) -o $<.$(id)-dev$(dev).%J.out -e $<.$(id)-dev$(dev).%J.txt -J "SW_GPU" mpiexec ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2
# 	mpirun -np $(mpi_tasks) ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2
# 	./$(ALIGN) ../data/sim.mtx $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2

run: simmtx_omp $(TARGET) $(QUERY) # $(ALIGN)
	bsub -n $(mpi_tasks) $(esub) -R "select[(maxmem==256G) && (type==any)] same[nthreads]" -env "all, OMP_DISPLAY_ENV=VERBOSE, OMP_DYNAMIC=FALSE, OMP_SCHEDULE=STATIC" $(queue) -o $<.$(id)-t$(num_threads_per_task).%J.out -e $<.$(id)-t$(num_threads_per_task).%J.txt -J "SW_MPI" mpiexec ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2
# 	-W 00:01
# 	OMP_DYNAMIC=FALSE OMP_SCHEDULE=STATIC XLSMPOPTS=procs="`t_map $(num_threads_per_task) $(mpi_tasks) 1 0`" mpirun -np $(mpi_tasks) ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2 ../data/sim.mtx
# 	./$(ALIGN) ../data/sim.mtx $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2
# 	valgrind --leak-check=full mpirun -np $(mpi_tasks) ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2
# 	mpisubmit.pl -p $(mpi_tasks) -t $(num_threads_per_task) -d ./$< -- $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2 > SW_MPI.lsf

mpi: simmtx_mpi $(TARGET) $(QUERY) $(ALIGN)
	mpirun -np $(mpi_tasks) ./$< $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2 ../data/sim.mtx
	./$(ALIGN) ../data/sim.mtx $(TARGET) $(len1) $(QUERY) $(len2) 2 -1 -2

clean:
	rm -f simmtx_mpi simmtx_omp simmtx_gpu $(PREP) $(GEN) $(ALIGN)
	rm -rf obj

cleanup: clean
	rm -f ../data/*.target ../data/*.query

archive: cleanup
	tar cz -C .. -f ../SW_MPI.tar.gz data/ SW_MPI/
