#pragma once
#ifndef PROCESS_H
#define PROCESS_H

typedef enum {
    RUNNING,
    READY,
    WAITING,
    FINISHED
} EstadoProceso;

typedef struct {
    char *Nombre;        // Nombre del proceso
    int pid;           // ID del proceso
    int Estado;         // Estado del proceso (RUNNING, READY, WAITING, FINISHED)
    int Tiempo_ejecucion;    // Tiempo de ejecución por ráfaga
    int Numero_rafagas;   // Número de ráfagas
    int Tiempo_espera;       // Tiempo de espera para I/O entre ráfagas
    int Deadline;      // Deadline de ejecución
    int T_INICIO;      // Tiempo de inicio del proceso
    int response_time; // Tiempo de respuesta del proceso
    int tlcpu;
    int T_pendiente; //Tiempo pendiente de la rafaga actual
    int rafagas_completas; //Numero de rafagas completadas
    int interrupciones;
    int wainting_time; //Tiempo de espera
    int turnaround_time; //Tiempo de retorno



} Process;

// Prototipos de las funciones
Process* create_process(char **data);  // Crear proceso
void update_process_state(Process *p, EstadoProceso new_state);  // Actualizar estado del proceso

#endif // PROCESS_H
