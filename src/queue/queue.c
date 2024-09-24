#include "queue.h"
#include <stdlib.h>

// Función para crear una nueva cola
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q != NULL) {
        initializeQueue(q); // Inicializar la cola
    }
    return q;
}

// Función para inicializar una cola
void initializeQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// Función para agregar un proceso a la cola
void enqueue(Queue *queue, Process *proceso) {
    // Crear un nuevo nodo para el proceso
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        // Manejo de error si no se puede asignar memoria
        return;
    }
    
    update_process_state(proceso, READY); // Actualizar el estado a READY
    new_node->process = proceso;
    new_node->next = NULL;

    // Si la cola está vacía, el nuevo nodo es el primero
    if (queue->front == NULL) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        // Agregar el nuevo nodo al final de la cola
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    queue->size++; // Aumentar el tamaño de la cola
}

// Función para eliminar un proceso de la cola
Process* dequeue(Queue* q) {
    if (q->front == NULL) {
        return NULL; // La cola está vacía
    }

    Node* temp = q->front; // Nodo a eliminar
    Process* process = temp->process; // Obtener el proceso del nodo

    q->front = q->front->next; // Mover el frente al siguiente nodo
    if (q->front == NULL) {
        q->rear = NULL; // Si la cola queda vacía, actualizar el final
    }

    q->size--; // Disminuir el tamaño de la cola
    update_process_state(process, RUNNING); // Actualizar estado a RUNNING
    free(temp); // Liberar el nodo eliminado
    return process; // Devolver el proceso
}

// Función para verificar si la cola está vacía
int isEmpty(Queue* q) {
    return q->front == NULL;
}

// Función para liberar la memoria de la cola
void freeQueue(Queue* q) {
    while (!isEmpty(q)) {
        Process* process = dequeue(q); // Obtener el proceso
        free(process->Nombre);         // Liberar el nombre del proceso
        free(process);                 // Liberar el proceso en sí
    }
    free(q); // Liberar la estructura de la cola
}
