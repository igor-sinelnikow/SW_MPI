#pragma once

#include <mpi.h>

#ifndef BLUEGENE
#   define CONST const
#else
#   define CONST
#endif

void Abort(const char* file, int line, const char* call, int errorcode);

void MPIE_File_open(MPI_Comm comm, CONST char *filename, int amode,
                    MPI_Info info, MPI_File *fh);

void MPIE_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                    MPI_Status *status);

void MPIE_File_read_ordered(MPI_File fh, void *buf, int count,
                            MPI_Datatype datatype, MPI_Status *status);

void MPIE_File_write_ordered(MPI_File fh, CONST void *buf, int count,
                             MPI_Datatype datatype, MPI_Status *status);

double MPIT_Recv(void *buf, int count, MPI_Datatype datatype, int source,
                 int tag, MPI_Comm comm, MPI_Status *status);

double MPIT_Wait(MPI_Request *request, MPI_Status *status);

double MPIT_Isend(CONST void *buf, int count, MPI_Datatype datatype, int dest,
                  int tag, MPI_Comm comm, MPI_Request *request);
