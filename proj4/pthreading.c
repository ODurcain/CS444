#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <getopt.h>
#include <pthread.h>

#define MAX_CUSTOMERS 1000

// pthread initializations. Allows for synchronization actions
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_customer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_server = PTHREAD_COND_INITIALIZER;

int queue_length = 0;
int customers = 0;
int customers_served = 0;
double arrival_time = 0;
double wait_time = 0;
double service_time = 0;
double total_queue_length = 0;
double occupied_time = 0;

// Command line defaults
double lambda = 5.0;
double mu = 7.0;
int customer_capacity = 1000;
int server_count = 1;

// Arrays to store individual data points for calculations
double interarrival_times[MAX_CUSTOMERS];
double waiting_times[MAX_CUSTOMERS];
double service_times[MAX_CUSTOMERS];
int queue_lengths[MAX_CUSTOMERS];

// Random number between 0 and 1
double rand_01(struct drand48_data *randData)
{
    double res;
    drand48_r(randData, &res);
    return res;
}

// exponential distributoion function to be used in threads 1 and 2
double rndExp(double lambda, struct drand48_data *randData)
{
    double tmp;
    drand48_r(randData, &tmp);
    return -log(1.0 - tmp) / lambda;
}

// Thread 1 customer generation
void *generate_customers(void *arg)
{
    struct drand48_data randData;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand48_r(tv.tv_sec + tv.tv_usec, &randData);

    struct timespec sleepTime;

    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 0;

    while (customers < customer_capacity)
    {
        double local_arrival_time = rndExp(lambda, &randData);

        sleepTime.tv_sec = (time_t)local_arrival_time;
        sleepTime.tv_nsec = (long)((local_arrival_time - sleepTime.tv_sec) * 1e9);

        nanosleep(&sleepTime, NULL);

        pthread_mutex_lock(&mutex);

        queue_length++;
        if (queue_length == 1)
        {
            pthread_cond_signal(&cond_server);
        }
        pthread_mutex_unlock(&mutex);
        customers++;
        arrival_time += local_arrival_time;
        wait_time += queue_length * local_arrival_time;

        interarrival_times[customers - 1] = local_arrival_time;
        waiting_times[customers - 1] = queue_length * local_arrival_time;
    }
    pthread_exit(NULL);
}

// Thread 2 server simulation
void *server(void *arg)
{
    struct drand48_data randData;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand48_r(tv.tv_sec + tv.tv_usec, &randData);

    struct timespec sleepTime;

    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 0;

    while (customers_served < customer_capacity)
    {
        pthread_mutex_lock(&mutex);

        while (queue_length == 0)
        {
            // If the queue is empty, wait for a signal from Thread 1
            pthread_cond_wait(&cond_server, &mutex);
        }

        // Queue is not empty, serve a customer
        queue_length--;

        pthread_mutex_unlock(&mutex);

        // Simulate service time (drawn from exponential distribution)
        double local_service_time = rndExp(mu, &randData);

        // Convert service_time to timespec format
        sleepTime.tv_sec = (time_t)local_service_time;
        sleepTime.tv_nsec = (long)((local_service_time - sleepTime.tv_sec) * 1e9);

        nanosleep(&sleepTime, NULL);

        pthread_mutex_lock(&mutex);

        // Update statistics
        customers_served++;
        service_time += local_service_time;
        occupied_time += local_service_time;

        service_times[customers_served - 1] = local_service_time;

        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

// Thread 3 observing queue length
void *check_queue_length(void *arg)
{
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 5000000L;
    while (customers_served < customer_capacity)
    {
        pthread_mutex_lock(&mutex);
        total_queue_length += queue_length;
        pthread_mutex_unlock(&mutex);
        nanosleep(&sleepTime, NULL);

        queue_lengths[customers_served - 1] = queue_length;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int option;

    while ((option = getopt(argc, argv, "l:m:c:s:")) != -1)
    {
        switch (option)
        {
        case 'l':
            lambda = atof(optarg);
            break;
        case 'm':
            mu = atof(optarg);
            break;
        case 'c':
            customer_capacity = atoi(optarg);
            break;
        case 's':
            server_count = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -l lambda -m mu -c numCustomer -s numServer\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (lambda >= mu * server_count)
    {
        fprintf(stderr, "Error. Is not λ < µ x numServer");
        exit(EXIT_FAILURE);
    }

    pthread_t thread1, thread2, thread3;

    pthread_create(&thread1, NULL, generate_customers, NULL);
    pthread_create(&thread2, NULL, server, NULL);
    pthread_create(&thread3, NULL, check_queue_length, NULL);

    // Start time of the simulation
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // End time of the simulation
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    // Calculate total time of the simulation in seconds

    double total_time = (end_time.tv_sec - start_time.tv_sec) +
                        (end_time.tv_usec - start_time.tv_usec) / 1e6;

    // Calculate means
    double mean_interarrival_time = arrival_time / customers;
    double mean_waiting_time = wait_time / customers;
    double mean_service_time = service_time / customers_served;
    double mean_queue_length = total_queue_length / (customers_served * 0.005);
    double server_utilization = occupied_time / total_time;

    // Calculate standard deviations
    double M2_interarrival = 0;
    double M2_waiting = 0;
    double M2_service = 0;
    double M2_queue_length = 0;

    for (int i = 0; i < customers; i++)
    {
        double delta_interarrival = interarrival_times[i] - mean_interarrival_time;
        double delta_waiting = waiting_times[i] - mean_waiting_time;
        M2_interarrival += delta_interarrival * (interarrival_times[i] - mean_interarrival_time);
        M2_waiting += delta_waiting * (waiting_times[i] - mean_waiting_time);
    }

    for (int i = 0; i < customers_served; i++)
    {
        double delta_service = service_times[i] - mean_service_time;
        double delta_queue_length = queue_lengths[i] - mean_queue_length;
        M2_service += delta_service * (service_times[i] - mean_service_time);
        M2_queue_length += delta_queue_length * (queue_lengths[i] - mean_queue_length);
    }

    double std_dev_interarrival_time = sqrt(M2_interarrival / customers);
    double std_dev_waiting_time = sqrt(M2_waiting / customers);
    double std_dev_service_time = sqrt(M2_service / customers_served);
    double std_dev_queue_length = sqrt(M2_queue_length / (customers_served * 0.005));

    // Print means
    printf("Mean Interarrival Time: %f\n", mean_interarrival_time);
    printf("Mean Waiting Time: %f\n", mean_waiting_time);
    printf("Mean Service Time: %f\n", mean_service_time);
    printf("Mean Queue Length: %f\n", mean_queue_length);

    // Print standard deviations
    printf("Standard Deviation Interarrival Time: %f\n", std_dev_interarrival_time);
    printf("Standard Deviation Waiting Time: %f\n", std_dev_waiting_time);
    printf("Standard Deviation Service Time: %f\n", std_dev_service_time);
    printf("Standard Deviation Queue Length: %f\n", std_dev_queue_length);

    printf("Server Utilization: %f%%\n", (server_utilization * 100));

    return 0;
}
