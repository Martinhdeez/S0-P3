#ifndef LIST_H__
#define LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> // Para manejar el tiempo
#include <sys/shm.h>
#include <sys/mman.h>//para munmap
#include <unistd.h>//libreria standard unix

#define LNULL NULL


//-------TIPOS DE DATOS----------
typedef struct node {
    void *data;
    struct node *next;
}Node;

typedef enum{
    FD,
    CHAR,
    MEM,
    MAP, 
    SHARED,
    ADDR
}key;

struct list{
    Node *header;
    int size; //numero de nodos de la lista
};

typedef enum openType{
    OPEN, DUPLICATED, MAPPED
}OpenType;

typedef struct openFile{
    char *name;
    int fd;
    int mode;
    OpenType type;
    int offset;
    int dupfd;
}OpenFile;

typedef struct list List;


typedef enum {
    MALLOC_MEMORY, SHARED_MEMORY, MAPPED_FILE
}allocationType;

typedef struct sharedMem{
    int key;
}SharedMem;

typedef struct mappedFile{
    char *name;
    int fd;
}MappedFile;

typedef struct memoryBlock{
    void *address;
    size_t size;
    time_t allocationTime;
    allocationType type;
    union{
        SharedMem shared;
        MappedFile mappedFile;
    }additionalInfo;
}MemoryBlock;

extern List *commandHistory;
extern List *openList;
extern List *memoryList;

//-----------FUNCIONES LIST-------------
List* createEmptyList(); 
Node* first(List *list);
Node* next(struct node *current);
int getSize(List *list);
void* getData(struct node *node);

int insertElement(void *data, size_t size,List *list); 
bool removeElement(void *data, key Key, List *list);
void freeList(key Key, List *list);

size_t getBlockSize(void *address);


#endif