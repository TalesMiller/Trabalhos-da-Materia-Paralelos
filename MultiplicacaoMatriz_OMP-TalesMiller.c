#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <sys/time.h>

#define TamMat 1000

#define TamBlock 128

int Mat1[TamMat][TamMat];
int Mat2[TamMat][TamMat];
int vetor1[TamMat * TamMat];
int vetor2[TamMat * TamMat];
int MultMat_Res[TamMat][TamMat];

int menor(int x, int y)
{
	if (x < y)
		return x;
	else
		return y;
}

void converter()
{
#pragma omp parallel for
	for (int i = 0; i < TamMat; i++)
	{
		for (int j = 0; j < TamMat; j++)
		{
			vetor1[i * TamMat + j] = Mat1[i][j];
			vetor2[j * TamMat + i] = Mat2[i][j];
		}
	}
}

void MultMat()
{
	int i, j, k, l, m, n, soma;
	struct timeval tv1, tv2;
	struct timezone tz;
	double tpassado;

	converter();
	gettimeofday(&tv1, &tz);
#pragma omp parallel for private(i, j, k, l, m, n, soma) shared(MultMat_Res) schedule(static)
	for (l = 0; l < TamMat; l += TamBlock)
		for (m = 0; m < TamMat; m += TamBlock)
			for (n = 0; n < TamMat; n += TamBlock)
				for (i = l; i < menor(TamMat, l + TamBlock); i++)
					for (j = m; j < menor(TamMat, m + TamBlock); j++)
					{
						soma = 0;
						for (k = n; k < menor(TamMat, n + TamBlock); k++)
							soma += vetor1[i * TamMat + k] * vetor2[j * TamMat + k];
						MultMat_Res[i][j] = soma;
					}

	gettimeofday(&tv2, &tz);
	tpassado = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) * 0.000001;
	printf("Tempo levado para = %.3f segundos.\n", tpassado);
}

void mostrar_matrizes()
{
	int i, j;

	printf("Matriz 1: \n");
	for (i = 0; i < TamMat; i++)
	{
		for (j = 0; j < TamMat; j++)
		{
			printf("%d\t", Mat1[i][j]);
		}
		printf("\n");
	}

	printf("\nMatriz 2: \n");
	for (i = 0; i < TamMat; i++)
	{
		for (j = 0; j < TamMat; j++)
		{
			printf("%d\t", Mat2[i][j]);
		}
		printf("\n");
	}
}

void mostrar_resultado()
{
	int i, j;

	printf("\nResultado da multiplicação:\n");
	for (i = 0; i < TamMat; i++)
	{
		for (j = 0; j < TamMat; j++)
		{
			printf("%d\t", MultMat_Res[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i, j;

	srand(time(NULL));

	for (i = 0; i < TamMat; i++)
	{
		for (j = 0; j < TamMat; j++)
		{
			Mat1[i][j] = rand() % 20;
			Mat2[i][j] = rand() % 20;
		}
	}

	omp_set_num_threads(omp_get_num_threads());

	MultMat();

	// mostrar_matrizes();
	// mostrar_resultado();
}