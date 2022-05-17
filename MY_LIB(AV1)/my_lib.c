//Desarrolladores del código: Bartomeu Capó Salas y Emanuel Hegedus
#include "my_lib.h"

//my_stdio
size_t my_strlen(const char *str){ //Devuelve la longitud del str que recibe
    size_t length = 0;
    for(int i = 1; str[i-1] != '\0'; i++){
        length = i;
    }
    return length;
}

int my_strcmp(const char *str1, const char *str2){ //Compara los valores de las letras de ambas palabras del parametro
    int solucion = 0;
    int i = 0;
    while(solucion == 0 && str1[i] != '\0' && str2[i] != '\0'){
        solucion = ((int)str1[i] - (int)str2[i]); 
        i++;
    }
    return solucion;
}

char *my_strcpy(char *dest, const char *src){ //Copia en dest la palabra en src
    int i = 0;
    while(i < (int) my_strlen(src)){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n){ //Copia en dest la palabra en src pero n longitud
    for(int i = 0; i < (int) n; i++){
        dest[i] = src[i];
    }
    dest[n] = '\0';
    return dest;
}

char *my_strcat(char *dest, const char *src){ //Concatena dest con src, es decir, añade src al final de dest
    int i = 0;
    while(dest[i] != '\0'){
        i++;
    }

    for(int o = 0; src[o] != '\0'; o++){
        dest[i] = src[o];
        i++;
    }
    return dest;
}

//Pila
struct my_stack *my_stack_init (int size){ //Inicializa la pila y devuelve un puntero a ella
    struct my_stack *stack;
    stack = malloc(sizeof(stack));
    stack->size = size;
    stack->first = NULL;
    return stack;
}

int my_stack_push (struct my_stack *stack, void *data){ //Realiza un push de data a la pila stack
    struct my_stack_node *nodo;
    struct my_stack_node *nodo_aux;
    if((stack != NULL) && (stack->size > 0)){
        nodo = malloc(stack->size);
        nodo->data = data;
        if((stack->first == NULL)){ //Caso primer nodo
            nodo->next = NULL;
            stack->first = nodo;
        }else{      //Casos nodo x
            nodo_aux = stack->first;
                while(nodo_aux->next != NULL){
                    nodo_aux = nodo_aux->next;
                }
            nodo_aux->next = nodo;
        }
        return 0;
    }else{
        return -1;
    }
}

void *my_stack_pop(struct my_stack *stack){ //Devuelve los datos en la parte superior de la pila y elimina ese nodo
    struct my_stack_node *nodo;
    struct my_stack_node *nodo_ant;
    void *ret;
    if(stack->first == NULL || stack == NULL){ //Pila vacia
        return NULL;
    }else{
        nodo = stack->first;
        nodo_ant = stack->first;
        while(nodo->next != NULL){ //Leer la pila hasta el último valor
            nodo_ant = nodo;
            nodo = nodo->next;
        }
        if(nodo == stack->first){ //Si el ultimo nodo == al primer nodo
            stack->first = NULL;
            ret = nodo->data;
            free(nodo);
        }else{ //Ultimo nodo != primer nodo
            ret = nodo->data;
            nodo_ant->next = NULL;
            free(nodo);
        }
        return ret; //Devuelve puntero a los datos
    }
}

int my_stack_len(struct my_stack *stack){ //Lee la pila y devuelve la longitud de esta
    int contador = 0;
    struct my_stack_node *nodo;
    nodo = stack->first;
    if(nodo == NULL){
        return contador;
    }else{
        contador++;
        while(nodo->next != NULL){
            contador++;
            nodo = nodo->next;
        }
        return contador;
    }
}

int my_stack_purge (struct my_stack* stack){ //Elimina todos los nodos de la pila y finalmente la pila
    int counter = 0;
    int size = stack->size;
    struct my_stack_node *nodo;
    struct my_stack_node *nodo_ant;
    nodo = stack->first;
    if(nodo == NULL){ //Pila sin nodos
        counter = counter + sizeof(struct my_stack);
        free(stack);
        return counter;
    }else{
        while(nodo->next != NULL){ //Busca el ultimo nodo para borrarlo
            while(nodo->next != NULL){
                nodo_ant = nodo;
                nodo = nodo->next;
            }
            counter = counter + sizeof(struct my_stack_node) + size;
            nodo_ant->next = NULL;
            free(nodo);
            nodo = stack->first;
            nodo_ant = stack->first;
        }
        counter = counter + sizeof(struct my_stack_node) + size;
        counter = counter + sizeof(struct my_stack);
        free(stack->first); //Finalmente libera el primer nodo
        free(stack); //Y la pila
        return counter; //Devuelve cantidad de bytes eliminados
    }
}

struct my_stack *my_stack_copy (struct my_stack *stack){ //Realiza la copia de stack y la devuelve, pero la copia esta invertida
    struct my_stack *stack_copy;
    struct my_stack_node *nodo;
    struct my_stack_node *nodo_aux;
    stack_copy = my_stack_init(stack->size);
    nodo = stack->first;
    nodo_aux = stack->first;
    while(nodo_aux->next != NULL){ //Coloca nodo_aux en el ultimo nodo
        nodo_aux = nodo_aux->next;
    }
    for(int i = my_stack_len(stack) - 1; i != 0; i--){ //For para el recorrido de la pila
        while(nodo->next != nodo_aux){ //Colocar nodo en el ultimo nodo
            nodo = nodo->next;
        }
        my_stack_push(stack_copy,nodo_aux->data); //Push de los datos a la pila copiada
        nodo_aux = nodo; //Nodo auxiliar baja un nodo
        nodo = stack->first; //reiniciamos nodo al primer nodo
    }
    my_stack_push(stack_copy,stack->first->data); //Finalmente push del primer nodo
    return stack_copy;
}

int my_stack_write (struct my_stack *stack , char *filename){ //Escribre en filename el contenido de la pila (primero size y despues tantos datos como nodos)
    int filedecr;
    filedecr = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int cont = 0;
    void *buff;
    struct my_stack *copia;
    if(filedecr < 0){ //Error al abrir
        return EXIT_FAILURE;
    }else{
        write(filedecr,&stack->size,sizeof(int)); //Escribir size de la pila
        copia = my_stack_copy(stack); //Copia de la pila para poder destruirla mientras se escribre
        while(cont < my_stack_len(stack)){
            cont++;
            buff = my_stack_pop(copia);
            if(buff != NULL){
                write(filedecr,buff,stack->size); //Escribir todos los nodos
            }
        }
        my_stack_purge(copia); //Nos cargamos la pila
        close(filedecr); //Cerramos file descriptor
        return cont; //Devuelve nodos escritos
    }
}

struct my_stack* my_stack_read (char *filename){ //Lee de filename una pila
    struct my_stack *stack_read;
    int aux;
    ssize_t size;
    int fd = open(filename, O_RDONLY);
    if(fd == -1){ //Error al abrir
        return NULL;
    }
    void *buff;
    read(fd,&size,sizeof(int)); //Leemos size de la pila escrita para inicializar una pila
    stack_read = my_stack_init(size);
    buff = malloc(size);
    aux = read(fd,buff,size); //Primera lectura (suponemos que almenos se guarda 1 nodo en la pila)
    my_stack_push(stack_read,buff);
    while(aux != 0){ //Mientras la lectura no devuelva 0 bytes leidos, leemos nodos
        buff = malloc(size);
        aux = read(fd,buff,size);
        if(aux != 0){
            my_stack_push(stack_read,buff); //Push de esos nodos
        }
    }
    close(fd); //Cerrar filedecr
    free(buff); //Liberamos el ultimo malloc, ya que sera el caso de lectura 0 bytes
    return stack_read; //Return pila leida
}