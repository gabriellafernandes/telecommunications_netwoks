#include<stdio.h>
#include<stdlib.h>

#define CHEGADA 1

// Definicao da estrutura da lista
typedef struct{
	int tipo; // tipo de evento
	double tempo; // tempo em que ocorreu o evento
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

lista * adicionar (lista * apontador, int n_tipo, double n_tempo)
{
	lista * lap = apontador;
	lista * ap_aux, * ap_next;
	if(apontador == NULL)
	{
		apontador = (lista *) malloc(sizeof (lista));
		apontador -> proximo = NULL;
		apontador -> tipo = n_tipo;
		apontador -> tempo = n_tempo;
		return apontador;
	}
	else
	{
		if (apontador->tempo > n_tempo) {
	        ap_aux = (lista *) malloc(sizeof (lista));
	        ap_aux -> tipo = n_tipo;
            ap_aux -> tempo = n_tempo;
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
		apontador -> tempo = n_tempo;
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
			printf("Tipo=%d\tTempo=%lf\n", apontador -> tipo, apontador -> tempo);
			apontador = (lista *)apontador -> proximo;
		}
	}
}
/*
// Pequeno exemplo de utilizacao
int main(void)
{
	lista  * lista_eventos;
	int tipo_ev; double tempo_ev;
	lista_eventos = NULL;
	lista_eventos = adicionar(lista_eventos, 1, 0.6);
	lista_eventos = adicionar(lista_eventos, 0, 0.4);
	lista_eventos = adicionar(lista_eventos, 1, 0.3);
	lista_eventos = adicionar(lista_eventos, 2, 0.5);
	lista_eventos = adicionar(lista_eventos, 1, 0.5);
	lista_eventos = adicionar(lista_eventos, 0, 0.2);
	lista_eventos = adicionar(lista_eventos, 1, 0.1);
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	if ( lista_eventos != NULL)
	{
		tipo_ev = lista_eventos -> tipo;
		tempo_ev = lista_eventos -> tempo;
		lista_eventos = remover(lista_eventos);
		printf("\nEVENTO REMOVIDO\n");
		printf("Tipo=%d\tTempo=%lf\n", tipo_ev, tempo_ev);
	}
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);

	lista_eventos = adicionar(lista_eventos, 2, 0.5);
	lista_eventos = adicionar(lista_eventos, 1, 0.3);
	printf("\nLISTA ACTUAL\n");
	imprimir(lista_eventos);
}*/


