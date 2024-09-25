#include "queue.h"
#include "../process/process.h"
#include <stdlib.h>
#include <stdio.h>

// Función para crear una nueva cola
Queue* create_queue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL) {
        fprintf(stderr, "Error al asignar memoria para la cola.\n");
        exit(EXIT_FAILURE);
    }
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    queue->quantum_ejecucion = 0; // o cualquier valor inicial que desees
    queue->identificador = -1; // o cualquier valor inicial que desees

    return queue;
}


// Función para agregar un proceso a la cola
// Función para agregar un proceso a la cola
void enqueue(Queue *queue, Process *proceso) {
    if (queue == NULL) {
        fprintf(stderr, "Error: queue is NULL\n");
        return;  // Manejo de error si la cola es NULL
    }
    
    if (proceso == NULL) {
        fprintf(stderr, "Error: process is NULL\n");
        return;  // Manejo de error si el proceso es NULL
    }

    // Asignar memoria para un nuevo nodo
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Error allocating memory for new node\n");
        return;
    }

    // Inicializar el nuevo nodo con el proceso y apuntar a NULL (no hay siguiente nodo)
    new_node->process = proceso;
    new_node->next = NULL;

    // Si la cola está vacía (front es NULL), el nuevo nodo será tanto front como rear
    if (queue->front == NULL) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        // Si la cola ya tiene elementos, agregar el nuevo nodo al final
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    // Incrementar el tamaño de la cola
    queue->size++;
}

// Función para eliminar un proceso de la cola
void dequeue(Queue *queue, Process *proceso) {
    if (isEmpty(queue)) {
        fprintf(stderr, "Error Dequeue: queue is empty\n");
        return;
    }

    Node *temp = queue->front;
    Node *prev = NULL;

    // Buscar el nodo con el proceso que queremos eliminar
    while (temp != NULL && temp->process != proceso) {
        prev = temp;
        temp = temp->next;
    }

    // Si no se encontró el proceso
    if (temp == NULL) {
        fprintf(stderr, "Error: process not found in the queue\n");
        return;
    }

    // Si el proceso a eliminar está en el frente
    if (temp == queue->front) {
        queue->front = temp->next;
    } else {
        // Saltar el nodo que contiene el proceso
        prev->next = temp->next;
    }

    // Si el nodo a eliminar es el último de la cola (rear)
    if (temp == queue->rear) {
        queue->rear = prev;
    }

    free(temp); // Liberar la memoria del nodo
    queue->size--;
}




// Función para verificar si la cola está vacía
int isEmpty(Queue* q) {
    return q->front == NULL;
}

// Función para liberar la memoria de la cola


void freeQueue(Queue *queue) {
    Node *current = queue->front;
    while (current != NULL) {
        Node *next = current->next;
        
        // Liberar el nombre del proceso
        if (current->process->Nombre != NULL) {
            //free(current->process->Nombre);
        }

        // Liberar el proceso en sí
        free(current->process);

        // Liberar el nodo de la cola
        free(current);
        
        current = next;
    }
    free(queue);
}
