#pragma once
#include "../process/process.h"  // Asegúrate de que el tipo Process esté definido aquí

typedef struct Node {
    Process *process; // Proceso asociado al nodo
    struct Node *next; // Siguiente nodo en la cola
} Node;

typedef struct Queue {
    Node* front;    // Puntero al primer nodo en la cola
    Node* rear;     // Puntero al último nodo en la cola
    int size;       // Número de procesos en la cola
} Queue;

// Prototipos de las funciones
Queue* create_queue();  // Asegúrate de tener esta declaración
void initializeQueue(Queue* q);
void enqueue(Queue *queue, Process *proceso);
Process* dequeue(Queue* q);
int isEmpty(Queue* q);
void freeQueue(Queue* q);