// Angel Higueros 20460
// Corto 5 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void par_qsort(int *data, int lo, int hi)
{
    if (lo > hi)
        return;
    int l = lo;
    int h = hi;
    int p = data[(hi + lo) / 2];
    while (l <= h)
    {
        while ((data[l] - p) < 0)
            l++;
        while ((data[h] - p) > 0)
            h--;
        if (l <= h)
        {
            int tmp = data[l];
            data[l] = data[h];
            data[h] = tmp;
            l++;
            h--;
        }
    }

#pragma omp task
    par_qsort(data, lo, h);
#pragma omp task
    par_qsort(data, l, hi);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Uso: %s <numero_n>\n", argv[1]);
        return 1;
    }

    int N = atoi(argv[1]);

    int *x = malloc(N * sizeof(int));
    int j;

    clock_t start_time = clock();

    //---- Inicialización con elementos aleatorios entre 1-N/2
    printf("Generando  %d numeros aleatorios \n", N);
    int posibles_elementos = N / 2;
#pragma omp parallel for
    for (j = 0; j < N; j++)
        x[j] = rand() % (posibles_elementos) + 1;

    // Abrir un archivo A y escribir los numeros aleatorios
    FILE *archivo = fopen("numeros_aleatorios.txt", "w");
    if (archivo == NULL)
    {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    for (int i = 0; i < N; i++)
    {
        int numero = x[i];
        fprintf(archivo, "%d", numero);

        if (i != N - 1)
        {
            fprintf(archivo, ",");
        }
    }

    fclose(archivo);

// Ordenar los numeros random y gardarlos en el nuevo archivo
#pragma omp parallel sections
    {
#pragma omp section
        {
            par_qsort(x, 0, N / 2 - 1);
        }
#pragma omp section
        {
            par_qsort(x, N / 2, N - 1);
        }
    }

    // Abrir un archivo B y escribir los numeros ordenados
    FILE *archivoB = fopen("numeros_ordenados.txt", "w");
    if (archivoB == NULL)
    {
        printf("No se pudo abrir el archivoB.\n");
        return 1;
    }

    for (int i = 0; i < N; i++)
    {
        int numero = x[i];
        fprintf(archivoB, "%d", numero);

        if (i != N - 1)
        {
            fprintf(archivoB, ",");
        }
    }

    fclose(archivoB);

    clock_t end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Se generaron  y ordenaron %d números aleatorios .\n", N);
    printf("Tiempo: %f segundos.\n", total_time);

    return 0;
}
