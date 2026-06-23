#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    int host_len = 0;
    char host[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(host, &host_len);

    int value = 0;
    if (rank == 0) {
        value = 12345;
    }

    printf("before bcast rank=%d size=%d host=%s value=%d\n", rank, size, host, value);
    fflush(stdout);

    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_ok = (value == 12345) ? 1 : 0;
    int global_ok = 0;
    MPI_Allreduce(&local_ok, &global_ok, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("after bcast rank=%d host=%s value=%d global_ok=%d/%d\n",
           rank, host, value, global_ok, size);
    fflush(stdout);

    MPI_Finalize();
    return global_ok == size ? EXIT_SUCCESS : EXIT_FAILURE;
}
