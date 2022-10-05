#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 10000000

int main(int argc, char *argv[])
{
    int i, qntdDentro = 0;
    double pi = 0.0, PontoX, PontoY, distancia;

    srand(time(NULL));

    MPI_Init(&argc, &argv); // Inicio da area paralela do MPI

    // Sincroniza os processos
    MPI_Barrier(MPI_COMM_WORLD);

    // Encontra quantos estão dentro do circulo
    for (i = 0; i < N; i++)
    {
        PontoX = rand() / (RAND_MAX + 1.0); // Gera dois pontos dentro do quadrado
        PontoY = rand() / (RAND_MAX + 1.0); // Gera dois pontos dentro do quadrado
        distancia = (PontoX * PontoX) + (PontoY * PontoY); // Calculo da distancia entre os dois pontos
        if (distancia < 1.0) //Verifica se a distancia é menor q o raio do circulo
            qntdDentro++;
    }

    // Sincroniza os processos
    MPI_Barrier(MPI_COMM_WORLD);

    pi = (4 * qntdDentro);
    printf("PI %f\n", pi/N);// Calcula o Pi

    MPI_Finalize();

    return 0;
}