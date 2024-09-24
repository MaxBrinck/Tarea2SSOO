#include "process.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Función para crear un nuevo proceso
Process* create_process(char **data) {
    Process *p = (Process *)malloc(sizeof(Process));
    if (p == NULL) {
        perror("Error al asignar memoria para el proceso");
        return NULL;
    }

    // Asegúrate de que Nombre tenga suficiente espacio
    p->Nombre = (char *)malloc(strlen(data[0]) + 1);
    if (p->Nombre == NULL) {
        perror("Error al asignar memoria para el nombre del proceso");
        free(p);
        return NULL;
    }

    strcpy(p->Nombre, data[0]);
    p->pid = atoi(data[1]);
    p->T_INICIO = atoi(data[2]);
    p->Tiempo_ejecucion = atoi(data[3]);
    p->Numero_rafagas = atoi(data[4]);
    p->Tiempo_espera = atoi(data[5]);
    p->Deadline = atoi(data[6]);
    p->Estado = READY;  // Inicializar el estado

    return p;
}

// Función para actualizar el estado de un proceso
void update_process_state(Process *p, EstadoProceso new_state) {
    p->Estado = new_state;
}
