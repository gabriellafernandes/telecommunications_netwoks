#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lista_ligada.h"

// calculate random number between 0 and 1
double random0_1()
{
    return (rand() % RAND_MAX + 1) / (float)RAND_MAX;
}

// calculate arrival and departure time, according to call type
double time_calc(int type, int area)
{
    double u = 0, d = 0, c = 0;
    
    u = random0_1();
    
    if (type == DEPARTURE)
    {
        if (area == GP)
        {
            d = GP_min - DM_GP * log(u);
            while (d > GP_max)
            {
                u = random0_1();
                d = GP_min - DM_GP * log(u);
            }
        }
        else if (area == GP_AS)
        {
            double u1 = 0.0, u2 = 0.0, teta = 0.0;
            while (d < GP_AS_min || d > GP_AS_max)
            {
                u1 = random0_1();
                u2 = random0_1();

                teta = 2 * M_PI * u1;
                d = (sqrt(-2 * log (u2)) * cos(teta));
                d = ((d * GD_std_dev) + GD_avg);
            }
            
        }
        else if (area == AS)
        {
            d = AS_min - DM_AS * log(u);
        }

        return d;
    }
    	
    else if(type == ARRIVAL)
    {
    	c = - log(u) / (float) LAMBDA;
    	return c;
    }
}

// returns if call GP or GP_As
int call_type()
{
    double probability = random0_1();
    if (probability < 0.3)
        return GP;
    else 
        return GP_AS;
}

// returns average of an array
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

// calculate running average
double running_avg(int i, double cur_samp, double prev_avarage)
{
    return prev_avarage * ((i-1)/ (double)i) + cur_samp * (1/(double) i);
}

// generate histograms
void histogram(double *array, int size, int dist, int* hist, char* filename)
{

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
        if (array[i] < 0)
            array[i] = - array[i];
       index = (int) (array[i] / ((float) dist));
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
        fprintf(stderr, "arguments missing!\nInsert n_gp n_as samples l_gp\n\n");
        exit(1);
    }

    int  n_gp, samples, l_gp, n_as;
    
    sscanf((argv[1]), "%d", &n_gp);
    sscanf((argv[2]), "%d", &n_as);
    sscanf((argv[3]), "%d", &samples);
    sscanf((argv[4]), "%d", &l_gp);
    

    double *delays = (double*) malloc(sizeof(double)*samples);
    double *error_abs = (double*) malloc(sizeof(double)*samples);
    double *error_pos = (double*) malloc(sizeof(double)*samples);
    double *error_neg = (double*) malloc(sizeof(double)*samples);
    double *delay_as = (double*) malloc(sizeof(double)*samples);
    double *delay_as_gp = (double*) malloc(sizeof(double)*samples);
    int *hist = (int*) malloc(sizeof(int)*SIZE);
    int *hist_e_p = (int*) malloc(sizeof(int)*SIZE);
    int *hist_e_n = (int*) malloc(sizeof(int)*SIZE);

    memset(delays, 0, samples);
    memset(error_abs, 0, samples);
    memset(error_pos, 0, samples);
    memset(error_neg, 0, samples);
    memset(delay_as, 0, samples);
    memset(hist_e_p ,0, SIZE);
    memset(hist_e_n, 0, SIZE);

    srand(time(NULL));
    
    lista* events;
    events = NULL;
    
    lista* buffer;
    buffer = NULL;

    lista* buffer_as;
    buffer_as = NULL;

    char* file_delays = "histogram3a_delays.csv";
    char* file_errors = "histogram3a_errors.csv";
    char* file_errors_pos = "histogram3a_errors_pos.csv";
    char* file_errors_neg = "histogram3a_errors_neg.csv";

    int total_arrivals = 0, queue = 0, busy_gp = 0, pkt_loss = 0, delayed_events = 0, position = 0, departuresv = 0, error_pos_n = 0, error_neg_n = 0, current_type = 0, next_type = 0;
    int busy_as = 0, count_as = 0;
    double current_time = 0.0, departure_time = 0.0, arrival_time = 0.0, last_exp_delay = 0.0, last_delay = 0.0, avarage_error_abs = 0.0, avarage_delays = 0.0, avarage_error_rel;
    double departure_avg = 0.0, total_delays = 0.0, total_delays_abs = 0.0, avarage_delays_as = 0.0, avarage_sum_delays = 0.0, duration = 0.0, error = 0.0;

    // firstly, add the initial element
    events = adicionar(events, ARRIVAL, call_type(), 0, 0, 0, 0);

    while (total_arrivals < samples)
    {
        current_time = events->tempo;
        current_type = events->tipo_chamada;

        if (events->tipo == ARRIVAL)
        {
            total_arrivals++;
            // in case there are free servers => process arrival and generate its departure time
            if (busy_gp < n_gp)
            {
                duration = time_calc(DEPARTURE, current_type);
                departure_time = current_time + duration;                                    
                events = adicionar(events, DEPARTURE, current_type, departure_time, 0, current_time, duration);

                busy_gp++;
            }
            else if (queue < l_gp)
            {                
                // calculate expected delay time through running avg
                last_exp_delay = running_avg(delayed_events + 1, last_delay, last_exp_delay);
                // printf("running avg = %lf\n", last_exp_delay);

                buffer = adicionar(buffer, ARRIVAL, current_type, current_time, last_exp_delay, current_time, 0);
            	delayed_events++;
            	queue++;
            }
            else
                pkt_loss++;

            // from an arrival, we ALWAYS generate the next arrival
            next_type = call_type();
            arrival_time = current_time + time_calc(ARRIVAL, next_type);
            events = adicionar(events, ARRIVAL, next_type, arrival_time, 0, 0, 0);
        }
        else if (events->tipo == DEPARTURE)
        {
            if (buffer != NULL)
            {
                // if there is some event waiting, process it right away
                delays[position] = current_time - buffer->tempo;
                last_delay = delays[position];
                total_delays += last_delay;

                // relative and absolute errors
                error = buffer->delay_expected - last_delay;
                if (error > 0)
                {
                	error_pos[error_pos_n] = error;
                	error_pos_n++;
                }
                else 
                {
                	error_neg[error_neg_n] = error;
                	error_neg_n++;
                }

                // relative and absolute errors
                error_abs[position] = fabs(error);
                total_delays += last_delay;
                total_delays_abs += error_abs[position];

	   	        position++;

                duration = time_calc(DEPARTURE, buffer->tipo_chamada);
                departure_time = current_time + duration;                                    
                events = adicionar(events, DEPARTURE, buffer->tipo_chamada, departure_time, 0, buffer->arrival_time, duration);

                buffer = remover(buffer);
	   	        queue--;
            }
            else 
                (busy_gp == 0) ? busy_gp = 0 : busy_gp--;

            // if event is GP_AS, it has to trigger the following AS event
            if (events->tipo_chamada == GP_AS)
            {
                if (busy_as < n_as)
                {
                    delay_as[count_as-1] = current_time - events->arrival_time;
                    avarage_sum_delays += (delay_as[count_as-1] - events->duration_GP);

                    departure_time = current_time + time_calc(DEPARTURE, AS);
                    events = adicionar(events, DEPARTURE, AS, departure_time, 0, 0, 0);

                    busy_as++;
                    count_as++;
                }  
                else 
                    buffer_as = adicionar(buffer_as, ARRIVAL, AS, current_time, 0, events->arrival_time, 0);   
            }

            // process AS calls
            else if (events->tipo_chamada == AS)
            {
                if (buffer_as != NULL)
                {
                    delay_as[count_as-1] = current_time - buffer_as->arrival_time;
                    delay_as_gp[count_as-1] = (delay_as[count_as-1] - buffer_as->duration_GP);
                    
                    departure_time = current_time + time_calc(DEPARTURE, AS);
                    events = adicionar(events, DEPARTURE, AS, departure_time, 0, 0, 0);

                    buffer_as = remover(buffer_as);
                }
                else 
                    (busy_as == 0) ? busy_as = 0 : busy_as--;
            }
        }

        events = remover(events);

    }

    histogram(delays, delayed_events, SPACE, hist, file_delays);
    histogram(error_abs, delayed_events, SPACE_ERRORS, hist, file_errors);
    histogram(error_neg, error_neg_n, SPACE_ERRORS, hist_e_n, file_errors_neg);
    histogram(error_pos, error_pos_n, SPACE_ERRORS, hist_e_p, file_errors_pos);
    avarage_delays = avarage(delays, delayed_events);
    avarage_error_abs = avarage(error_abs, delayed_events);
    avarage_delays_as = avarage(delay_as, count_as);
    avarage_sum_delays = avarage(delay_as_gp, count_as);


    // results:
    printf(" with the values of:\n\n  arrivals = %d  ||  lambda = %.3lf  ||  dm = %d\n  n_gp = %d  ||  l_gp = %d  ||  n_as = %d\n\n\n\n we obtain:\n\n", total_arrivals, LAMBDA, DM_GP, n_gp, l_gp, n_as);
    printf(" --> probability of packet loss: P(loss) = %.3f%%\n\n", 100 * pkt_loss / (float) total_arrivals);
    printf(" --> probability of delay: P(A > 0) = %.3f%%\n\n --> avarage delay: Am = %.3lfs\n\n", 100 * delayed_events / (float) total_arrivals, avarage_delays);
    printf(" --> last predicted delay value = %.3fs\n\n", last_exp_delay);
    printf(" --> avarage of absolute prediction error = %.3fs\n\n", avarage_error_abs);
    printf(" --> avarage of relative prediction error = %.3f%%\n\n", 100 * avarage_error_abs / (float) avarage_delays);
    printf(" --> avarage of delay to answer area specific call = %.3fs\n\n", avarage_delays_as);
    printf(" --> avarage of sum of delays until call is answered by area specific operator = %.3fs\n\n", avarage_sum_delays);
    // printf(" --> number of area specific calls out of the %d calls taken = %d\n\n", total_arrivals - pkt_loss - delayed_events, count_as);
}
