#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define THREADS_NUM 14

void SaveResults(double speedup);
double Function(double x);
double integral_result = 0;
double N = 1000000;

int upper_limit = 1;
int lower_limit = 0;
double height;

int main(int argc, char **argv)
{
    height = (double)(upper_limit - lower_limit) / N;
    clock_t sqe_start, sqe_end;
    sqe_start = clock();

    integral_result += Function(upper_limit) + Function(lower_limit);

    for (int i = 0; i < N; i++)
    {
        integral_result += 2 * Function(i * height);
    }

    sqe_end = clock();
    double seq_elapsed = (double)(sqe_end - sqe_start) / CLOCKS_PER_SEC;

#ifdef _OPENMP
    printf("OPENMP is supported! -ver: %d\n", _OPENMP);
#endif

    integral_result = 0;
    double integral_result_reduction = 0;

    double parallel_start = omp_get_wtime();

    int num_procs = omp_get_num_procs();
    printf("Num of processors = %d\n", num_procs);

    omp_set_num_threads(THREADS_NUM);
    int num_threads = omp_get_num_threads();
    printf("Number of threads = %d\n", num_threads);

#pragma omp parallel shared(integral_result, integral_result_reduction)
    {
        int my_id = omp_get_thread_num();
        double local_result = 0;

#pragma omp for
        for (int i = 0; i < (int)N; i++)
        {
            local_result += 2 * Function(i * height);
        }

#pragma omp for reduction(+:integral_result_reduction)
        for (int i = 0; i < (int)N; i++)
        {
            integral_result_reduction += 2 * Function(i * height);
        }

#pragma omp critical
        {
            printf("Integral calculated by thread <%d> is = %f\n", my_id, local_result);
            integral_result += local_result;
        }
    }

    printf("\n\nIntegral result = %f\n", (height / 2) * integral_result);
    printf("Integral result using reductions = %f\n", (height / 2) * integral_result_reduction);
    double parallel_elapsed = omp_get_wtime() - parallel_start;
    double speedup = seq_elapsed / parallel_elapsed;
    printf("Speedup = %f\n", speedup);

    SaveResults(speedup);

    return 0;
}

double Function(double x)
{
    return 4 / (1 + x * x);
}

void SaveResults(const double speedup)
{
    char file_name[50];
    sprintf(file_name, "P=%d - %f.txt", THREADS_NUM, speedup);
    FILE* output = fopen(file_name, "w");
    if (output != NULL)
    {
        fprintf(output, "%d\n%f\n", THREADS_NUM, speedup);
        fclose(output);
    }
}
