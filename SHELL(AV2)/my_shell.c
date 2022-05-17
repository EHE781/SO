#define _POSIX_C_SOURCE 200112L
//Programa realizado por Emanuel Hegedus y Bartomeu Capó Salas
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 64
#define PROMPT '$'
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"
#define USE_READLINE
#include "my_shell.h"

static struct info_process jobs_list[N_JOBS];
static int n_pids = 1;
static char* argv0;
static char prompt[2048];
/*La variable prompt se ha puesto como variable estática 
  para poder usarla en diferentes métodos, para evitar errores
  en los que el prompt no se mostraba correctamente*/

//Función encargada de quitar \n y poner \0
void chomp(char *s)
{
    s[strcspn(s, "\n")] = '\0';
}

//Handler de la señal SIGINT, mandará SIGTEM si hay un proceso en foreground
void ctrlc(int signum){
    signal(SIGINT,ctrlc);
    if(jobs_list[0].pid > 0){
        printf("\n");
        if(strcmp(jobs_list[0].command_line,argv0) != 0){
            kill(jobs_list[0].pid,SIGTERM);
        }else{
            fprintf(stderr,"Señal SIGTERM no enviada debido a que el proceso en foreground es el shell");
        }
    }
    else
    {
        #ifdef USE_READLINE
            printf("\n%s",prompt);
        #else    
            printf("\n");
        #endif
        fprintf(stderr,"Señal SIGTERM no enviada debido a que no hay proceso en foreground");
    }
}

//Handler de la señal SIGTSTP, pausará el proceso en foreground
void ctrlz(int signum){
    signal(SIGTSTP,ctrlz);
    if(jobs_list[0].pid > 0){
        if(strcmp(jobs_list[0].command_line,argv0) != 0){
            kill(jobs_list[0].pid,SIGSTOP);
            printf("Señal SIGSTOP enviada a %i (%s) \n",jobs_list[0].pid,jobs_list[0].command_line);
            jobs_list[0].status = 'D';
            jobs_list_add(jobs_list[0].pid,jobs_list[0].status,jobs_list[0].command_line);
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'E';
            memset(jobs_list[0].command_line,0,COMMAND_LINE_SIZE);
            strcpy(jobs_list[0].command_line,argv0);
        }else{
            fprintf(stderr,"Señal SIGTSTP no enviada debiado a que el proceso en foreground es el shell");
        }
    }else{
        #ifdef USE_READLINE
            printf("\n%s",prompt);
        #else    
            printf("\n");
        #endif
        fprintf(stderr,"Señal SIGTSTP no enviada debido a que no hay poceso en foreground");
    }
}

//Handler de señal SIGCHLD, mostrará por pantalla el proceso que ha terminado y como
void reaper(int signum){
    signal(SIGCHLD,reaper);
    pid_t ended;
    int status;
    int backg_pos;
    while((ended = waitpid(-1,&status,WNOHANG)) > 0){
        backg_pos = jobs_list_find(ended);
        if(backg_pos != -1){
            if(WIFSIGNALED(status)){
                printf("reaper() -> Enterrado proceso con PID: %d. Debido a la señal %i \n", ended, WTERMSIG(status));
            }else if(WIFEXITED(status)){
                printf("reaper() -> Enterrado proceso con PID: %d. Ha terminado de forma normal\n", ended);
            }else{
                printf("reaper() -> Enterrado proceso con PID: %d. Ha terminado por un error\n", ended);
            }
            jobs_list_remove(backg_pos);
        }else{
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
            memset(jobs_list[0].command_line, 0, sizeof(jobs_list[0].command_line));
            strcpy(jobs_list[0].command_line, argv0);
            if(WIFSIGNALED(status)){
                printf("reaper() -> Enterrado proceso con PID: %d. Debido a la señal %i \n", ended, WTERMSIG(status));
            }else if(WIFEXITED(status)){
                printf("reaper() -> Enterrado proceso con PID: %d. Ha terminado de forma normal\n", ended);
            }else{
                printf("reaper() -> Enterrado proceso con PID: %d. Ha terminado por un error\n", ended);
            }
        }
    }
}

//Programa encargado de leer la entrada de teclado
char *read_line(char *line)
{
    char buffer[COMMAND_LINE_SIZE];
    char *tiempo;
    //Solo se usa ptr con readline, evitar warnings
    #ifdef USE_READLINE
    char *ptr;
    #endif
    time_t rawtime;
    struct tm *info;
    getcwd(buffer,ARGS_SIZE);
    time(&rawtime);
    info = localtime(&rawtime);
    tiempo = asctime(info);
    chomp(tiempo);
    //sincronización entre prints, para evitar que se dibujen uno tras otro
    sleep(0.93);
    //Guardamos el prompt con sprintf que permite guardar, con el formato de los prints, un array
    sprintf(prompt, GREEN "%s" COLOR_RESET ":~" YELLOW "[%s]" CYAN "%s" RED "%c" MAGENTA, getenv("USER"), tiempo, buffer, PROMPT);
    sleep(0.93); //sincronización entr prints, para evitar que se dibujen uno tras otro
    if(jobs_list[0].pid>0){

    }else{
        #ifdef USE_READLINE
        ptr = readline(prompt);
        if(ptr != NULL){
            strcpy(line,ptr);
        }else{
            line = NULL;
        }
        free(ptr);
        if(line && *line){
            add_history (line);
        }
        #else
        printf("%s",prompt);
        line = fgets(line,COMMAND_LINE_SIZE,stdin);
        #endif
    }
    fflush(stdout);
    //En el caso de Ctrl+D saltamos de linea y cerramos el programa
    if(line == NULL){
        printf("\n");
        exit(0);
    }
    return line;
}

//Método que devuelve true o false dependiendo de si se debe redirigir la salida
int is_output_redirection(char** args){
    int outp = 0;
    int args_path;
    int fd;
    for(int i = 0;(args[i] != NULL && outp == 0); i++){
        if(strcmp(args[i],">") == 0){
            if(strlen(args[i+1]) > 1){
                args_path = i+1;
                outp = 1;
                args[i] = NULL;
            }
        }
    }
    if(outp == 1){
        fd = open(args[args_path], O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR);
        dup2(fd,1);
        close(fd);
    }
    return outp;
}

//Busca el caracter & y devuelve true si lo encuentra, false en caos contrario
bool is_background(char* line){
    int length;
    length = strlen(line);
    #ifdef USE_READLINE
    if(line[length - 1] == '&'){
        line[length - 1] = (char) 0;
        return true;
    }
    #else
    if(line[length - 2] == '&'){
        line[length - 2] = '\0';
        return true;
    }
    #endif
    return false;
}

//Método encargado de ejecutar las lineas que escribe el usuario
int execute_line(char*line){
    char *args[ARGS_SIZE];
    char line_or[COMMAND_LINE_SIZE];
    bool backg;
    pid_t pid;
    if(line != NULL){
        memset(line_or, 0, COMMAND_LINE_SIZE);
        strcpy(line_or,line);
        strcpy(jobs_list[0].command_line,line);
        backg = is_background(line);
        parse_args(args,line);
        if(check_internal(args) == 0){
            pid = fork();
            if(pid > 0){
                //Si se ha creado un hijo en background añadimos el job, sino esperamos
                if(backg == true){
                    jobs_list_add(pid,'E',line_or);
                }else{
                    jobs_list[0].pid = pid;
                    while(jobs_list[0].pid != 0){
                        pause();
                    }
                }
            }else if(pid == 0){
                //asignamos señales a sus handlers
                signal(SIGCHLD,SIG_DFL);
                signal(SIGINT,SIG_IGN);
                if(backg == true){
                    signal(SIGTSTP,SIG_IGN);
                }else{
                    signal(SIGTSTP,SIG_DFL);
                }
                is_output_redirection(args);
                execvp(args[0],args);
                exit(0);
            }
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

//método que quita la posición pos de jobs_list
int jobs_list_remove(int pos){
    //n_pids es el top de la pila, por lo tanto es el último proceso
    //Copiamos el último proceso de la lista a la posicion que queremos borrar
    jobs_list[pos] = jobs_list[n_pids];
    jobs_list[n_pids].pid = 0;
    n_pids = n_pids - 1;
    return 0;
}

//método que agrega un job a jobs_list
int jobs_list_add(pid_t pid, char status, char* command_line){
    if(n_pids >= N_JOBS){
        perror("No se ha podido añadir el proceso, array full");
        return -1;
    }else{
        jobs_list[n_pids].pid = pid;
        jobs_list[n_pids].status = status;
        memset(jobs_list[n_pids].command_line, 0 , COMMAND_LINE_SIZE);
        strcpy(jobs_list[n_pids].command_line,command_line);
        n_pids = n_pids + 1;
        return 0;
    }
}

//Devuelve la posición en jobs_list con la que coincide el pid
int jobs_list_find(pid_t pid){
    for(int i = 0; i <= n_pids; i++){
        if(jobs_list[i].pid == pid){
            return i;
        }
    }
    return -1;
}

/*divide la entrada del usuario en tokens para facilitar
 posteriormente el uso de la entrada*/
int parse_args(char **args,char *line){
    int counter = 0;
    char comment[ARGS_SIZE];
    strcpy(comment, line);
    args[0] = strtok(line, " \n\0");
    if ((strncmp(args[0], "#", 1)))
    {
        for (int i = 1; args[i - 1] != NULL; i++)
        {
            args[i] = strtok(NULL, " \n\0");
            counter++;
        }
    }
    return counter;
}

/*Comprueba si la entrada del usuario es un comando interno
  y, en caso de serlo, entra en el mismo*/
int check_internal(char **args)
{
    if (strcmp(args[0], "cd") == 0)
    {
        internal_cd(args);
        return 1;
    }
    else if (strcmp(args[0], "export") == 0)
    {
        internal_export(args);
        return 1;
    }
    else if (strcmp(args[0], "source") == 0)
    {
        internal_source(args);
        return 1;
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        internal_jobs(args);
        return 1;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        internal_fg(args);
        return 1;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        internal_bg(args);
        return 1;
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        return 0;
    }
}

/*Comando cd del shell implementado internamente en
 nuestro mini_shell, el cd avanzado está implementado
 permite el movimiento lógico entre directorios*/
int internal_cd(char **args)
{
    char buff[ARGS_SIZE];
    const char search1 = (char)34; //34 y 39 son comillas y dobles comillas
    const char search2 = (char)39;
    const char search3 = (char)92;
    int hayComillas = 0;
    char edited_args[ARGS_SIZE];
    if (args[1] == NULL)
    {
        if (chdir(getenv("HOME")) == -1)
        {
            fprintf(stderr, "Error al cambiar directorio\n");
            return -1;
        }
        else
        {
            getcwd(buff, ARGS_SIZE);
            return 0;
        }
    }
    else
    {
        if (strchr(args[1], search1) == NULL && strchr(args[1], search2) == NULL)
        {
            if (args[1][strlen(args[1]) - 1] == search3)
            {
                args[1][strlen(args[1]) - 1] = '\0';
                strcpy(edited_args, args[1]);
                strcat(edited_args, " ");
                strcat(edited_args, args[2]);
            }
        }
        else
        {
            hayComillas = 1;
            strcpy(edited_args, "\0");
            for (int i = 1; args[i] != NULL; i++)
            {
                strcat(edited_args, args[i]);
                if (edited_args[i + 1] != (char) 0)
                {
                    strcat(edited_args, " ");
                }
            }
            memmove(&edited_args[0], &edited_args[1], strlen(edited_args) - 1);
            memmove(&edited_args[strlen(edited_args) - 3], &edited_args[strlen(edited_args)], 1);
            chdir("cd.."); //un arreglo muy malo pero es lo que hay
        }
        if (((chdir(args[1]) == -1) && (hayComillas == 0)) || ((chdir(edited_args) == -1 && (hayComillas == 1))))
        {
            fprintf(stderr, "Error, no se ha encontrado el directorio\n");
            return -1;
        }
        else
        {
            getcwd(buff, ARGS_SIZE);
            return 0;
        }
    }
}

//Comando export del shell implementado en nuestro mini_shell
//Export cambia el valor de una variable de entrono a otro valor
int internal_export(char **args)
{
    const char delim[2] = "=";
    char *token;
    char *token2;
    if (args[1] == NULL)
    {
        fprintf(stderr, "Sintaxis de export: export (Nombre de variable de entorno)=(Valor deseado)\n");
        return -1;
    }
    else
    {
        token = strtok(args[1], delim);
        token2 = strtok(NULL, "\n");
        if (setenv(token, token2, 1) == -1)
        {
            fprintf(stderr, "Error al intentar cambiar el valor\n");
            return -1;
        }
        return 0;
    }
}

//Comando source del shell implementado en nuestro mini_shell
//Lee un archivo e intenta ejecutar linea a linea
int internal_source(char **args)
{
    FILE *fp;
    char string[ARGS_SIZE];
    if (args[1] == NULL)
    {
        fprintf(stderr, "Sintaxis de source: source (Nombre de archivo) (Argumentos)\n");
        return -1;
    }
    fp = fopen(args[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: El archivo no existe,compruebe la dirección\n");
        return -1;
    }
    while (fgets(string, ARGS_SIZE, fp) != NULL)
    {
        fflush(fp);
        execute_line(string);
        
    }
    fclose(fp);
    return 0;
}

//Comando jobs del shell implementado en nuestro mini_shell
//Devuelve todos los trabajos activos o en espera
int internal_jobs(char**args){
    int total = 0;
    for(int i = 1; ((i < N_JOBS) && (jobs_list[i].pid != 0)); i++){
        printf("Job numero %i:\n          PID: %i\n          "
        "Command line: %s\n          Status: %c \n",i,jobs_list[i].pid,
        jobs_list[i].command_line,jobs_list[i].status);
        total = i;
    }
    return total;
}

//Comando fg que reanuda un trabajo pausado en foreground
int internal_fg(char **args)
{
    int pos;
    int len;
    if (args[1] == NULL)
    {
        fprintf(stderr, "Sintaxis de fg: fg (número correspondiente al trabajo)\n");
        return -1;
    }
    sscanf(args[1], "%d", &pos);
    if(pos >= n_pids || pos == 0){
        fprintf(stderr,"No existe este trabajo \n");
        return -1;
    }
    if(jobs_list[pos].status == 'D'){
        kill(jobs_list[pos].pid,SIGCONT);
    }
    len = strlen(jobs_list[pos].command_line);
    #ifdef USE_READLINE
    if(jobs_list[pos].command_line[len-1] == '&'){
        jobs_list[pos].command_line[len-1] = (char) 0;
    }
    #else
    if(jobs_list[pos].command_line[len-2] == '&'){
        jobs_list[pos].command_line[len-2] = (char) 0;
    }
    #endif
    jobs_list[0] = jobs_list[pos];
    jobs_list[0].status = 'E';
    jobs_list_remove(pos);
    printf("fg() -> command_line a ejecutar: %s \n",jobs_list[0].command_line);
    execute_line(jobs_list[0].command_line);
    return 0;
}

//Comando fg que reanuda un trabajo pausado en background
int internal_bg(char **args)
{
    int pos;
    int len;
    if (args[1] == NULL)
    {
        fprintf(stderr, "Sintaxis de bg: bg (número correspondiente al trabajo)\n");
        return -1;
    }
    sscanf(args[1], "%d", &pos);
    if(pos >= n_pids || pos == 0){
        fprintf(stderr,"No existe este trabajo\n");
        return -1;
    }
    if(jobs_list[pos].status == 'E'){
        fprintf(stderr,"Ya se está ejecutando en 2º plano\n");
        return -1;
    }
    jobs_list[pos].status = 'E';
    len = strlen(jobs_list[pos].command_line);
    jobs_list[pos].command_line[len] = ' ';
    jobs_list[pos].command_line[len + 1] = '&';
    kill(jobs_list[pos].pid,SIGCONT);
    printf("bg() -> proceso reanudado en background con numero: %i, PID: %i, status: %c y command_line: %s\n"
    ,pos,jobs_list[pos].pid,jobs_list[pos].status,jobs_list[pos].command_line);
    return 0;
}

//Inicializador del programa, asigna signal handlers e inicializa todo lo necesario
int main(int argc, char **args)
{
    char line[COMMAND_LINE_SIZE];
    argv0 = args[0];
    signal(SIGCHLD,reaper);
    signal(SIGINT,ctrlc);
    signal(SIGTSTP,ctrlz);
    jobs_list[0].pid = 0;
    jobs_list[0].status = 'E';
    strcpy(jobs_list[0].command_line, argv0);
    while(1){
        while(read_line(line) != NULL && strlen(line) > 1){
            execute_line(line);
        }
    }
}