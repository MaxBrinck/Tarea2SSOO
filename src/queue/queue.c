#include "queue.h"
#include "../process/process.h"
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
void dequeue(Queue *queue, Process *proceso) {
    
    // Verifica si la cola está vacía
    if (queue->front == NULL) {
        printf("Error: La cola está vacía.\n");
        return;
    }

    Node* temp = queue->front;
    Node* prev = NULL;

    // Recorre la cola para encontrar el proceso
    while (temp != NULL) {
        if (temp->process == proceso) { // Si el proceso es encontrado
            // Si es el primer nodo (frente de la cola)
            if (prev == NULL) {
                queue->front = temp->next;
                // Si también es el último nodo
                if (queue->front == NULL) {
                    queue->rear = NULL;
                }
            } else {
                // Saltar el nodo que contiene el proceso
                prev->next = temp->next;
                // Si era el último nodo, actualizar rear
                if (temp == queue->rear) {
                    queue->rear = prev;
                }
            }

            // Disminuir el tamaño de la cola
            queue->size--;

            // Liberar la memoria del nodo y terminar
            free(temp);
            printf("Proceso con PID %d ha sido liberado.\n", proceso->pid);
            return;
        }

        // Continuar al siguiente nodo
        prev = temp;
        temp = temp->next;
    }

    // Si se llega aquí, no se encontró el proceso
    printf("Error: El proceso no se encontró en la cola.\n");
}


// Función para verificar si la cola está vacía
int isEmpty(Queue* q) {
    return q->front == NULL;
}

// Función para liberar la memoria de la cola
void freeQueue(Queue* q) {
    Node* current = q->front;
    Node* next_node;

    // Recorre la cola y libera cada nodo
    while (current != NULL) {
        next_node = current->next;      // Guardar el siguiente nodo
        free(current->process->Nombre); // Liberar el nombre del proceso
        free(current->process);         // Liberar el proceso en sí
        free(current);                  // Liberar el nodo actual
        current = next_node;            // Avanzar al siguiente nodo
    }

    // Liberar la estructura de la cola
    free(q);
}
