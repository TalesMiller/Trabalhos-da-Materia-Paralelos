#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define RTIME 500000 // Define o tempo de renovação em nós

// Valores das dimensões do tabuleiro
unsigned int largura;
unsigned int altura;

// Numero de Threads
unsigned int nthreads;

// O programa possui 2 arrays para salvar os passos
// A memoria necessaria é alicada no mais pelas dimensões dos valores
int **array1;
int **array2;

// Flag para especificar o modo bench
unsigned int bench_flag;

// Flag para especificar o dimensão do modo padrão
unsigned int dflag;

// Variáveis de estrutura para medir o tempo de execução
struct timeval t_inicio, t_final;


void inicializar_tabuleiro(int **ptratual, int dflag)
{
	// Se a flag defaut inicia com 0, ou com '0 ou 1' com valores randomicos
	int i, j;

	if (dflag)
	{
		for (i = 0; i < largura; i++)
			for (j = 0; j < altura; j++)
				ptratual[i][j] = 0;
	}
	else
	{
		for (i = 0; i < largura; i++)
			for (j = 0; j < altura; j++)
				ptratual[i][j] = rand() % 2;
	}
}

void copiar_regiao(int **ptratual, int **proxptr)
{
	int i, j;
	for (j = 0; j < altura; j++)
		for (i = 0; i < largura; i++)
			if ((i == 0) || (j == 0) || (j == altura - 1) || (i == largura - 1))
				proxptr[i][j] = ptratual[i][j];
}

// Função para retornar o número de vizinhos vivos da célula i,j.
int adjacente_a(int **ptratual, int i, int j)
{
	int linha, coluna, contador;

	contador = 0;

	// Examina todos os vizinhos
	for (linha = -1; linha <= 1; linha++)
		for (coluna = -1; coluna <= 1; coluna++)
		{
			// Exclui célula atual do contador
			if (linha || coluna)
				if (ptratual[i + linha][j + coluna])
					contador++;
			// Não precisa ficar contando se o número for >3 (sem mudança de comportamento)
			if (contador > 3)
			{ // Quebra loops aninhados
				break;
				break;
			}
		}
	return contador;
}

void play(int **ptratual, int **proxptr, int inicio, int final)
{
	int i, j, vivo;

	// Exclude board region and apply for each cell the game's rules, aplicando as regras do jogo da vida
	for (i = 1; i < largura - 1; i++)
		for (j = inicio; j < final; j++)
		{
			vivo = adjacente_a(ptratual, i, j);
			if (vivo == 2)
				proxptr[i][j] = ptratual[i][j]; /* celula vive para continuar o proximo round */
			if (vivo == 3)
				proxptr[i][j] = 1; /* celula vive, numero exato igual a 3 vizinhos */
			if (vivo < 2)
				proxptr[i][j] = 0; /* celula morre */
			if (vivo > 3)
				proxptr[i][j] = 0; /* mais de 3 vizinhos celula morre de superpopulação */
		}
}

void print(int **ptratual)
{
	int i, j;

	for (j = 0; j < altura; j++)
	{
		for (i = 0; i < largura; i++)
		{
			printf("%c", ptratual[i][j] ? 'x' : ' ');
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	// Ponteiros para cuidar dos arrays (atuais e proximos).
	// Temp é utilizada para trocar os ponteiros
	int **ptratual, **proxptr, **temp;

	int i, tid;
	unsigned int inicio, final;

	dflag = 0;
	char a;

	printf("Deseja utilizar o modo bench? ");
	scanf("%s", &a);

	if (a == 'S' || a == 's')
	{
		bench_flag = 1;
	}

	printf("Qual a altura da matriz deseja se iniciar? ");
	scanf("%d", &altura);
	printf("Qual a largura da matriz deseja se iniciar? ");
	scanf("%d", &largura);
	printf("Digite a quantidade de threads q deseja utilizar: ");
	scanf("%d", &nthreads);

	// Verifica se as dimensões para mostrar são observaveis
	if (!dflag && !bench_flag)
	{
		if ((largura > 100) || (altura > 50))
		{
			printf("cuidado!!!! muito grande para mostrar na tela.\n");
			printf("As dimensões recomendadas serem menores de 100 de largura e 50 de altura.\n");
			printf("Valores maiores recomendados para o modo bench.\n");
			printf("deseja continuar com esses valores? ");
			scanf("%s", &a);
			if (a == 'N' || a == 'n')
			{
				printf("Coloque a nova altura para a matriz ");
				scanf("%d", &altura);
				printf("Coloque a nova largura para a matriz ");
				scanf("%d", &largura);
			}
		}
	}

	// Aloca 2 arrays com as devidas dimensões
	array1 = (int **)malloc(largura * sizeof(int *));
	if (array1 == NULL)
	{
		printf("Problema na alocação de memória do array1!\n");
		printf("Saiu do programa.\n");
		return -1;
	}
	for (i = 0; i < largura; i++)
	{
		array1[i] = (int *)malloc(altura * sizeof(int));
		if (array1[i] == NULL)
		{
			printf("Problema na alocação de memória do array1!\n");
			printf("Saiu do programa.\n");
			return -1;
		}
	}

	array2 = (int **)malloc(largura * sizeof(int *));
	if (array2 == NULL)
	{
		printf("Problema na alocação de memória do array2!\n");
		printf("Saiu do programa.\n");
		return -1;
	}
	for (i = 0; i < largura; i++)
	{
		array2[i] = (int *)malloc(altura * sizeof(int));
		if (array2[i] == NULL)
		{
			printf("Problema na alocação de memória do array2!\n");
			printf("Saiu do programa.\n");
			return -1;
		}
	}

	// Ponteiros de inteiros para mostrar para os arrays revelantes
	ptratual = array1;
	proxptr = array2;

	// Inicua o arrau atual com zeros ou valores aleatórios
	// Atributo especificado pela flag padrão
	inicializar_tabuleiro(ptratual, dflag);

	// Copy the unchanged region cells
	copiar_regiao(ptratual, proxptr);

	// Mensagem para mostrar que entrou no estado Bench
	if (bench_flag)
	{
		system("clear");
		printf("==================Modo Bench==================\n");
		printf("-----------Esperando pela resposta------------\n\n");
		gettimeofday(&t_inicio, NULL);
	}

/*------Parallel omp directive for compiler------
Os ponteiros de vetores sao todos variaveis compartilhadas em todas as threads

ID da thread e limite de area sao privados
Especifica o numero de threads baseado no input do usuario
*/
#pragma omp parallel shared(ptratual, proxptr, temp, nthreads) private(tid, i, inicio, final) num_threads(nthreads)
	{
		// Obter o ID do número do thread
		tid = omp_get_thread_num();

		// Especificar limites
		inicio = tid * (altura / nthreads);
		final = inicio + (altura / nthreads);

		// Exclui celulas externas
		if (tid == 0)
			inicio++;
		if (tid == nthreads - 1)
			final = altura - 1;

		// Jogando o jogo por 100 rounds
		for (i = 0; i < 100; i++)
		{
			// Cada thread chama play() com seus próprios parametros
			play(ptratual, proxptr, inicio, final);

			//coloca uma barreira para garantir que cada thread finalizou a jogada antes de continuar
#pragma omp barrier

			//A thread com ID '0' é responsavel por trocar os ponteiros e printar o tabuleiros em cada round.
			if (tid == 0)
			{
				if ((i == 0) && (!bench_flag))
				{
					system("clear");
					print(ptratual);
					printf("------------------------------");
					printf("Estado inicial do tabuleiro");
					printf("-----------------------------\n");
					printf("   \"Defina as dimensões do console pelo menos %dx%d para ter uma visão completa do tabuleiro\"", largura, altura + 4);
					printf("\n\t\tRenove a cada %d nós  ", RTIME);
					printf("Aperte ENTER para começar o jogo!\n");
					getchar();
					system("clear");
				}
				else
				{ // Troca os ponteros
					temp = ptratual;
					ptratual = proxptr;
					proxptr = temp;

					if (!bench_flag)
					{
						print(ptratual);
						printf("----------------------------");
						printf("Estado do tabuleiro no round: %d", i);
						printf("---------------------------\n");
						usleep(RTIME);
						system("clear");
					}
				}
			} // Fim da area critica

#pragma omp barrier // Outra barreira para garantia que o ponteiro foi trocado antes do prox round
		}
	} // Final da area paralela

	// Printar o estado no caso do modo bench
	if (bench_flag)
	{
		gettimeofday(&t_final, NULL);
		printf("%ld\n", ((t_final.tv_sec * 1000000 + t_final.tv_usec) - (t_inicio.tv_sec * 1000000 + t_inicio.tv_usec)));
	}

	return 0;
}