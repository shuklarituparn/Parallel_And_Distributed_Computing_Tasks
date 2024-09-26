#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double function(double x) { return 4.0 / (1.0 + x * x); }

double integrate(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.5 * (function(a) + function(b));
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += function(x);
    }
    return sum * h;
}

int main(int argc, char** argv) {
    int rank, size, N;
    double a = 0.0, b = 1.0, h, local_a, local_b, local_integral, sequential_integral;
    double total_integral = 0.0;
    double start_time, end_time, parallel_time, sequential_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: mpirun -np <num_processes> %s <N>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    N = atoi(argv[1]);
    h = (b - a) / N;

    if (rank == 0) {
        start_time = MPI_Wtime();
        sequential_integral = integrate(a, b, N);
        end_time = MPI_Wtime();
        sequential_time = end_time - start_time;
        printf("Took %f seconds to sequentially integrate\n", sequential_time);
        printf("Integrated with the value of %f\n", sequential_integral);
    }

    int local_N = N / size;
    int remainder = N % size;
    if (rank < remainder) {
        local_N++;
        local_a = a + rank * local_N * h;
    } else {
        local_a = a + (rank * local_N + remainder) * h;
    }
    local_b = local_a + local_N * h;

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    local_integral = integrate(local_a, local_b, local_N);

    if (rank != 0) {
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        total_integral = local_integral;
        for (int source = 1; source < size; source++) {
            MPI_Recv(&local_integral, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_integral += local_integral;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    parallel_time = end_time - start_time;

    if (rank == 0) {
        printf("With N = %d trapezoids, the integral estimate from %f to %f = %.5f\n",
               N, a, b, total_integral);
        printf("Parallel time: %.6f seconds\n", parallel_time);
        printf("Speedup: %.2f\n", sequential_time / parallel_time);
        FILE *fp= fopen("speedup_data.txt","a");
        if (fp==NULL){
            printf("Error opening file\n");
            MPI_Finalize();
            return 1;
        }
        fprintf(fp, "%d,%d,%f,%f,%f\n", N, size, sequential_time, parallel_time, sequential_time/parallel_time);
        fclose(fp);

    }

    MPI_Finalize();
    return 0;
}