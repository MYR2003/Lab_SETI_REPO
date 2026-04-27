#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int rank, size;
    char hostname[256];
    gethostname(hostname, 256);

    // Inicializar MPI con soporte para hilos
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 1. Prueba de Latencia (Ping-Pong)
    if (rank == 0) {
        printf("=== PRUEBA DE LATENCIA DEL CLUSTER ===\n");
        for (int i = 1; i < size; i++) {
            double start_time = MPI_Wtime();
            int ping = 1;
            // Enviamos un entero al nodo i
            MPI_Send(&ping, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // Esperamos respuesta
            MPI_Recv(&ping, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            double end_time = MPI_Wtime();
            printf("Respuesta desde Nodo %d (%s): Latencia = %.6f ms\n", 
                    i, hostname, (end_time - start_time) * 1000);
        }
    } else {
        int ping;
        MPI_Recv(&ping, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&ping, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Sincronizar todos antes de imprimir hilos
    MPI_Barrier(MPI_COMM_WORLD);
    sleep(1); 

    // 2. Verificación de Threads por Nodo
    #pragma omp parallel num_threads(2)
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();
        printf("[NODO %d - %s] Thread %d de %d activo.\n", 
                rank, hostname, thread_id, total_threads);
    }

    MPI_Finalize();
    return 0;
}