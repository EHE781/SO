//Programa realizado por Emanuel Hegedus y Bartomeu Capó Salas
#include <stdio.h>
#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

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

char *read_line(char *line);
int execute_line(char*line);
int parse_args(char **args,char *line);
int check_internal(char **args);
int internal_cd(char**args);
int internal_export(char **args);
int internal_source(char**args);
int internal_jobs(char**args);
int internal_fg(char**args);
int internal_bg(char**args);
int jobs_list_add(pid_t pid, char status, char* command_line);
int jobs_list_find(pid_t pid);
int jobs_list_remove(int pos);
bool is_background(char* args);
void reaper(int signum);
void ctrlc(int signum);
void ctrlz(int signum);
void chomp(char *s);

struct info_process{
    pid_t pid;
    char status; // 'E' (ejecutando),'D' (detenido),'F' (finalizado)
    char command_line[COMMAND_LINE_SIZE]; //Comando
};