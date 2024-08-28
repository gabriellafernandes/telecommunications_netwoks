#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lista_ligada.h"

#define LAMBDA 5
#define SIZE 25
#define SPACE 0.04
#define ARRIVALS 100000
#define CHEGADA 1
#define DELTA 0.001
// delta x lambda = probabilidade de um evento chegar
// delta sao os intervalos (consideramos 1ms)

char* filename = "histograma1b.csv";

void write_csv(int* values)
{
    FILE* f;
    f = fopen(filename, "w+");
    if(f < 0)
    {
	printf("error opening file\n");
	exit(1);
    }

    for (int i = 0; i < SIZE; i++)
    {
	fprintf(f, "%d%s", values[i], (i == SIZE - 1) ? "": "\n");
    }

    fclose(f);
}


void print_hist(int* value)
{
    int verifica = 0;
    for (int i = 0; i < SIZE; i++)
    {
	printf("%d - %d%s", i+1, value[i], "\n");
	verifica += value[i];
    }
}


int main(){
    int* hist = (int *)malloc(sizeof(int)*SIZE);
    double probabilidade, c, media, total = 0;
    double tempo = 0, tempo_chegada = 0;
    int position = 0, eventos_validos = 0;
    lista* fila;
    fila = NULL;

    srand(time(NULL));
    
    while (eventos_validos <= ARRIVALS)
    {
    	tempo += DELTA;
    	probabilidade = (rand() % RAND_MAX + 1) / (float)RAND_MAX;
    	if (probabilidade <= DELTA*LAMBDA)
    	{
	    eventos_validos++;
	    // verificamos se continua a calcular um intervalo ou se iniciamos outro
	    
	    if (tempo_chegada == 0)
	    	tempo_chegada = tempo;
	    else 
	    {
	    	c = tempo - tempo_chegada; 
	    	tempo_chegada = tempo;
	    	fila = adicionar (fila, CHEGADA, c);
	    	
	    	total += fila->tempo;

		position = (int) (fila->tempo / (float) SPACE);
		/*if (position >= SIZE)	
	    	    hist[SIZE - 1]++;
		else*/
                if (position < SIZE)
		   hist[position]++;
	 
		fila = remover (fila);
	    }
    	}
	     
    }
    
    media = total / (float)ARRIVALS;
    
    print_hist(hist);
    printf("\n-> estimador do valor medio entre chegadas de eventos: %f\n\n", media);
    write_csv(hist);
    free(hist);
    return 0;
}

