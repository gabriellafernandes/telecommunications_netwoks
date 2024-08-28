#include<stdio.h>
#include<stdlib.h>

#define ARRIVAL 0
#define DEPARTURE 1
#define GP 0
#define GP_AS 1
#define AS 2

// 80 calls/h = 80/3600 calls/s
#define LAMBDA (80 / (double) 3600)
#define SIZE 25
// space = 1 / 5 * lambda
#define SPACE 9
#define SPACE_ERRORS 4.5

// General-Purpose
#define DM_GP 120
#define GP_max 300
#define GP_min 60

// Area-specific
#define DM_AS 150
#define AS_min 60

// GP Area-specific
#define GP_AS_max 120
#define GP_AS_min 30

// Gaussian distribution
#define GD_avg 60
#define GD_std_dev 20

// Probabilities
#define P_GP 0.3
#define P_AS 0.7


typedef struct{
	int tipo; // type of event 
	int tipo_chamada; // type of call
	double tempo; // acts as time of departure or arrival, or simply to pass current time, depending on the event
	double delay_expected; // to transmit calculated expected delay 
	double arrival_time; // to calculate delays of Area Specific events, with GP_AS call duration 
	double duration_GP; // to later calculate sum of delays of Area Specific events
	struct lista * proximo;
} lista;


// Funcao que remove o primeiro elemento da lista 
// retorna novo primeiro elemento
lista * remover (lista * apontador)
{
	lista * lap = (lista *)apontador -> proximo;
	free(apontador);
	return lap;
}

// Funcao que adiciona novo elemento a lista, ordenando a mesma por tempo

lista * adicionar (lista * apontador, int n_tipo, int n_tipo_chamada, double n_tempo, double n_delay_expected, double n_arrival_time, double n_duration_GP)
{
	lista * lap = apontador;
	lista * ap_aux, * ap_next;
	if(apontador == NULL)
	{
		apontador = (lista *) malloc(sizeof (lista));
		apontador -> proximo = NULL;
		apontador -> tipo = n_tipo;
		apontador -> tipo_chamada = n_tipo_chamada;
		apontador -> delay_expected = n_delay_expected;
		apontador -> arrival_time = n_arrival_time;
		apontador -> duration_GP = n_duration_GP;
		apontador -> tempo = n_tempo;
		return apontador;
	}
	else
	{
		if (apontador->tempo > n_tempo) {
	        ap_aux = (lista *) malloc(sizeof (lista));
	        ap_aux -> tipo = n_tipo;
            ap_aux -> tempo = n_tempo;
			ap_aux -> tipo_chamada = n_tipo_chamada;
			ap_aux -> delay_expected = n_delay_expected;
			ap_aux -> arrival_time = n_arrival_time;
			ap_aux -> duration_GP = n_duration_GP;
            ap_aux -> proximo = (struct lista *) apontador;
            return ap_aux;
	    }

		ap_next = (lista *)apontador -> proximo;
		while(apontador != NULL)
		{
			if((ap_next == NULL) || ((ap_next -> tempo) > n_tempo))
				break;
			apontador = (lista *)apontador -> proximo;
			ap_next = (lista *)apontador -> proximo;
		}
		ap_aux = (lista *)apontador -> proximo;
		apontador -> proximo = (struct lista *) malloc(sizeof (lista));
		apontador = (lista *)apontador -> proximo;
		if(ap_aux != NULL)
			apontador -> proximo = (struct lista *)ap_aux;
		else
			apontador -> proximo = NULL;
		apontador -> tipo = n_tipo;
		apontador -> tipo_chamada = n_tipo_chamada;
		apontador -> tempo = n_tempo;
		apontador -> delay_expected = n_delay_expected;
		apontador -> arrival_time = n_arrival_time;
		apontador -> duration_GP = n_duration_GP;
		return lap;
	}
}

// Funcao que imprime no ecra todos os elementos da lista
void imprimir (lista * apontador)
{
	if(apontador == NULL)
		printf("Lista vazia!\n");
	else
	{
		while(apontador != NULL)
		{
			printf("Tipo=%d\tTipo chamada=%d\tTempo=%lf\tAtraso esperado=%lf\nDelay AS=%lf\nDuration %lf\n", apontador -> tipo, apontador -> tipo_chamada, apontador -> tempo, apontador -> delay_expected, apontador -> arrival_time, apontador->duration_GP);
			apontador = (lista *)apontador -> proximo;
		}
	}
}
