#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lista_ligada.h"

#define ARRIVAL 1
#define DEPARTURE 2

#define LAMBDA 200
#define DM 0.008


double time_calc(int type)
{
    double u, d, c;
    
    u = (rand() % RAND_MAX + 1) / (float)RAND_MAX;
    
    if (type == DEPARTURE)
    {
    	d = -DM * log(u);
    	return d;
    }
    	
    else 
    {
    	c = - log(u) / (float) LAMBDA;
    	return c;
    }
}


// arguments: 1: n = channels || 2: s = samples
int main(int argc, char* argv[]){
    if (argc < 3)
    {
        fprintf(stderr, "arguments missing!\n");
        exit(1);
    }
    
    double departure_time, arrival_time, current_time; 
    int n, samples, busy = 0, blocked = 0, total_arrivals = 0;
    
    sscanf((argv[1]), "%d", &n);
    sscanf((argv[2]), "%d", &samples);
    
    
    srand(time(NULL));
    
    lista* fila;
    fila = NULL;
    
    // firstly, add the initial element
    fila = adicionar (fila, ARRIVAL, 0);
    
    // iterate through next elements 
    while(total_arrivals < samples)
    {
        current_time = fila->tempo;
        
        // even when the system is blocked a new event is generated
    	if(fila->tipo == ARRIVAL)
    	{   
    	    total_arrivals ++;
    	    if (busy < n) //meaning there are free resources to process arrival and generate its departure
            { 
                fila = remover (fila);
                
                departure_time = current_time + time_calc(DEPARTURE);
        	fila = adicionar(fila, DEPARTURE, departure_time);
        	
                busy ++;           
            }
            
            else
            {
            	fila = remover (fila);
            	blocked++;
            }
            arrival_time = current_time + time_calc(ARRIVAL);
            fila = adicionar(fila, ARRIVAL, arrival_time);
	}
	
	else if (fila->tipo == DEPARTURE)
	{
	   fila = remover (fila);
	   (busy == 0) ? busy = 0 : busy--;
	}
    }
    
    
    printf("with the values of\n\narrivals = %d  ||  lambda = %d  ||  dm = %f  ||  n = %d\n\nwe obtain a blocking probability of %f%%\n\n", 
    	  total_arrivals, LAMBDA, DM, n, (blocked * 100 /(float) total_arrivals));
    
    return 0;
}

