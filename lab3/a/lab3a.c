#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lista_ligada.h"

#define ARRIVAL 0
#define DEPARTURE 1

// 800 calls/h = 80/3600 calls/s
#define LAMBDA (80/ (double) 3600)
#define SIZE 25 
// space = 1 / 5 * lambda
#define SPACE 9

// General-Purpose
#define DM_GP 120
#define GP_max 300
#define GP_min 60

//  calculate value of the departure or arrival time
double time_calc(int type)
{
    double u = 0, d = 0, c = 0;
    
    u = (rand() % RAND_MAX + 1) / (float)RAND_MAX;
    
    if (type == DEPARTURE)
    {
    	while (d > GP_max || d < GP_min)
        {
            u = (rand() % RAND_MAX + 1) / (float)RAND_MAX;
            d = - DM_GP * log(u);
        }

    	return d;
    }
    	
    else 
    {
    	c = - log(u) / (float) LAMBDA;
    	return c;
    }
}

// calculate avarage value of the values of an array
double avarage(double *array, int size)
{
    double avg = 0;
    for (int i = 0; i < size; i++)
    {
    	avg += array[i];
    }

    if (size == 0)
        return 0;
        
    avg = avg / (float)size;

    return avg;
}

// calculate the estimated value for the next delay time of a packet
double running_avg(int i, double cur_samp, double prev_avarage)
{
    return prev_avarage * ((i-1) / (double) i) + cur_samp * (1 / (double) i);
}

// generate histograms
void histogram(double *array, int size, int* hist, char* filename)
{
    memset(hist, 0, SIZE);
    int i, index;
    
    FILE* f;
    f = fopen(filename, "w+");
    if(f < 0)
    {
        printf("error opening file\n");
        exit(1);
    }
    
    for (i = 0; i < size - 1; i++)
    {
       index = (int) (array[i] / ((float) SPACE));
       if(index >= SIZE)
          hist[SIZE - 1]++;
       else 
          hist[index]++;
       /*if (index < SIZE)	
           hist[index]++;*/
    }

    for (int i = 0; i < SIZE; i++)
    {
    	// printf("%d%s", hist[i], (i == SIZE - 1) ? "": "\n");
	    fprintf(f, "%d%s", hist[i], (i == SIZE - 1) ? "": "\n");
    }

    fclose(f);
}


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "arguments missing!\nInsert n samples buffer size\n\n");
        exit(1);
    }

    int  n, samples, l;
    
    sscanf((argv[1]), "%d", &n);
    sscanf((argv[2]), "%d", &samples);
    sscanf((argv[3]), "%d", &l);
    

    double *delays = (double*) malloc(sizeof(double)*samples);
    double *departures = (double*) malloc(sizeof(double)*samples);
    int* hist = (int *)malloc(sizeof(int)*SIZE);
    double *error_rel = (double*) malloc(sizeof(double)*samples);
    double *error_abs = (double*) malloc(sizeof(double)*samples);
    double *error_pos = (double*) malloc(sizeof(double)*samples);
    double *error_neg = (double*) malloc(sizeof(double)*samples);

    memset(delays, 0, samples);
    memset(departures, 0, samples);
    memset(hist, 0, SIZE);
    memset(error_abs, 0, samples);
    memset(error_rel, 0, samples);
    memset(error_pos, 0, samples);
    memset(error_neg, 0, samples);

    srand(time(NULL));
    
    lista* events_gp;
    events_gp = NULL;
    
    lista* buffer;
    buffer = NULL;

    char* file_delays = "histogram3a_delays.csv";
    char* file_errors = "histogram3a_errors.csv";
    char* file_errors_pos = "histogram3a_errors_pos.csv";
    char* file_errors_neg = "histogram3a_errors_neg.csv";

    int total_arrivals = 0, queue = 0, busy = 0, pkt_loss = 0, delayed_events = 0, position = 0, departuresv = 0, error_pos_n = 0, error_neg_n = 0;
    double current_time = 0.0, departure_time = 0.0, arrival_time = 0.0, last_exp_delay = 0.0, last_delay = 0.0, avarage_error_abs = 0.0, avarage_error_rel = 0.0, avarage_delays = 0.0;
    double departure_avg = 0.0, total_delays = 0.0;

    // firstly, add the initial element
    events_gp = adicionar(events_gp, ARRIVAL, 0, 0);

    while (total_arrivals < samples)
    {
        current_time = events_gp->tempo;

        if (events_gp->tipo == ARRIVAL)
        {
            total_arrivals++;
            // in case there are free servers => process arrival and generate its departure time
            if (busy < n)
            {
                departure_time = current_time + time_calc(DEPARTURE);
                events_gp = adicionar(events_gp, DEPARTURE, departure_time, 0);

                busy++;

                departures[departuresv] = departure_time - current_time;
                departuresv++;
            }
            else if (queue < l)
            {
                // calculate expected delay time through running avg
                last_exp_delay = running_avg(delayed_events + 1, last_delay, last_exp_delay);
                buffer = adicionar(buffer, ARRIVAL, current_time, last_exp_delay);
            	delayed_events++;
            	queue++;
            }
            else
                pkt_loss++;

            // from an arrival, we ALWAYS generate the next arrival
            arrival_time = current_time + time_calc(ARRIVAL);
            events_gp = adicionar(events_gp, ARRIVAL, arrival_time, 0);
        }
        else if (events_gp->tipo == DEPARTURE)
        {
            if (buffer != NULL)
            {
                // if there is some event waiting, process it right away
                delays[position] = current_time - buffer->tempo;
                last_delay = delays[position];
                total_delays += last_delay;

                // relative and absolute errors
                if (buffer->delay_expected - last_delay > 0)
                {
                	error_pos[error_pos_n] = buffer->delay_expected - last_delay;
                	error_pos_n++;
                }
                else
                {
                	error_neg[error_neg_n] = buffer->delay_expected - last_delay;
                	error_neg_n++;
                }
                	
                error_abs[position] = fabs(buffer->delay_expected - last_delay);
                error_rel[position] = error_abs[position] / (double) last_delay; 

	   	        position++;
	   	        departure_time = current_time + time_calc(DEPARTURE);
                events_gp = adicionar(events_gp, DEPARTURE, departure_time, 0);

                departures[departuresv] = departure_time - current_time;
                departuresv++;

                buffer = remover(buffer);
	   	        queue--;
            }
            else 
                (busy == 0) ? busy = 0 : busy--;
        }

        events_gp = remover(events_gp);
    }

    histogram(delays, delayed_events, hist, file_delays);
    histogram(error_abs, delayed_events, hist, file_errors);
    histogram(error_pos, error_pos_n, hist, file_errors_pos);
    histogram(error_neg, error_neg_n, hist, file_errors_neg);
    avarage_delays = avarage(delays, delayed_events);
    avarage_error_abs = avarage(error_abs, delayed_events);
    avarage_error_rel = avarage(error_rel, delayed_events);
    departure_avg = avarage(departures, departuresv);

    // results:
    printf("\n--> with the values of:\n\n   arrivals = %d  ||  lambda = %.3lf  ||  dm = %d  ||  n = %d  ||  l = %d\n\n-->we obtain:\n\n\n", total_arrivals, LAMBDA, DM_GP, n, l);
    printf(" --> probability of packet loss: P(loss) = %.3f%%\n\n", 100 * pkt_loss / (float) total_arrivals);
    printf(" --> probability of delay: P(A > 0) = %.3f%%\n\n --> avarage delay: Am = %.3fs\n\n", 100 * delayed_events / (float) total_arrivals, avarage_delays);
    printf(" --> last predicted delay = %.3fs\n\n", last_exp_delay);
    printf(" --> avarage of absolute prediction error = %.3fs\n\n", avarage_error_abs);
    printf(" --> avarage of relative prediction error = %.3f%%\n\n", 100 * avarage_error_rel);
    printf(" --> avarage call duration = %.3fs\n\n", departure_avg);

    free(delays);
    free(error_neg);
    free(error_pos);
    free(error_abs);
    free(error_rel);
    free(departures);
}
