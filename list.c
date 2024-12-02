#include "list.h"

List *commandHistory = NULL;
List *openList = NULL;
List *memoryList = NULL;

List* createEmptyList(){
    
    List *tmp = malloc(sizeof(List));
    if(tmp == NULL){
        perror("Error creating list: ");
        exit(EXIT_FAILURE);
    } 

    tmp->size = 0;
    tmp->header = malloc(sizeof(struct node));
    if(tmp->header == NULL){
        free(tmp);
        perror("Error creating node: ");
        exit(EXIT_FAILURE);
    } 

    tmp->header->data = NULL;
    tmp->header->next = NULL;

    return tmp;
}

Node* first(List *list){
    return list->header->next;
}

Node* next(struct node *current){
    return current->next;
}

int getSize(List *list){
    return list->size;
}


bool createNode(Node **node){
    *node = malloc(sizeof(Node));
    return *node != NULL;
}

void* getData(Node *node){
   return node->data; 
}

int insertElement(void *data, size_t size, List* list){
    
    struct node *newNode;
    if(!createNode(&newNode)){
        free(newNode);
        perror("Error creating node: ");
        return 1;
    }
    newNode->data = data;
    newNode->next = list->header->next;
    list->header->next = newNode;
    list->size++;
    return 0;

}

void freeMemBlocks(Node *current) {
    MemoryBlock* block = (MemoryBlock*)current->data;

    if (!block) return; // Si el bloque es NULL, no hay nada que liberar

    // Liberar recursos específicos según el tipo de asignación
    if (block->type == MALLOC_MEMORY) {
        // Liberar memoria asignada dinámicamente
        if (block->address != NULL) {
            free(block->address);
            block->address = NULL; // Asegura que no se libere de nuevo
        }
    } else if (block->type == SHARED_MEMORY) {
        // Liberar memoria compartida (detach y remove)
        if (block->address) {
            shmdt(block->address); // Detach del segmento de memoria compartida
            shmctl(block->additionalInfo.shared.key, IPC_RMID, NULL); // Eliminar segmento
            block->address = NULL;
        }
    } else if (block->type == MAPPED_FILE) {
        // Cerrar el archivo mapeado y liberar el nombre dinámico
        if (block->address) {
            munmap(block->address, block->size); // Desmapear memoria
            block->address = NULL;
        }
        if (block->additionalInfo.mappedFile.fd >= 0) {
            close(block->additionalInfo.mappedFile.fd);
            removeElement(&(block->additionalInfo.mappedFile.fd), FD, openList); // Eliminarlo de la lista de archivos abiertos
            if (block->additionalInfo.mappedFile.name != NULL) {
                free(block->additionalInfo.mappedFile.name);
                block->additionalInfo.mappedFile.name = NULL; // Evitar doble liberación
            }
        }
    } else {
        printf("Invalid type of memory\n");
    }

    free(block); // Liberar la estructura del bloque
    current->data = NULL; // Asegurar que el puntero no apunte a memoria liberada
}

size_t getBlockSize(void *address) {
    struct node *current = memoryList->header->next;

    while (current != NULL && current->data != NULL) {
        MemoryBlock *block = (MemoryBlock*)current->data;
        if (block->address == address) {
            return block->size;
        }
        current = current->next;  // Avanza 'current' al siguiente nodo
    }
    return -1;
}

bool removeElement(void *data, key Key, List *list) {
    if (!list || !list->header || list->size == 0 || !data) {
        return false;
    }

    struct node *prev = list->header;
    struct node *current = list->header->next;

    while (current != NULL) {
        bool match = false, normalMatch = false;
        if (current->data == NULL) {
            current = current->next;
            continue;
        }
        switch (Key) {
            case CHAR:
                normalMatch = (current->data == data);
                break;
            case FD: {
                int *fd = (int *)data;
                normalMatch = (((OpenFile*)current->data)->fd == *fd);
                if(((OpenFile*)current->data)->fd == *fd){
                    close(((OpenFile*)current->data)->fd);
                    free(((OpenFile*)current->data)->name);
                    free((OpenFile*)current->data);
                }
                break;
            }
            case MEM: {
                size_t *size = (size_t *)data;
                MemoryBlock *block = (MemoryBlock *)current->data;
                // Verificar si el puntero `current->data` no es NULL
                if (block == NULL) {
                    current = current->next;
                    continue; // Si es NULL, pasa al siguiente nodo
                }
                match = (block->type == MALLOC_MEMORY && block->size == *size);
                break;
            }
            case MAP: {
                char *name = (char *)data;
                MemoryBlock *block = (MemoryBlock *)current->data;
                match = (block->type == MAPPED_FILE && strcmp(block->additionalInfo.mappedFile.name, name) == 0);
                break;
            }
            case SHARED: {
                int *key = (int *)data;
                MemoryBlock *block = (MemoryBlock *)current->data;
                match = (block->type == SHARED_MEMORY && block->additionalInfo.shared.key == *key);
                break;
            }
            case ADDR:
                match = (((MemoryBlock *)current->data)->address == data);
                break;
            default:
                return false;
        }

        if (match || normalMatch) {
            prev->next = current->next;
            if(match) freeMemBlocks(current);
            free(current); // Liberar el nodo después de haber liberado su contenido
            list->size--;
            return true;
        }

        prev = current;
        current = current->next;
    }

    return false;
}

void freeOpenFiles(Node* current) {
    OpenFile* file = (OpenFile*)current->data;
    
    if (file) {
        // Asegurarse de que no estamos tratando con los archivos estándar
        if (file->name && (strcmp(file->name, "stdin") != 0) && 
            (strcmp(file->name, "stdout") != 0) && 
            (strcmp(file->name, "stderr") != 0)) {
            free(file->name); // Liberar el nombre del archivo
            file->name = NULL; // Asegurarse de que no se vuelva a liberar
        }
        
        // Si se ha abierto un archivo, se puede cerrar aquí
        if (file->fd >= 0) {
            close(file->fd); // Cerrar el archivo abierto
        }
    }
    
    free(file); // Liberar la estructura OpenFile
    current->data = NULL; // Evitar acceso a memoria liberada
}

void freeList(key Key, List *list) {
    Node* current = list->header->next;

    while (current != NULL) {
        Node* tmp = current->next;

        if (Key == FD) {
            // Asegúrate de liberar correctamente los archivos abiertos
            freeOpenFiles(current);
        } else if (Key == CHAR) {
            if (current->data) {
                free(current->data);
                current->data = NULL; // Evitar acceso a memoria liberada
            }
        } else if (Key == MEM) {
            freeMemBlocks(current);
        }

        free(current); // Liberar el nodo
        current = tmp;
    }
    
    free(list->header);
    list->header = NULL; // Evitar acceso a memoria liberada
    free(list); 
}
