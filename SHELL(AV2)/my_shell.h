//Programa realizado por Emanuel Hegedus y Bartomeu Capó Salas
#include <stdio.h>
#ifdef USE_READLINE
#include "/usr/include/readline/readline.h"
#include "/usr/include/readline/history.h"
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