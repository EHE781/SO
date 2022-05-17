#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//Pila
struct my_stack_node {
    void *data;
    struct my_stack_node *next;
};
struct my_stack{
    int size;
    struct my_stack_node *first;
};

//my_stdio
size_t my_strlen(const char *str);
int my_strcmp(const char *str1, const char *str2);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strcat(char *dest, const char *src);

//funciones pila
struct my_stack *my_stack_init (int size);
int my_stack_push (struct my_stack *stack, void *data);
void *my_stack_pop (struct my_stack *stack);
int my_stack_len (struct my_stack *stack);
struct my_stack *my_stack_copy (struct my_stack *stack);
int my_stack_write (struct my_stack *stack, char *filename);
struct my_stack *my_stack_read (char *filename);
int my_stack_purge (struct my_stack *stack);