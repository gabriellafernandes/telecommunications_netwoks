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

char* filename = "histograma1a.csv";

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
    for (int i = 0; i < SIZE; i++)
    {
	printf("%d - %d%s", i+1, value[i], "\n");
    }
}


int main(){
    int* hist = (int *)malloc(sizeof(int)*SIZE);
    double u, c, media, total = 0, tempo = 0, n_tempo;
    int position = 0;
    lista* fila;
    fila = NULL;

    srand(time(NULL));
    
    for (int i = 0; i < ARRIVALS; i++)
    {
	u = (rand() % RAND_MAX + 1) / (float)RAND_MAX;
        c = - log(u) / (float) LAMBDA;
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
    
    
    media = total / (float)ARRIVALS;
    
    print_hist(hist);
    printf("\n-> estimador do valor medio entre chegadas de eventos: %f\n\n", media);
    write_csv(hist);
    free(hist);
    return 0;
}

