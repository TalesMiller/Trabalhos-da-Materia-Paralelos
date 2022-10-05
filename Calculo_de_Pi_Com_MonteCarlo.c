#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static long num_steps = 100000000;
int main()
{
    int pontosNoCirculo = 0, qntdPontos = 0;
    double posX, posY, dist;
    int i;
    double pi;

#pragma omp parallel private(posX, posY, dist, i) reduction(+ : pontosNoCirculo, qntdPontos)
    {
        srand48((int)time(NULL));
        int numThreads = omp_get_num_threads();

        for (i = 0; i < num_steps/numThreads; i++)
        {
            posX = (double)drand48(); // Gera uma posição float(entre 0 e 1) aleatória
            posY = (double)drand48(); // Gera uma posição float(entre 0 e 1) aleatória

            dist = ((posX * posX) + (posY * posY)); //Calcula a distancia entre os dois pontos

            if (dist <= 1)// Verifica se a distancia é menor que o raio, assim ficando dentro do circulo.
            {
                pontosNoCirculo++; // Contador de posições dentro do circulo
            }
            qntdPontos++; //Contador de quantidade de pontos
            
        }
        
    }

    pi = 4.0 * ((double)pontosNoCirculo / (double)(qntdPontos)); // Multiplica a area do quadrado pela chance de um ponto estar dentro da esfera.
    printf("%f\n", pi);
}
