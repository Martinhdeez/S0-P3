/*Martín Hernández González", "m.hernandezg@udc.es*/
/*Daniel Quinteiro Graña", "daniel.quinteirog@udc.es*/


#include "shell_utilities.h"
#include "list.h"
#include "command_handler.h"



int main(){

    commandHistory = createEmptyList();
    openList = createEmptyList();
    Cmd_open(NULL);//inicializar open
    memoryList = createEmptyList();
    int end = 0;
    char **tokens = malloc(MAX_PROMPT/2*sizeof(char));
    if(tokens == NULL){
        perror("Allocation error: ");
        return 1;
    }

    while(!end){
        printPrompt();
        char *prompt = readPrompt();
        end = processPrompt(prompt, tokens);    
    }

    free(tokens);
    freeList(FD, openList);
    freeList(CHAR, commandHistory);
    freeList(MEM, memoryList);

    return 0;
}
