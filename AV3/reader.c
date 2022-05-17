#include "av3.h"

int main(int arg_cont, char *argv[]){
    void *data_read;
    int items;
    int data[10];
    int max;
    int min;
    int sum;
    int avrg = 0;
    struct my_stack *stack;
    if(argv[1] == NULL){
        printf("La sintaxis correcta es ./reader 'nombre de fichero' \n" );
        exit(0);
    }
    stack = my_stack_read(argv[1]);
    if(stack == NULL){
        printf("Este fichero esta vacio, intenta con otro\n");
        exit(0);
    }
    int stack_lenght = my_stack_len(stack);
    printf("Stack length: %i \n",stack_lenght);
    for(int i = 0; i < stack_lenght; i++){
        data_read = my_stack_pop(stack);
        data[i] = *((int *) data_read);
        avrg = avrg + data[i];
        items = i+1;
        printf("Valor %i de la pila: %i\n", i+1, data[i]);
    }
    sum = avrg;
    avrg = sum/items;  //sum divided by total items is the average
    //Encontrar mayor y menor en la pila
    max = data[0];
    min = data[0];
    for(int i = 1; i < 10;i++){
        if(max < data[i]){
            max = data[i];
        }
        if(min > data[i]){
            min = data[i];
        }
    }
    printf(MAGENTA"Items: %i  "BLUE"Sum: %i  "RED"Min: %i  "GREEN"Max: %i  "YELLOW"Average: %i \n"COLOR_RESET, items,sum,min,max,avrg);
}