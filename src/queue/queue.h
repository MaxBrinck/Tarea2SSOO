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
    int quantum_ejecucion; // Quantum de ejecución
    int identificador; // Identificador de la cola 0 para low, 1 para high
} Queue;

// Prototipos de las funciones
Queue* create_queue();  // Crear nueva cola
void initializeQueue(Queue* q);  // Inicializar la cola
void enqueue(Queue *queue, Process *proceso);  // Agregar proceso a la cola
void dequeue(Queue *queue, Process *proceso );  // Sacar proceso de la cola
int isEmpty(Queue* q);  // Verificar si la cola está vacía
void freeQueue(Queue* q);  // Liberar la memoria de la cola
