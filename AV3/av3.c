#include "av3.h"
#define NUM_THREADS 10
#define REPEAT 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct my_stack *stack;

void *execute_order_66()
{
    void *data;
    int *adder;
    for (int i = 0; i < REPEAT; i++)
    {
        pthread_mutex_lock(&mutex);     //start of critical zone
        data = my_stack_pop(stack);     //pop existing data
        pthread_mutex_unlock(&mutex);   //end of critical zone
        data = data - 3;                //for adder fix
        adder = (int *)data;            //int for numbers
        adder++;
        data = adder;                   //store new data
        pthread_mutex_lock(&mutex);     //start of critical zone
        my_stack_push(stack, data);     //push modified data
        pthread_mutex_unlock(&mutex);   //end of critical zone
    }
    pthread_exit(0);
}

int main(int arg_cont, char *argv[])
{
    int *data;
    pthread_t threads[NUM_THREADS];

    if (argv[1] == NULL)
    {
        printf("The right syntax is: ./av3 'filename' \n");
        exit(0);
    }
    printf("Threads: %i, Iterations: %i \n", NUM_THREADS, REPEAT);
    sleep(0.5);
    stack = my_stack_read(argv[1]);
    if (stack == NULL)
    {
        printf("Initial stack lenght: 0\n");
        stack = my_stack_init(4);                                           //4 = int size
        for (int i = 0; i < 10; i++)
        {
            data = malloc(sizeof(int));
            data = 0;
            my_stack_push(stack, data);
        }
    }
    else
    {
        printf("Initial stack lenght: %i\n",my_stack_len(stack));
        if (my_stack_len(stack) < NUM_THREADS)
        {
            for (int i = my_stack_len(stack); i < NUM_THREADS; i++)
            {
                data = malloc(sizeof(int));
                data = 0;
                my_stack_push(stack, data);
            }
        }
    }
    printf("Final stack lenght: %i\n",my_stack_len(stack));
    printf("Creating %i threads, this could take a while \n",NUM_THREADS);
    sleep(0.5);
    //Empezamos a crear los threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, &execute_order_66, NULL);
        printf("%i) Thread %li created\n", i, threads[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("Ending threads, storing stack.. \n");
    int elements = my_stack_write(stack, argv[1]);
    if (elements != 10)
    {
        printf("Write to stack: Failed!\n");
        exit(0);
    }
    printf("Released bytes: %i \n", my_stack_purge(stack));
    printf("Write to stack: Success! \nWritten elements: %i \n",elements);
    printf("\nBye from main\n");
    exit(0);
}