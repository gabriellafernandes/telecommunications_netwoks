#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lista_ligada.h"

// Packets without available resources are placed in a queue. calculate:
// (A = pkt delay)
// estimator of the probability that the pkt is delayed (P(A > 0))
// estimator of the avarage delay of all packets
// histogram of the delay of delayed pkts
// estimator of the probability packet is delayed more than Ax (P(A > Ax))

#define ARRIVAL 1
#define DEPARTURE 2

#define LAMBDA 200
#define DM 0.008
#define SIZE 25
#define SPACE 0.001

char* filename = "values_delayGeneralCase_2n_6l.csv";

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


double avarage(float *array, int size)
{
    double avg = 0;
    for (int i = 0; i < size; i++)
    {
    	avg += array[i];
    }
    avg = avg / (float)size;
    return avg;
}


void histogram(float *array, int size, int* hist)
{
    int i, index;
    float max = 0;
    
    FILE* f;
    f = fopen(filename, "w+");
    if(f < 0)
    {
	printf("error opening file\n");
	exit(1);
    }
    
    for (i = 0; i < size; i++)
    {
       if (array[i] > max)
          max = array[i];
    }
    
    for (i = 0; i < size - 1; i++)
    {
       index = (int) (array[i] / (float) (SPACE));
       if(index >= SIZE)
          hist[SIZE - 1]++;
       else 
          hist[index]++;
       /*if (index < SIZE)	
           hist[index]++;*/
    }

    for (int i = 0; i < SIZE; i++)
    {
    	//printf("%d%s", hist[i], (i == SIZE - 1) ? "": "\n");
	fprintf(f, "%d%s", hist[i], (i == SIZE - 1) ? "": "\n");
    }

    fclose(f);
}

// arguments: 1: n = channels 2: s = samples 3: Ax 4: L = buff_size
int main(int argc, char* argv[]){
    if (argc < 5)
    {
        fprintf(stderr, "arguments missing!\n");
        exit(1);
    }
    
    double departure_time, arrival_time, current_time, avarage_delays, Ax; 
    int busy = 0, blocked = 0, total_arrivals = 0, delayed_events = 0, position = 0, queue = 0, pkt_loss = 0, count_ax = 0;
    int  n, samples, l;
    
    sscanf((argv[1]), "%d", &n);
    sscanf((argv[2]), "%d", &samples);
    sscanf((argv[3]), "%lf", &Ax);
    sscanf((argv[4]), "%d", &l);
    
    float *delays = (float *) malloc(sizeof(float)*samples);
    int* hist = (int *)malloc(sizeof(int)*SIZE);
    
    srand(time(NULL));
    
    lista* fila;
    fila = NULL;
    
    lista* buffer;
    buffer = NULL;
    
    // firstly, add the initial element
    fila = adicionar (fila, ARRIVAL, 0);
    
    // iterate through next elements 
    while(total_arrivals < samples)
    {
        current_time = fila->tempo;
        
    	if(fila->tipo == ARRIVAL)
    	{   
    	    total_arrivals++;
    	    
    	    // meaning there are free resources to process arrival and generate its departure
    	    if (busy < n) 
            { 
                departure_time = current_time + time_calc(DEPARTURE);
        	fila = adicionar(fila, DEPARTURE, departure_time);
        	
                busy++;           
            }
            
            // in case the system all servers are busy, we add them to the buffer            
            else if (queue < l)
            {
            	buffer = adicionar(buffer, ARRIVAL, current_time);
            	delayed_events++;
            	queue++;
            }
            else
            	pkt_loss++;
            	
            fila = remover(fila);
            
            arrival_time = current_time + time_calc(ARRIVAL);
            fila = adicionar(fila, ARRIVAL, arrival_time);
	}
	
	else if (fila->tipo == DEPARTURE)
	{
	   fila = remover(fila);
	   
	   // generate next departure in case the buffer has elements 
	   // system is then now busy
	   if (buffer != NULL)
	   {
	   	delays[position] = current_time - buffer->tempo;
	   	if (delays[position] > Ax)
	   	    count_ax++;
	   	position++;
	   	buffer = remover(buffer);
	   	
	   	departure_time = current_time + time_calc(DEPARTURE);
	   	fila = adicionar(fila, DEPARTURE, departure_time);
	   	queue--;
	   }
	   else
	   	(busy == 0) ? busy = 0 : busy--;
	}
    }
    
    if (delayed_events > 0)
    	avarage_delays = avarage(delays, delayed_events);
    else 
    	avarage_delays = 0;
    
    histogram(delays, delayed_events, hist);
    
    // results:
    
    printf("-->with the values of:\n\narrivals = %d  ||  lambda = %d  ||  dm = %.3f  ||  n = %d  ||  l = %d\n\n-->we obtain:\n", total_arrivals, LAMBDA, DM, n, l);
    
    printf("   - probability of packet loss: P(loss) = %.3f%%\n\n", 100 * pkt_loss / (float) total_arrivals);
    
    printf("   - probability of delay: P(A > 0) = %.3f%%\n\n   - avarage delay: Am = %.3fs\n\n", 100 * delayed_events / (float) total_arrivals, avarage_delays);
    
    printf("   - probability of the avarage value of a delay being greater than Ax:\n     P(A > %.3f) = %.3f%%\n", Ax, 100 * count_ax / (float) total_arrivals);
    
    free(hist);
    return 0;
}

