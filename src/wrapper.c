#include <stdio.h>
#include <mpi.h>

#include "wrapper.h"

void Abort(const char* file, int line, const char* call, int errorcode) {
	char errorstring[MPI_MAX_ERROR_STRING];
	int rank, resultlen;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if (rank == 0)
		fprintf(stderr, "%s:%d: error in %s:\n", file, line, call);
	MPI_Error_string(errorcode,errorstring,&resultlen);
	fprintf(stderr, "rank %d: %s\n", rank, errorstring);
	MPI_Abort(MPI_COMM_WORLD,errorcode);
}

void MPIE_File_open(MPI_Comm comm, const char *filename, \
                    int amode, MPI_Info info, \
                    MPI_File *fh) {
	int errorcode = MPI_File_open(comm,filename,amode,info,fh);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_File_open",errorcode);
}

void MPIE_File_read(MPI_File fh, void *buf, \
                    int count, MPI_Datatype datatype, MPI_Status *status) {
	int errorcode = MPI_File_read(fh,buf,count,datatype,status);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_File_read",errorcode);
}

void MPIE_File_read_ordered(MPI_File fh, void *buf, \
                            int count, MPI_Datatype datatype, \
                            MPI_Status *status) {
	int errorcode = MPI_File_read_ordered(fh,buf,count,datatype,status);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_File_read_ordered",errorcode);
}

void MPIE_File_write_ordered(MPI_File fh, const void *buf, \
                             int count, MPI_Datatype datatype, \
                             MPI_Status *status) {
	int errorcode = MPI_File_write_ordered(fh,buf,count,datatype,status);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_File_write_ordered",errorcode);
}

double MPIT_Recv(void *buf, int count, MPI_Datatype datatype, \
                 int source, int tag, MPI_Comm comm, MPI_Status *status) {
	double time = -MPI_Wtime();
	int errorcode = MPI_Recv(buf,count,datatype,source,tag,comm,status);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_Recv",errorcode);
	return (time += MPI_Wtime());
}

double MPIT_Wait(MPI_Request *request, MPI_Status *status) {
	double time = -MPI_Wtime();
	int errorcode = MPI_Wait(request,status);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_Wait",errorcode);
	return (time += MPI_Wtime());
}

double MPIT_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, \
                  int tag, MPI_Comm comm, MPI_Request *request) {
	double time = -MPI_Wtime();
	int errorcode = MPI_Isend(buf,count,datatype,dest,tag,comm,request);
	if (errorcode != MPI_SUCCESS)
		Abort(__FILE__,__LINE__-2,"MPI_Isend",errorcode);
	return (time += MPI_Wtime());
}
