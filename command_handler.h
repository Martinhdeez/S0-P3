#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "list.h"
#include "shell_utilities.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> 
#include <unistd.h> // Para obtener pid y ppid(libreir estandar de linux/unix)
#include <time.h>   // Para obtener fecha actual
#include <fcntl.h>  // Para cmd open, fcntl, O_CREAT, ...
#include <errno.h>  // Para perror
#include <sys/utsname.h> //libreria de linux para sacar la informacion del sistema
#include <sys/stat.h> //para mkdir y muchas otras flags
#include <dirent.h>//para listfile
#include <pwd.h>  // Para obtener el nombre de usuario
#include <grp.h>  // Para obtener el nombre del grupo
#include <sys/types.h>
#include <time.h> // Para manejar el tiempo
#include <sys/mman.h>
#include <sys/shm.h>
#include <errno.h>

#define GREENN "\033[0;32m"
#define BLUEE "\033[0;34m"
#define WHITE "\033[0m"
#define ORANGE "\033[38;5;214m"
#define LIGHTBLUE "\033[38;5;81m"

// Función que toma tokens (comando y argumentos)
typedef int (*CmdFunc)(char *tokens[]);

typedef struct{
    char *name;
    CmdFunc func;
}Command;

// Funciones que no requieren una lista
int Cmd_authors(char *tokens[]);   
int Cmd_pid();                    
int Cmd_ppid();                    
int Cmd_cd(char *tokens[]);    
int Cmd_date(char *tokens[]);    
int Cmd_dup(char *tokens[]);
int Cmd_historic(char *tokens[]); 
int Cmd_open(char *tokens[]);    
int Cmd_close(char *tokens[]);   
int Cmd_infosys();       
int Cmd_cwd();
int Cmd_recurse(char *tokens[]);         
int Cmd_makefile(char *tokens[]);  
int Cmd_makedir(char *tokens[]);  
int Cmd_listfile(char *tokens[]); 
int Cmd_listdir(char *tokens[]); 
int Cmd_allocate(char *tokens[]);   
int Cmd_deallocate(char *tokens[]); 
int Cmd_help(char *tokens[]);  
int Cmd_memory(char *tokens[]);     
int Cmd_memfill(char *tokens[]);     
int Cmd_read(char *tokens[]);
int Cmd_readfile(char *tokoens[]);
int Cmd_write(char *tokens[]);
int Cmd_write(char *tokens[]);      
int Cmd_getuid();
int Cmd_setuid(char *tokens[]);
int Cmd_exit();      

void printAllMemList(Node *nodo, int order, int n);

//AUX FUN
char* GetCwd();
int isInteger(const char *str);

int Cmd_ls(char *tokens[]);
char* get_formatted_cwd();

extern Command commands[];

#endif