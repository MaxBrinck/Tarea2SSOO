#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

typedef struct {
    char *Nombre;
    int pid;
    int interrupciones;
    int turnaround_time;
    int response_time;
    int waiting_time;
    int suma_deadline;
} ProcessStats;

void schedule(Queue *high_priority_queue, Queue *low_priority_queue, Process **procesos, int num_procesos, int quantum_high, int quantum_low) {
    int tiempo_global = 0; // Tiempo global de la simulación

    // Array para almacenar las estadísticas de cada proceso
    ProcessStats *process_stats = malloc(num_procesos * sizeof(ProcessStats));
    if (!process_stats) {
        perror("Error al asignar memoria para process_stats");
        return;
    }

    for (int i = 0; i < num_procesos; i++) {
        process_stats[i].Nombre = procesos[i]->Nombre;
        process_stats[i].pid = procesos[i]->pid;
        process_stats[i].interrupciones = 0;
        process_stats[i].turnaround_time = 0;
        process_stats[i].response_time = -1; // -1 indica que aún no ha sido asignado
        process_stats[i].waiting_time = 0;
        process_stats[i].suma_deadline = 0;
    }

    while (1) {
        // Actualizar procesos WAITING a READY si ha terminado su tiempo de espera
        for (int i = 0; i < num_procesos; i++) {
            if (procesos[i] != NULL && procesos[i]->Estado == WAITING && procesos[i]->Tiempo_espera <= tiempo_global) {
                procesos[i]->Estado = READY;
                enqueue(high_priority_queue, procesos[i]);
                procesos[i] = NULL;
            }
        }

        // Encolar nuevos procesos
        for (int i = 0; i < num_procesos; i++) {
            if (procesos[i] != NULL && procesos[i]->T_INICIO <= tiempo_global) {
                enqueue(high_priority_queue, procesos[i]);
                procesos[i] = NULL;
            }
        }

        // Si no hay procesos restantes, romper el ciclo
        if (isEmpty(high_priority_queue) && isEmpty(low_priority_queue)) break;

        // Procesar alta prioridad
        if (!isEmpty(high_priority_queue)) {
            Process *current = dequeue(high_priority_queue);
            if (current == NULL) continue;

            if (current->response_time == -1) {
                current->response_time = tiempo_global; // Asignar tiempo de respuesta
            }

            printf("Ejecutando proceso (PID: %d, Nombre: %s) - Estado: RUNNING en t=%d\n", current->pid, current->Nombre, tiempo_global);

            int tiempo_ejecucion = (current->Tiempo_ejecucion > quantum_high) ? quantum_high : current->Tiempo_ejecucion;
            current->Tiempo_ejecucion -= tiempo_ejecucion;
            tiempo_global += tiempo_ejecucion;

            if (current->Tiempo_ejecucion == 0) {
                // Proceso terminado
                process_stats[current->pid - 1].turnaround_time = tiempo_global - current->T_INICIO;
                process_stats[current->pid - 1].waiting_time = tiempo_global - current->T_INICIO - (current->Numero_rafagas - 1) * current->Tiempo_espera;
                printf("Proceso (PID: %d, Nombre: %s) terminado en t=%d\n", current->pid, current->Nombre, tiempo_global);
                free(current);
            } else {
                // Pasar a la cola baja si se consume todo el quantum
                current->Estado = READY;
                enqueue(low_priority_queue, current);
            }
        }
        // Procesar baja prioridad
        else if (!isEmpty(low_priority_queue)) {
            Process *current = dequeue(low_priority_queue);
            if (current == NULL) continue;

            printf("Ejecutando proceso (PID: %d, Nombre: %s) - Estado: RUNNING en t=%d\n", current->pid, current->Nombre, tiempo_global);

            int tiempo_ejecucion = (current->Tiempo_ejecucion > quantum_low) ? quantum_low : current->Tiempo_ejecucion;
            current->Tiempo_ejecucion -= tiempo_ejecucion;
            tiempo_global += tiempo_ejecucion;

            if (current->Tiempo_ejecucion == 0) {
                // Proceso terminado
                process_stats[current->pid - 1].turnaround_time = tiempo_global - current->T_INICIO;
                process_stats[current->pid - 1].waiting_time = tiempo_global - current->T_INICIO - (current->Numero_rafagas - 1) * current->Tiempo_espera;
                printf("Proceso (PID: %d, Nombre: %s) terminado en t=%d\n", current->pid, current->Nombre, tiempo_global);
                free(current);
            } else {
                // Verificar si sube a la cola alta
                if (2 * current->Deadline < tiempo_global - current->T_INICIO) {
                    current->Estado = READY;
                    enqueue(high_priority_queue, current);
                } else {
                    current->Estado = READY;
                    enqueue(low_priority_queue, current);
                }
            }
        } else {
            tiempo_global++;
        }
    }

    // Escribir estadísticas en el archivo
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("No se pudo abrir el archivo de salida");
        free(process_stats);
        return;
    }

    for (int i = 0; i < num_procesos; i++) {
        fprintf(output_file, "%s,%d,%d,%d,%d,%d,%d\n",
                process_stats[i].Nombre,
                process_stats[i].pid,
                process_stats[i].interrupciones,
                process_stats[i].turnaround_time,
                process_stats[i].response_time,
                process_stats[i].waiting_time,
                process_stats[i].suma_deadline);
    }

    fclose(output_file);
    free(process_stats);
}


int main(int argc, char const *argv[]) {
    char *file_name = (char *)argv[1];
    InputFile *input_file = read_file(file_name);

    printf("Nombre archivo: %s\n", file_name);
    printf("Cantidad de procesos: %d\n", input_file->len);
    printf("Procesos:\n");

    Queue *high_priority_queue = create_queue();
    Queue *low_priority_queue = create_queue();

    int q = 2; // Definir un quantum base
    int quantum_high = 2 * q;
    int quantum_low = q;

    Process **procesos = (Process **)malloc(input_file->len * sizeof(Process *));
    
    for (int i = 0; i < input_file->len; ++i) {
        Process *proceso = (Process *)malloc(sizeof(Process));

        int pid = atoi(input_file->lines[i][1]);
        int tiempo_ejecucion = atoi(input_file->lines[i][3]);
        int numero_rafagas = atoi(input_file->lines[i][4]);
        int tiempo_espera = atoi(input_file->lines[i][5]);
        int deadline = atoi(input_file->lines[i][6]);
        int t_inicio = atoi(input_file->lines[i][2]);  // Agregar T_INICIO del archivo de entrada

        proceso->Nombre = (char *)malloc(strlen(input_file->lines[i][0]) + 1);
        strcpy(proceso->Nombre, input_file->lines[i][0]);
        proceso->pid = pid;
        proceso->Estado = READY;
        proceso->Tiempo_ejecucion = tiempo_ejecucion;
        proceso->Numero_rafagas = numero_rafagas;
        proceso->Tiempo_espera = tiempo_espera;
        proceso->Deadline = deadline;
        proceso->T_INICIO = t_inicio;

        procesos[i] = proceso; 
    }

    schedule(high_priority_queue, low_priority_queue, procesos, input_file->len, quantum_high, quantum_low);

    input_file_destroy(input_file);
    free(procesos);

    return 0;
}