#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int *a, int *b, int l, int m, int r)
{

	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while ((h <= m) && (j <= r))
	{

		if (a[h] <= a[j])
		{

			b[i] = a[h];
			h++;
		}

		else
		{

			b[i] = a[j];
			j++;
		}

		i++;
	}

	if (m < h)
	{

		for (k = j; k <= r; k++)
		{

			b[i] = a[k];
			i++;
		}
	}

	else
	{

		for (k = h; k <= m; k++)
		{

			b[i] = a[k];
			i++;
		}
	}

	for (k = l; k <= r; k++)
	{

		a[k] = b[k];
	}
}

void mergeSort(int *a, int *b, int l, int r)
{

	int m;

	if (l < r)
	{

		m = (l + r) / 2;

		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);
	}
}

void print_Vetor(int *a,int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d ",a[i]);
	}
		printf("\n");
	
}

int main(int argc, char **argv)
{

	/* Cria e popula vetor, com 20 valores */
	int n = 20;
	int *vetor_original = malloc(n * sizeof(int));

	int c;
	srand(time(NULL));
	for (c = 0; c < n; c++)
	{
		vetor_original[c] = rand() % n;
	}


	/* inicializa mpi */
	int ranqueado;
	int tamanho_mundo;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &ranqueado);
	MPI_Comm_size(MPI_COMM_WORLD, &tamanho_mundo);

	if (ranqueado == 0)
	{
		printf("Vetor desordenado: ");
		print_Vetor(vetor_original,n);
	}
		

	/* Divide vetor em tamanhos iguais */
	int tamanho = n / tamanho_mundo;

	/* Manda cada subvetor p cada processo/thread */
	int *sub_vetor = malloc(tamanho * sizeof(int));
	MPI_Scatter(vetor_original, tamanho, MPI_INT, sub_vetor, tamanho, MPI_INT, 0, MPI_COMM_WORLD);

	/* faz o mergesort em cada processo/thread */
	int *tmp_vetor = malloc(tamanho * sizeof(int));
	mergeSort(sub_vetor, tmp_vetor, 0, (tamanho - 1));

	/* Gather os vetores ordenados em um so */
	int *ordenado = NULL;
	if (ranqueado == 0)
		ordenado = malloc(n * sizeof(int));

	MPI_Gather(sub_vetor, tamanho, MPI_INT, ordenado, tamanho, MPI_INT, 0, MPI_COMM_WORLD);

	if (ranqueado == 0)
	{
		int *vetor_qualquer = malloc(n * sizeof(int));
		for(int i = 0; i < n;i+=tamanho)
		{
			merge(ordenado,vetor_qualquer,0,i-1,i+tamanho-1);
		}

		/* mostrando vetor ordenado */
		printf("Vetor ordenado: ");
		print_Vetor(ordenado,n);
	}

	MPI_Finalize();
}