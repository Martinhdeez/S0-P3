#include "shell_utilities.h"
#include "list.h"
#include "command_handler.h"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"
#define BLUE "\033[1;34m"


void printPrompt(){
    char* cwd = get_formatted_cwd();
    // Obtener el ID del usuario actual
    uid_t uid = getuid();
    // Obtener la información del usuario
    struct passwd *pw = getpwuid(uid);

    // Obtener el nombre del sistema
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("Error obtaining hostname");
        strcpy(hostname, "unknown"); // Valor por defecto en caso de error
    }

    printf("%s", RED);
    // Mostrar el prompt en el formato deseado
    if (pw) {
        printf("%s@%s:", pw->pw_name, hostname); // Formato: nombre@grupo>
    } else {
         printf("martin@dani:");
    }

    printf("%s", BLUE);
    printf("%s", cwd);
    printf("%s", RESET);
    printf("$ ");
    free(cwd);
}

char* readPrompt(){

    char *prompt= malloc(MAX_PROMPT * sizeof(char));
    if (prompt == NULL){
        perror("Allocation error: ");
        exit(EXIT_FAILURE);
    }

    if (fgets(prompt, MAX_PROMPT, stdin) == NULL){
        
        free(prompt);
        if(feof(stdin)) perror("End of entry: ");
        else perror("Error writting: ");
        
        exit(EXIT_FAILURE);
    }

    // Eliminar \n al final de la línea
    size_t len = strlen(prompt);
    if (len > 0 && prompt[len - 1] == '\n')
        prompt[len - 1] = '\0';

    return prompt;
}


int splitLine(char *prompt, char *tokens[]){
    
    int i = 1;
    if ((tokens[0] = strtok(prompt, " \n\t")) == NULL)
        return 0; // no se ha escrito nada
    while ((tokens[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

int processPrompt(char *prompt, char *tokens[]){
    if(prompt[0] != ' ' && prompt[0] != '\0') insertElement(prompt, strlen(prompt)+1, commandHistory);

    int ctr = 0;
    char *aux = strdup(prompt);
    int nTok = splitLine(aux, tokens);
    if(nTok <= 0)return 0;

    for(int i = 0;commands[i].name != NULL; i++){
        if(strcmp(tokens[0], commands[i].name)==0){

            ctr = commands[i].func(tokens); //llama a la funcion y evalua si se ha ejecutado correctamente
            free(aux);
            if(ctr == 2) return 1;//señal de salida 
            if(ctr){
                //manejo de errores
            }
            
            return 0;
        }
    }
    printf("Command not found: %s , help for more information about the avaliable commands\n", tokens[0]);
    free(aux);
    return 0;
}

