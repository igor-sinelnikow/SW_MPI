#pragma once

void Abort(const char* file, int line, const char* call, int errorcode);

void MPIE_File_open(MPI_Comm comm, const char *filename, \
                    int amode, MPI_Info info, \
                    MPI_File *fh);

void MPIE_File_read(MPI_File fh, void *buf, \
                    int count, MPI_Datatype datatype, MPI_Status *status);

void MPIE_File_read_ordered(MPI_File fh, void *buf, \
                            int count, MPI_Datatype datatype, \
                            MPI_Status *status);

void MPIE_File_write_ordered(MPI_File fh, const void *buf, \
                             int count, MPI_Datatype datatype, \
                             MPI_Status *status);

double MPIT_Recv(void *buf, int count, MPI_Datatype datatype, \
                 int source, int tag, MPI_Comm comm, MPI_Status *status);

double MPIT_Wait(MPI_Request *request, MPI_Status *status);

double MPIT_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, \
                  int tag, MPI_Comm comm, MPI_Request *request);
